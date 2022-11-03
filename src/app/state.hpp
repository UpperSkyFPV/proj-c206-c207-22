#pragma once

#include "conn.hpp"
#include "dao/address.hpp"
#include "dao/chat.hpp"
#include "dao/user.hpp"
#include "models/chat.hpp"
#include <algorithm>
#include <functional>
#include <memory>
#include <optional>
#include <vector>

namespace uppr::app {

class AppState {
public:
    AppState(shared_ptr<db::Connection> db)
        : chat_dao{db}, address_dao{db}, user_dao{db} {}

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
    const std::vector<models::ChatModel>& get_chats() const { return chats; }

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