#include "net-scene.hpp"
#include "commom.hpp"
#include "fmt/color.h"
#include "message.hpp"
#include "msgpack/msgpack.hpp"
#include "sockpp/sock_address.h"
#include "sockpp/udp_socket.h"
#include "udpmsg.hpp"

#include <algorithm>
#include <asm-generic/socket.h>
#include <bits/types/struct_timeval.h>
#include <cstdint>
#include <cstdlib>
#include <exception>
#include <fmt/ranges.h>
#include <string>
#include <thread>

namespace uppr::app {

template <typename R>
bool is_ready(std::future<R> const &f) {
    return f.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
}

void NetScene::update(eng::Engine &engine) {
    auto &outbound = state->get_outbound_message_list();

    for (auto list = outbound.begin(); list != outbound.end(); list++) {
        // Run for each pending list to check for how many are complete
        for (auto item = list->begin(); item != list->end(); item++) {
            if (is_ready(*item)) {
                // Collect the result from the item
                const auto [id, result] = item->get();
                if (result.empty()) {
                    LOG_F(INFO, "ended {} with no errors", id);
                    // No errors
                    state->set_message_with_sent(id);
                } else {
                    LOG_F(ERROR, "ended {} with errors: {}", id, result);
                    // Had errors!
                    state->set_message_with_error(id, result);
                }

                // Remove from the list
                item = list->erase(item);
            }
        }

        // Remove if there are no more items
        if (list->empty()) list = outbound.erase(list);
    }

    while (!inbound_messages.empty()) {
        state->recv_message(inbound_messages.dequeue());
    }
}

void NetScene::draw(eng::Engine &engine, term::Transform transform,
                    term::Size size, term::TermScreen &screen) {
    using namespace fmt;

    screen.print(transform.move(0, size.gety() - 2), emphasis::reverse, "{}",
                 state->get_outbound_message_list().size());
}

void NetScene::mount(eng::Engine &engine) {
    const auto port_str = std::getenv("PORT");
    const auto port = port_str ? std::stoi(port_str) : 8080;

    LOG_F(INFO, "Got listening port {}", port);

    listener_alive = true;

    listener = std::thread{[this, port] {
        sockpp::inet_address addr{"0.0.0.0", static_cast<in_port_t>(port)};
        sockpp::inet_address recv_addr;

        sockpp::udp_socket sock;

        struct timeval tv;
        tv.tv_usec = 100'000; // 100ms
        sock.set_option(SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

        if (!sock.bind(addr)) {
            LOG_F(ERROR, "Error binding receiving socket");
            return;
        }

        std::vector<uint8_t> buf;
        buf.resize(64);

        while (listener_alive) {

            try {
                const auto n =
                    sock.recv_from(buf.data(), buf.size(), &recv_addr);
                if (n <= 0) {
                    // timeout should be ignored
                    // LOG_F(INFO, "n <= 0 = {}", n);
                    continue;
                }

                LOG_F(7, "Got message data ({} bytes) from {}: {}", n,
                      recv_addr.to_string(), buf);

                std::error_code ec;
                const auto message =
                    msgpack::unpack<models::UdpMessage>(buf.data(), n, ec);
                if (ec) {
                    LOG_F(ERROR, "Got invalid message: {}", ec.message());
                    continue;
                }

                LOG_F(4, "Got message: {}, {}", message.content, message.sent_by);

                inbound_messages.enqueue(message);
            } catch (const std::exception &e) {
                LOG_F(ERROR, "Error receiving message: '{}'", e.what());
            }
        }
    }};
}

void NetScene::unmount(eng::Engine &engine) { join_listener(); }

void NetScene::join_listener() {
    listener_alive = false;
    if (listener.joinable()) listener.join();
}
} // namespace uppr::app
