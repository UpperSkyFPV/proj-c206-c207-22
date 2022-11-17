#pragma once

#include "address.hpp"
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

    void update_users();

private:
    shared_ptr<AppState> state;

    int last_selected_chat{-1};

    std::vector<std::pair<models::AddressModel, models::UserModel>>
        users_in_current_chat;
};
} // namespace uppr::app
