#include "chatinfo-scene.hpp"
#include "fmt/color.h"
#include "fmt/core.h"
#include "key.hpp"

namespace uppr::app {
void ChatInfoScene::update(eng::Engine &engine) {
    if (last_selected_chat != state->get_selected_chat()) { update_users(); }
}

void ChatInfoScene::draw(eng::Engine &engine, term::Transform transform,
                         term::Size size, term::TermScreen &screen) {
    using namespace fmt;

    const auto selected_chat = state->get_selected_chatmodel();
    if (selected_chat) {
        draw_chat_info(*selected_chat, engine, transform, size, screen);
    } else {
        constexpr auto fmt = "Nothing selected"sv;
        const auto x = size.getx() / 2 - fmt.length() / 2;

        screen.print(transform.move(x, 2), fmt);
    }
}

void ChatInfoScene::mount(eng::Engine &engine) { update_users(); }

void ChatInfoScene::unmount(eng::Engine &engine) {}

void ChatInfoScene::draw_chat_info(const models::ChatModel &chat,
                                   eng::Engine &engine,
                                   term::Transform transform, term::Size size,
                                   term::TermScreen &screen) {
    using namespace fmt;

    transform += {0, 1};
    screen.print(transform, emphasis::bold | emphasis::underline, "{}: {}",
                 chat.id, chat.name);

    transform += {0, 2};

    std::vector<std::string> stringified;
    stringified.reserve(users_in_current_chat.size());

    for (const auto &[addr, user] : users_in_current_chat)
        stringified.push_back(format("{} [{}]", user.name, addr.port));

    screen.print(transform, "members: {}", join(stringified, ", "));
}

void ChatInfoScene::update_users() {
    LOG_SCOPE_F(9, "update_users ChatViewInfoScene");

    const auto selected = state->get_selected_chatmodel();
    if (selected) {
        const auto users = state->get_users_of_chat(*selected);

        users_in_current_chat.clear();
        for (const auto &u : users) {
            users_in_current_chat.push_back(
                {state->get_address_of(u.user_address), u});
        }
    }

    for (const auto &[a, u] : users_in_current_chat) {
        LOG_F(INFO, "{}, {}, {}, {}:{}", u.id, u.name, u.user_address, a.host,
              a.port);
    }

    last_selected_chat = state->get_selected_chat();
}
} // namespace uppr::app
