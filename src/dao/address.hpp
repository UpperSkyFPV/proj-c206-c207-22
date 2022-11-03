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

    models::AddressModel with_id(int id) const {
        const auto stmt = db->prepare("SELECT * FROM Address WHERE id = ?");
        stmt.bind_int(1, id);
        stmt.step();

        return models::AddressModel::from_row(stmt);
    }
};
} // namespace uppr::dao
