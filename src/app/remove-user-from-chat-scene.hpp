#pragma once

#include "engine.hpp"
#include "models/user.hpp"
#include "scene.hpp"
#include "state.hpp"
#include <vector>

namespace uppr::app {
/**
 * This is a simple form that requires the input for creating a new user.
 */
class RemoveUserFromChatScene : public eng::Scene {
public:
    RemoveUserFromChatScene(shared_ptr<AppState> s) : state{s} {}

    void update(eng::Engine &engine) override {}

    void draw(eng::Engine &engine, term::Transform transform, term::Size size,
              term::TermScreen &screen) override;

    void mount(eng::Engine &engine) override;

    void unmount(eng::Engine &engine) override;

    static std::shared_ptr<RemoveUserFromChatScene> make(shared_ptr<AppState> s) {
        return std::make_shared<RemoveUserFromChatScene>(s);
    }

private:
    /**
     * Fetch from the database all of the users into our local copy.
     */
    void update_users();

    /**
     * Move the `selected_item` to the next possible user.
     */
    void select_next_user();

    /**
     * Move the `selected_item` to the previous possible user.
     */
    void select_prev_user();

    /**
     * Called when we want to save data!
     */
    void confirm();

private:
    /**
     * Shared app state.
     */
    shared_ptr<AppState> state;

    /**
     * The list of all users that exist, paired with if they are already present
     * in this current chat.
     */
    std::vector<std::pair<models::UserModel, bool>> users;

    /**
     * Handle for the callback for selecting the next input
     */
    eng::Engine::EventBus::Handle select_next_keybind_handle;

    /**
     * Handle for the callback for selecting the previous input
     */
    eng::Engine::EventBus::Handle select_prev_keybind_handle;

    /**
     * The currently selected input item.
     */
    int selected_user{-1};

    /**
     * This is needed so we dont enter an infinite loop when there are no users
     * available to be added.
     */
    bool has_user_available{};
};
} // namespace uppr::app
