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
        if (selected_chat < 0) return selected_chat = 0;

        return selected_chat = (selected_chat + 1) % chats.size();
    }

    /**
     * Select the previous chat (if no chat is selected, then select the last
     * one).
     *
     * @return The index (not the ID!) of the newly selected chat.
     */
    int select_prev_chat() {
        if (--selected_chat < 0) return selected_chat = chats.size() - 1;

        return selected_chat;
    }

    /**
     * Deselect the chat (no chat will be marked as selected).
     */
    void deselect_chat() { selected_chat = -1; }

    /**
     * If there is a chat selected.
     */
    bool has_chat_selected() { return selected_chat > 0; }

    /**
     * Get what chat is currently selected.
     */
    int get_selected_chat() const { return selected_chat; }

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
     * Update the list of users with new values from the database
     */
    void fetch_users() { users = user_dao.all(); }

    /**
     * Insert a new user into the database.
     */
    void insert_new_user(models::UserModel user,
                         const models::AddressModel &address) {
        address_dao.insert(address);
        user.user_address = db->last_inserted_id();

        user_dao.insert(user);

        fetch_users();
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
