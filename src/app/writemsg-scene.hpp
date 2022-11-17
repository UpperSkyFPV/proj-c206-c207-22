#pragma once

#include "engine.hpp"
#include "scene.hpp"
#include "state.hpp"
#include <memory>
namespace uppr::app {

class WriteMsgScene : public eng::Scene {
public:
    WriteMsgScene(shared_ptr<AppState> s) : state{s} {}

    void update(eng::Engine &engine) override {}

    void draw(eng::Engine &engine, term::Transform transform, term::Size size,
              term::TermScreen &screen) override;

    void mount(eng::Engine &engine) override;

    void unmount(eng::Engine &engine) override;

    static std::shared_ptr<WriteMsgScene> make(shared_ptr<AppState> s) {
        return std::make_shared<WriteMsgScene>(s);
    }

private:
    shared_ptr<AppState> state;

    eng::Engine::EventBus::Handle start_writing_keybind_handle;

    bool wants_input{false};
};
} // namespace uppr::app
