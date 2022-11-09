#pragma once

#include "conn.hpp"
#include "dao.hpp"
#include "models/chat.hpp"
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
};
} // namespace uppr::dao
