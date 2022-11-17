#pragma once

#include "conn.hpp"
#include "dao.hpp"
#include "models/address.hpp"
#include "models/user.hpp"
#include "stmt.hpp"
#include <optional>
#include <vector>

namespace uppr::dao {

/**
 * DAO (Data Access Object) for the `User` table.
 */
class UserDAO : DAO {
public:
    UserDAO(shared_ptr<db::Connection> db_) : DAO{db_} {}

    std::vector<models::UserModel> all() const {
        return select_with<models::UserModel>("SELECT * FROM User");
    }

    std::vector<models::UserModel> all_for_chat(int chat_id) const {
        LOG_F(9, "Getting all users on chat {}", chat_id);

        constexpr auto sql =
            R"~~(
SELECT U.`id`, U.`name`, U.`user_address` FROM User AS U
        JOIN Chat_has_User AS CU ON CU.User_id = U.id
        WHERE CU.Chat_id = ?
)~~"sv;
        const auto stmt = db->prepare(sql);
        stmt.bind_int(1, chat_id);

        std::vector<models::UserModel> users;
        while (stmt.step().is_row()) {
            users.push_back(models::UserModel::from_row(stmt));
        }

        return users;
    }

    models::UserModel with_id(int id) {
        constexpr auto sql = "SELECT * FROM User WHERE id = ?"sv;
        const auto stmt = db->prepare(sql);
        stmt.bind_int(1, id);
        stmt.step();

        return models::UserModel::from_row(stmt);
    }

    void insert(const models::UserModel &m) const {
        const auto stmt =
            db->prepare("INSERT INTO User(name, user_address) VALUES (?, ?)");
        stmt.bind_text(1, m.name);
        stmt.bind_int(2, m.user_address);
        stmt.step();
    }
};
} // namespace uppr::dao
