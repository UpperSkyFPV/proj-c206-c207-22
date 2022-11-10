#pragma once

#include "conn.hpp"
#include "dao.hpp"
#include "models/chat.hpp"
#include "models/user.hpp"
#include "stmt.hpp"
#include <vector>

namespace uppr::dao {

/**
 * DAO (Data Access Object) for the `Chat` table.
 */
class ChatDAO : DAO {
public:
    ChatDAO(shared_ptr<db::Connection> db_) : DAO{db_} {}

    std::vector<models::ChatModel> all() const {
        return select_with<models::ChatModel>("SELECT * FROM Chat");
    }

    void insert(const models::ChatModel &m) const {
        const auto stmt =
            db->prepare("INSERT INTO Chat(name, description) VALUES (?, ?)");
        stmt.bind_text(1, m.name);
        stmt.bind_text(2, m.description);
        stmt.step();
    }

    void add_user(const models::ChatModel &chat,
                  const models::UserModel &user) const {
        const auto stmt = db->prepare(
            "INSERT INTO Chat_has_User(Chat_id, User_id) VALUES (?, ?)");
        stmt.bind_int(1, chat.id);
        stmt.bind_int(2, user.id);
        stmt.step();
    }
};
} // namespace uppr::dao
