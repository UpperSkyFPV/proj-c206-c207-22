#include "remove-user-from-chat-scene.hpp"
#include <set>

namespace uppr::app {
void RemoveUserFromChatScene::draw(eng::Engine &engine,
                                   term::Transform transform, term::Size size,
                                   term::TermScreen &screen) {
    using namespace fmt;

    const auto current_chat = state->get_selected_chatmodel();
    if (!current_chat) { LOG_F(ERROR, "NO CHAT SELECTED, BUT MODAL IS OPEN!"); }

    // Draw a border box
    screen.box(transform, size.getx(), size.gety(), {});
    screen.print(transform.move(2, 0), "Remove user from {}", current_chat->name);
    transform += {2, 1};

    usize idx{};
    for (const auto &[user, present] : users) {
        text_style style;
        if (!present) style |= emphasis::faint;
        if (idx == selected_user) style |= emphasis::reverse;

        screen.print(transform, style, "[{}] {}", user.id, user.name);

        transform += {0, 1};
        idx++;
    }

    transform += {0, 2};
    screen.print(transform, emphasis::reverse, "Confirm <enter>");
    transform += {20, 0};
    screen.print(transform, "Exit <ESC>");
}

void RemoveUserFromChatScene::mount(eng::Engine &engine) {
    LOG_F(INFO, "mounted RemoveUserFromChatScene");

    update_users();

    select_next_keybind_handle =
        engine.get_eventbus().appendListener('\r', [this](char c) {
            if (has_user_available) confirm();
        });

    select_next_keybind_handle = engine.get_eventbus().appendListener(
        '\t', [this](char c) { select_next_user(); });

    select_prev_keybind_handle = engine.get_eventbus().appendListener(
        eng::Event::NonChar::shift_tab, [this](char c) { select_prev_user(); });
}

void RemoveUserFromChatScene::unmount(eng::Engine &engine) {
    engine.get_eventbus().removeListener('\t', select_next_keybind_handle);
    engine.get_eventbus().removeListener(eng::Event::NonChar::shift_tab,
                                         select_prev_keybind_handle);
}

void RemoveUserFromChatScene::update_users() {
    std::set<models::UserModel> users_present;
    for (const auto &user : state->get_users_of_selected_chat()) {
        LOG_F(8, "adding '{}' user to present set", user.name);
        users_present.insert(user);
    }

    users.clear();

    // Make shure we have loaded users
    state->fetch_users();
    const auto all_users = state->get_users();

    for (const auto &user : all_users) {
        const auto present = users_present.find(user) != users_present.end();
        LOG_F(8, "adding '{}' user to full list [{}]", user.name, present);
        users.push_back({user, present});
    }

    // Dont enter infinite loops
    has_user_available = users_present.size() > 0;
    if (has_user_available) select_next_user();
}

void RemoveUserFromChatScene::select_next_user() {
    if (!has_user_available || users.size() == 0) {
        selected_user = -1;
        return;
    }

    do {
        selected_user = (selected_user + 1) % users.size();
    } while (selected_user >= 0 && !users[selected_user].second);
}

void RemoveUserFromChatScene::select_prev_user() {
    if (!has_user_available) {
        selected_user = -1;
        return;
    }

    do {
        selected_user--;
        if (selected_user < 0) selected_user = users.size() - 1;
    } while (selected_user >= 0 && !users[selected_user].second);
}

void RemoveUserFromChatScene::confirm() {
    if (selected_user < 0) {
        LOG_F(ERROR, "No selected user on confirm!");
        return;
    }

    state->remove_user_from_selected_chat(users[selected_user].first);
    update_users();
}
} // namespace uppr::app
