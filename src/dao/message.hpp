#pragma once

#include "dao.hpp"
#include "models/message.hpp"
#include <vector>

namespace uppr::dao {

/**
 * DAO (Data Access Object) for the `Message` table.
 */
class MessageDAO : DAO {
public:
    MessageDAO(shared_ptr<db::Connection> db_) : DAO{db_} {}

    std::vector<models::MessageModel> all() const {
        return select_with<models::MessageModel>("SELECT * FROM User");
    }

    std::vector<models::MessageModel> all_for_chat(int chat_id) const {
        LOG_F(9, "Getting all messages on chat {}", chat_id);

        constexpr auto sql =
            R"~~(SELECT * FROM Message as M WHERE M.`in_chat` = ? ORDER BY M.`id` DESC)~~"sv;
        const auto stmt = db->prepare(sql);
        stmt.bind_int(1, chat_id);

        std::vector<models::MessageModel> messages;
        auto s = stmt.step();
        while (s.is_row()) {
            messages.push_back(models::MessageModel::from_row(stmt));

            s = stmt.step();
        }

        return messages;
    }

    void update_with_error(int msg_id, const std::string& error) const {
        constexpr auto sql = 
            R"~~(UPDATE Message SET error = ? WHERE id = ?)~~"sv;
        const auto stmt = db->prepare(sql);
        stmt.bind_text(1, error);
        stmt.bind_int(2, msg_id);
        stmt.step();
    }

    void update_with_sent(int msg_id, bool value) const {
        constexpr auto sql = 
            R"~~(UPDATE Message SET sent = ? WHERE id = ?)~~"sv;
        const auto stmt = db->prepare(sql);
        stmt.bind_int(1, value);
        stmt.bind_int(2, msg_id);
        stmt.step();
    }


    // returns the id of the newly inserted row
    int insert(const models::MessageModel &m) const {
        const auto stmt =
            db->prepare("INSERT INTO Message(content, sent, received, error, "
                        "in_chat, sent_by) VALUES (?, ?, ?, ?, ?, ?)");
        stmt.bind_text(1, m.content);
        stmt.bind_int(2, m.sent);
        stmt.bind_int(3, m.received);
        stmt.bind_text(4, m.error);
        stmt.bind_int(5, m.in_chat);
        stmt.bind_int(6, m.sent_by);
        stmt.step();

        return db->last_inserted_id();
    }
};
} // namespace uppr::dao
