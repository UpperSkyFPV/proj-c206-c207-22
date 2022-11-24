#pragma once

#include "conn.hpp"
#include "dao.hpp"
#include "models/chat.hpp"
#include "models/user.hpp"
#include "stmt.hpp"
#include <string_view>
#include <vector>

namespace uppr::dao {

/**
 * DAO (Data Access Object) for the `Chat` table.
 */
class ChatDAO : DAO {
public:
    ChatDAO(shared_ptr<db::Connection> db_) : DAO{db_} {}

    std::vector<models::ChatModel> all() const {
        return select_with<models::ChatModel>("SELECT * FROM Chat"sv);
    }

    void insert(const models::ChatModel &m) const {
        const auto stmt =
            db->prepare("INSERT INTO Chat(name, description) VALUES (?, ?)"sv);
        stmt.bind_text(1, m.name);
        stmt.bind_text(2, m.description);
        stmt.step();
    }

    void add_user(const models::ChatModel &chat,
                  const models::UserModel &user) const {
        const auto stmt = db->prepare(
            "INSERT INTO Chat_has_User(Chat_id, User_id) VALUES (?, ?)"sv);
        stmt.bind_int(1, chat.id);
        stmt.bind_int(2, user.id);
        stmt.step();
    }

    void remove_user(const models::ChatModel &chat,
                     const models::UserModel &user) const {
        LOG_F(7, "Removing user {} from chat {}", user.id, chat.id);
        const auto stmt = db->prepare(
            "DELETE FROM Chat_has_User WHERE Chat_id = ? AND User_id = ?"sv);
        stmt.bind_int(1, chat.id);
        stmt.bind_int(2, user.id);
        stmt.step();
    }

    using UserChat = std::pair<models::UserModel, models::ChatModel>;

    std::vector<UserChat> all_that_contain_user(string_view name) {
        std::vector<std::pair<models::UserModel, models::ChatModel>> result;
        constexpr auto sql = R"~~(
SELECT U.`id`, U.`name`, U.`user_address`, C.`id`, C.`name`, C.`description`
       FROM User as U
       JOIN Chat_has_User as CU
       JOIN Chat as C
       WHERE U.`name` = ? AND CU.`User_id` = U.`id` AND CU.`Chat_id` = C.`id`
)~~"sv;
        const auto stmt = db->prepare(sql);
        stmt.bind_text(1, name);

        while (stmt.step().is_row()) {

            const auto user = models::UserModel::from_row(stmt);
            models::ChatModel chat;
            chat.id = stmt.column_int(3);
            chat.name = stmt.column_text(4);
            chat.description = stmt.column_text(5);

            result.push_back({user, chat});
        }

        return result;
    }

    std::vector<UserChat> all_that_contain_user_and_chat(string_view username,
                                                         string_view chatname) {
        std::vector<std::pair<models::UserModel, models::ChatModel>> result;
        constexpr auto sql = R"~~(
SELECT U.`id`, U.`name`, U.`user_address`, C.`id`, C.`name`, C.`description`
       FROM User as U
       JOIN Chat_has_User as CU
       JOIN Chat as C
       WHERE U.`name` = ? AND
             C.`name` = ? AND
             CU.`User_id` = U.`id` AND
             CU.`Chat_id` = C.`id`
)~~"sv;
        const auto stmt = db->prepare(sql);
        stmt.bind_text(1, username);
        stmt.bind_text(2, chatname);

        while (stmt.step().is_row()) {

            const auto user = models::UserModel::from_row(stmt);
            models::ChatModel chat;
            chat.id = stmt.column_int(3);
            chat.name = stmt.column_text(4);
            chat.description = stmt.column_text(5);

            result.push_back({user, chat});
        }

        return result;
    }
};
} // namespace uppr::dao
