#pragma once

#include "chatinfo-scene.hpp"
#include "conn.hpp"
#include "eng/engine.hpp"
#include "eng/scene.hpp"
#include "file.hpp"
#include "key.hpp"
#include "result.hpp"
#include "state.hpp"
#include "vector2.hpp"

namespace uppr::app {

class ChatScene : public eng::Scene {
public:
    ChatScene(shared_ptr<AppState> s)
        : state{s}, chat_info{std::make_unique<ChatInfoScene>(s)} {}

    void update(eng::Engine &engine) override { chat_info->update(engine); }

    void draw(eng::Engine &engine, term::Transform transform, term::Size size,
              term::TermScreen &screen) override {
        using namespace fmt;

        const auto info_size = term::Size{size.getx(), size.gety() / 5};

        chat_info->draw(engine, transform, info_size, screen);
        transform += {0, static_cast<int>(info_size.gety())};

        screen.hline(transform.getx() + 1, transform.getx() + size.getx() - 1,
                     transform.gety(), '=');
    }

    void mount(eng::Engine &engine) override { chat_info->mount(engine); }

    void unmount(eng::Engine &engine) override { chat_info->unmount(engine); }

    static std::shared_ptr<ChatScene> make(shared_ptr<AppState> s) {
        return std::make_shared<ChatScene>(s);
    }

private:
    eng::Engine::EventBus::Handle u_handle;

    shared_ptr<AppState> state;
    unique_ptr<ChatInfoScene> chat_info;
};
} // namespace uppr::app
