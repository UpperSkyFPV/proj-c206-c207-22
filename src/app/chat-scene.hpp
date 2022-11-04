#pragma once

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
    ChatScene(shared_ptr<AppState> s) : state{s} {}

    void update(eng::Engine &engine) override {}

    void draw(eng::Engine &engine, term::Transform transform, term::Size size,
              term::TermScreen &screen) override {
        using namespace fmt;

        screen.print(transform, "Hello!");
    }

    void mount(eng::Engine &engine) override {}

    void unmount(eng::Engine &engine) override {}

    static std::shared_ptr<ChatScene> make(shared_ptr<AppState> s) {
        return std::make_shared<ChatScene>(s);
    }

private:
    eng::Engine::EventBus::Handle u_handle;

    shared_ptr<AppState> state;
};
} // namespace uppr::app
