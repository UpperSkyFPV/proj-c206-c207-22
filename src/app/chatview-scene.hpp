#pragma once

#include "engine.hpp"
#include "scene.hpp"
#include "state.hpp"
#include <memory>

namespace uppr::app {

class ChatViewScene : public eng::Scene {
public:
    ChatViewScene(shared_ptr<AppState> s) : state{s} {}

    void update(eng::Engine &engine) override;

    void draw(eng::Engine &engine, term::Transform transform, term::Size size,
              term::TermScreen &screen) override;

    void mount(eng::Engine &engine) override;

    void unmount(eng::Engine &engine) override;

    static std::shared_ptr<ChatViewScene> make(shared_ptr<AppState> s) {
        return std::make_shared<ChatViewScene>(s);
    }

private:
    /**
     * Update the chats stored in the state.
     *
     * Also, resets the `chats_needs_update` flag.
     */
    void update_chat() {
        state->fetch_chats();

        chats_needs_update = false;
    }

private:
    /**
     * Shared app state.
     */
    shared_ptr<AppState> state;

    /**
     * Handle for the callback for cycling the chat.
     */
    eng::Engine::EventBus::Handle cycle_chat_keybind_handle;

    /**
     * If we need to update the chats from the database.
     */
    bool chats_needs_update{true};
};
} // namespace uppr::app
