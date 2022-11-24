#pragma once

#include "conn.hpp"
#include "dao/address.hpp"
#include "dao/chat.hpp"
#include "dao/message.hpp"
#include "dao/user.hpp"
#include "message.hpp"
#include "models/address.hpp"
#include "models/chat.hpp"
#include "models/user.hpp"
#include "result.hpp"
#include "safe-queue.hpp"
#include "udpmsg.hpp"
#include <algorithm>
#include <eventpp/eventqueue.h>
#include <exception>
#include <functional>
#include <future>
#include <memory>
#include <msgpack/msgpack.hpp>
#include <optional>
#include <sockpp/udp_socket.h>
#include <string_view>
#include <utility>
#include <vector>

namespace uppr::app {

class AppState {
public:
    AppState(shared_ptr<db::Connection> db_, int port_,
             const std::string &name_)
        : db{db_}, chat_dao{db_}, address_dao{db_}, user_dao{db_},
          message_dao{db_}, name{name_}, port{port_} {}

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
        fetch_users();

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
        fetch_users();

        return selected_chat;
    }

    /**
     * Deselect the chat (no chat will be marked as selected).
     */
    void deselect_chat() {
        selected_chat = -1;

        // Needs to update this every time we change the selected chat
        fetch_users_of_chat();
        fetch_users();
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

    models::AddressModel get_address_of(int chatid) {
        return address_dao.with_id(chatid);
    }

    std::optional<refw<const models::UserModel>> find_user(int id) {
        //LOG_F(8, "users={}", users.size());
        const auto a = std::ranges::find_if(
            users, [id](const auto &c) { return c.id == id; });
        if (a == users.end()) return std::nullopt;

        return std::ref(*a);
    }

    /**
     * Insert a new chat into the database.
     */
    void insert_new_chat(const models::ChatModel &chat) {
        chat_dao.insert(chat);

        // update our copy after inserting
        fetch_chats();
    }

    /**
     * Push a string message (sent from the current chat).
     */
    void push_message(string_view message) {
        if (!has_chat_selected()) {
            LOG_F(INFO, "No chat selected, but push_message was called!");
            return;
        }

        models::MessageModel model{
            .id = -1,
            .content = std::string{message},
            .sent = false,
            .received = false,
            .error = "",
            .in_chat = get_selected_chatmodel()->id,
            .sent_by = -1, // this should be filled in on receiving, so we set
                           // it to nothing here (this way we signal that -1
                           // means "sent by us")
        };

        models::UdpMessage msg{
            .content = model.content,
            .sent_by = name,
            .sent_from = get_selected_chatmodel()->name,
        };

        const auto id = message_dao.insert(model);
        send_message(id, msg);
    }

    void recv_message(const models::UdpMessage &msg) {
        LOG_F(INFO, "> {} on {}: {}", msg.sent_by, msg.sent_from, msg.content);

        try {
            const auto users_chats = chat_dao.all_that_contain_user_and_chat(
                msg.sent_by, msg.sent_from);
            for (const auto &[user, chat] : users_chats) {
                LOG_F(2, "Insering message from {}[{}] to {}[{}]", user.name,
                      user.id, chat.name, chat.id);
                models::MessageModel message{
                    .id = -1,
                    .content = msg.content,
                    .sent = true,
                    .received = true,
                    .error = ""s,
                    .in_chat = chat.id,
                    .sent_by = user.id,
                };

                message_dao.insert(message);
            }
        } catch (const db::DatabaseError &e) {
            LOG_F(ERROR, "database error: {} {}", e.what(),
                  e.get_result().str());
        }

        // Update everything?
        fetch_users();
        fetch_chats();
        fetch_users_of_chat();
    }

    auto &get_outbound_message_list() { return outbound_messages; }

    void set_message_with_error(int msg_id, const std::string &error) {
        message_dao.update_with_error(msg_id, error);
    }

    void set_message_with_sent(int msg_id) {
        message_dao.update_with_sent(msg_id, true);
    }

    std::vector<models::MessageModel> get_messages_of_current_chat() {
        const auto sel = get_selected_chatmodel();
        if (!sel) return {};

        return message_dao.all_for_chat(sel->id);
    }

    string_view get_name() const noexcept { return name; }

private:
    void send_message(int local_id, models::UdpMessage msg) {
        std::list<std::future<std::pair<int, std::string>>> results;

        for (const auto &member : members_of_chat) {
            // Dont sent to our own address
            if (member.name == name) continue;

            const auto addr = address_dao.with_id(member.user_address);

            LOG_F(5, "sending msg '{}' to {}:{}", msg.content, addr.host,
                  addr.port);

            auto result = std::async(std::launch::async, [=]() mutable {
                const auto payload = msgpack::pack(msg);

                try {
                    sockpp::udp_socket sock;
                    const sockpp::inet_address udp_addr{
                        addr.host, static_cast<in_port_t>(addr.port)};

                    sock.send_to(payload.data(), payload.size(), udp_addr);
                } catch (const std::exception &e) {
                    LOG_F(ERROR, "error sending message '{}' to {}:{}: {}",
                          msg.content, addr.host, addr.port, e.what());

                    return std::make_pair(local_id, std::string{e.what()});
                }

                return std::make_pair(local_id, ""s);
            });

            results.push_back(std::move(result));
        }

        outbound_messages.push_back(std::move(results));
    }

    int get_port() const { return port; }

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
     * Store outbound messages that were sent, but not yet decided if
     * successfull.
     */
    std::list<std::list<std::future<std::pair<int, std::string>>>>
        outbound_messages;

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

    /**
     * DAO for the messages.
     */
    dao::MessageDAO message_dao;

    /**
     * This is our name.
     */
    std::string name;

    /**
     * This is our port.
     */
    int port;
};
} // namespace uppr::app
