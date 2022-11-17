#pragma once

#include "eventpp/eventqueue.h"
#include "message.hpp"
#include "safe-queue.hpp"
#include "scene.hpp"
#include "state.hpp"
#include "udpmsg.hpp"
#include <thread>

namespace uppr::app {

class NetScene : public eng::Scene {
public:
    NetScene(shared_ptr<AppState> s) : state{s} {}
    virtual ~NetScene() { join_listener(); }

    void update(eng::Engine &engine) override;

    void draw(eng::Engine &engine, term::Transform transform, term::Size size,
              term::TermScreen &screen) override;

    void mount(eng::Engine &engine) override;

    void unmount(eng::Engine &engine) override;

    static std::shared_ptr<NetScene> make(shared_ptr<AppState> s) {
        return std::make_shared<NetScene>(s);
    }

private:
    void join_listener();

private:
    shared_ptr<AppState> state;
    std::thread listener;
    std::atomic<bool> listener_alive;

    SafeQueue<models::UdpMessage> inbound_messages;
};
} // namespace uppr::app
