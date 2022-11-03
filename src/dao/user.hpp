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
};
} // namespace uppr::dao
