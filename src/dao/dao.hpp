#pragma once

#include "conn.hpp"
#include <string_view>

namespace uppr::dao {

/**
 * Utility base class for _Data Access Objects_.
 */
class DAO {
public:
    DAO(shared_ptr<db::Connection> db_) : db{db_} {}

protected:
    template <typename T>
    std::vector<T> select_with(string_view sql) const {
        std::vector<T> chats;
        db->execute_many(sql, [&](const db::PreparedStmt &stmt) {
            chats.push_back(T::from_row(stmt));
        });

        return chats;
    }

protected:
    /**
     * Database connection.
     */
    shared_ptr<db::Connection> db;
};
} // namespace uppr::dao
