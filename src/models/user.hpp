#pragma once

#include "commom.hpp"
#include "stmt.hpp"

namespace uppr::models {

/**
 * The in-program structure to represent a row in the `User` table.
 */
struct UserModel {
    // INTEGER PRIMARY KEY
    int id;
    // VARCHAR(45)
    std::string name;
    // INT
    int user_address;

    static UserModel from_row(const db::PreparedStmt &stmt) {
        const auto id = stmt.column_int(0);
        const auto name = stmt.column_text(1);
        const auto user_address = stmt.column_int(2);

        return {id, std::string{name}, user_address};
    }
};
} // namespace uppr::models
