#pragma once

#include "conn.hpp"
#include "dao.hpp"
#include "models/address.hpp"
#include "models/user.hpp"
#include "stmt.hpp"
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

    void insert(const models::UserModel &m) const {
        const auto stmt =
            db->prepare("INSERT INTO User(name, user_address) VALUES (?, ?)");
        stmt.bind_text(1, m.name);
        stmt.bind_int(2, m.user_address);
        stmt.step();
    }
};
} // namespace uppr::dao
