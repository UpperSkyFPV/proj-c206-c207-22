#pragma once

#include "commom.hpp"
#include "stmt.hpp"

namespace uppr::models {

/**
 * The in-program structure to represent a row in the `Address` table.
 */
struct AddressModel {
    // INTEGER PRIMARY KEY
    int id;
    // VARCHAR(45)
    std::string host;
    // INT
    int port;

    static AddressModel from_row(const db::PreparedStmt &stmt) {
        const auto id = stmt.column_int(0);
        const auto host = stmt.column_text(1);
        const auto port = stmt.column_int(2);

        return {id, std::string{host}, port};
    }
};
} // namespace uppr::models
