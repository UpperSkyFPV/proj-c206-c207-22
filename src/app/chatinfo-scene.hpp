#pragma once

#include "engine.hpp"
#include "models/chat.hpp"
#include "models/user.hpp"
#include "scene.hpp"
#include "state.hpp"

namespace uppr::app {

class ChatInfoScene : public eng::Scene {
public:
    ChatInfoScene(shared_ptr<AppState> s) : state{s} {}

    void update(eng::Engine &engine) override;

    void draw(eng::Engine &engine, term::Transform transform, term::Size size,
              term::TermScreen &screen) override;

    void mount(eng::Engine &engine) override;

    void unmount(eng::Engine &engine) override;

    static std::shared_ptr<ChatInfoScene> make(shared_ptr<AppState> s) {
        return std::make_shared<ChatInfoScene>(s);
    }

private:
    void draw_chat_info(const models::ChatModel &chat, eng::Engine &engine,
                        term::Transform transform, term::Size size,
                        term::TermScreen &screen);

    void update_users() {
        LOG_SCOPE_F(9, "update_users ChatViewInfoScene");

        const auto selected = state->get_selected_chatmodel();
        if (selected) users_in_current_chat = state->get_users_of_chat(*selected);

        for (const auto &u : users_in_current_chat) {
            LOG_F(INFO, "{}, {}, {}", u.id, u.name, u.user_address);
        }

        last_selected_chat = state->get_selected_chat();
    }

private:
    /**
     * Handle for the callback for adding a new user to a chat.
     */
    eng::Engine::EventBus::Handle add_user_keybind_handle;

    shared_ptr<AppState> state;

    int last_selected_chat{-1};

    std::vector<models::UserModel> users_in_current_chat;
};
} // namespace uppr::app
