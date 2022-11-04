#pragma once

#include "scene.hpp"
#include "state.hpp"
namespace uppr::app {

class ChatInfoScene : public eng::Scene {
public:
    ChatInfoScene(shared_ptr<AppState> s) : state{s} {}

    void update(eng::Engine &engine) override {}

    void draw(eng::Engine &engine, term::Transform transform, term::Size size,
              term::TermScreen &screen) override {
        using namespace fmt;

        screen.print(transform, "I am INFO!");
    }

    void mount(eng::Engine &engine) override {}

    void unmount(eng::Engine &engine) override {}

    static std::shared_ptr<ChatInfoScene> make(shared_ptr<AppState> s) {
        return std::make_shared<ChatInfoScene>(s);
    }

private:
    shared_ptr<AppState> state;
};
} // namespace uppr::app
