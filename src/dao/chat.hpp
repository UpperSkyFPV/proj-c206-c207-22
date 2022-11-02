#pragma once

#include "conn.hpp"
#include "dao.hpp"
#include "models/chat.hpp"
#include "stmt.hpp"

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
};
} // namespace uppr::dao
