#pragma once

#include "conn.hpp"
#include "dao/address.hpp"
#include "dao/chat.hpp"
#include "dao/user.hpp"
#include "models/address.hpp"
#include "models/chat.hpp"
#include "models/user.hpp"
#include <algorithm>
#include <functional>
#include <memory>
#include <optional>
#include <vector>

namespace uppr::app {

class AppState {
public:
    AppState(shared_ptr<db::Connection> db_)
        : db{db_}, chat_dao{db_}, address_dao{db_}, user_dao{db_} {}

public:
    /**
     * Select the next chat (if no chat is selected, then select the first one).
     *
     * @return The index (not the ID!) of the newly selected chat.
     */
    int select_next_chat() {
        if (selected_chat < 0)
            selected_chat = 0;
        else
            selected_chat = (selected_chat + 1) % chats.size();

        // Needs to update this every time we change the selected chat
        fetch_users_of_chat();

        return selected_chat;
    }

    /**
     * Select the previous chat (if no chat is selected, then select the last
     * one).
     *
     * @return The index (not the ID!) of the newly selected chat.
     */
    int select_prev_chat() {
        if (--selected_chat < 0) selected_chat = chats.size() - 1;

        // Needs to update this every time we change the selected chat
        fetch_users_of_chat();

        return selected_chat;
    }

    /**
     * Deselect the chat (no chat will be marked as selected).
     */
    void deselect_chat() {
        selected_chat = -1;

        // Needs to update this every time we change the selected chat
        fetch_users_of_chat();
    }

    /**
     * If there is a chat selected.
     */
    bool has_chat_selected() const { return selected_chat >= 0; }

    /**
     * Get what chat is currently selected.
     */
    int get_selected_chat() const { return selected_chat; }

    /**
     * Get the selected chat object. Returns `nullptr` if no chat is currently
     * selected.
     */
    const models::ChatModel *get_selected_chatmodel() const {
        return has_chat_selected() ? &get_chat(get_selected_chat()) : nullptr;
    }

    /**
     * Get all chats.
     */
    const std::vector<models::ChatModel> &get_chats() const { return chats; }

    /**
     * Get the chat at the given index.
     */
    const models::ChatModel &get_chat(usize idx) const { return chats.at(idx); }

    /**
     * Find the chat with the given id.
     */
    optional<refw<const models::ChatModel>> find_chat(usize id) const {
        const auto a = std::ranges::find_if(
            chats, [id](const auto &c) { return c.id == id; });
        if (a == chats.end()) return std::nullopt;

        return std::ref(*a);
    }

    /**
     * Update the list of chats with new values from the database
     */
    void fetch_chats() { chats = chat_dao.all(); }

    /**
     * Get all users.
     */
    const std::vector<models::UserModel> &get_users() const { return users; }

    /**
     * Get all users that are members of the given chat.
     */
    std::vector<models::UserModel>
    get_users_of_chat(const models::ChatModel &chat) const {
        return user_dao.all_for_chat(chat.id);
    }

    /**
     * Get all users that are members of the currently selected chat.
     */
    const std::vector<models::UserModel> &get_users_of_selected_chat() const {
        return members_of_chat;
    }

    /**
     * Get the user at the given index.
     */
    const models::UserModel &get_user(usize idx) const { return users.at(idx); }

    /**
     * Update the list of users with new values from the database
     */
    void fetch_users() { users = user_dao.all(); }

    /**
     * Update the list of users of current chat.
     */
    void fetch_users_of_chat() {
        const auto sel = get_selected_chatmodel();
        if (sel)
            members_of_chat = get_users_of_chat(*sel);
        else
            members_of_chat.clear();
    }

    /**
     * Add the given user to the currently selected chat.
     */
    void add_user_to_selected_chat(const models::UserModel &user) {
        const auto chat = get_selected_chatmodel();
        if (!chat) {
            LOG_F(ERROR,
                  "No chat selected when add_user_to_selected_chat was called");
            return;
        }

        chat_dao.add_user(*chat, user);

        // Refetch everything?
        fetch_users();
        fetch_chats();
        fetch_users_of_chat();
    }

    void remove_user_from_selected_chat(const models::UserModel &user) {
        const auto chat = get_selected_chatmodel();
        if (!chat) {
            LOG_F(ERROR, "No chat selected when remove_user_from_selected_chat "
                         "was called");
            return;
        }

        chat_dao.remove_user(*chat, user);

        // Refetch everything?
        fetch_users();
        fetch_chats();
        fetch_users_of_chat();
    }

    /**
     * Insert a new user into the database.
     */
    void insert_new_user(models::UserModel user,
                         const models::AddressModel &address) {
        address_dao.insert(address);
        user.user_address = db->last_inserted_id();

        user_dao.insert(user);

        // update our copy after inserting
        fetch_users();
    }

    /**
     * Insert a new chat into the database.
     */
    void insert_new_chat(const models::ChatModel &chat) {
        chat_dao.insert(chat);

        // update our copy after inserting
        fetch_chats();
    }

private:
    /**
     * The index of the currently selected chat. This will be negative if no
     * chat is selected.
     */
    int selected_chat{-1};

    /**
     * Store all of the chats.
     */
    std::vector<models::ChatModel> chats;

    /**
     * Store all of the users.
     */
    std::vector<models::UserModel> users;

    /**
     * Store all of the users that are members of the current chat.
     */
    std::vector<models::UserModel> members_of_chat;

    /**
     * Store a reference to the database connection.
     */
    shared_ptr<db::Connection> db;

    /**
     * DAO for the chats.
     */
    dao::ChatDAO chat_dao;

    /**
     * DAO for the addresses.
     */
    dao::AddressDAO address_dao;

    /**
     * DAO for the users.
     */
    dao::UserDAO user_dao;
};
} // namespace uppr::app
