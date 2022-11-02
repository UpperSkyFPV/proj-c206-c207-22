#pragma once

#include "conn.hpp"
#include "dao.hpp"
#include "models/address.hpp"
#include "stmt.hpp"

namespace uppr::dao {

/**
 * DAO (Data Access Object) for the `Address` table.
 */
class AddressDAO : DAO {
public:
    AddressDAO(shared_ptr<db::Connection> db_) : DAO{db_} {}

    std::vector<models::AddressModel> all() const {
        return select_with<models::AddressModel>("SELECT * FROM Address");
    }
};
} // namespace uppr::dao
