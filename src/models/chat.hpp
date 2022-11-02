#pragma once

#include "commom.hpp"
#include "stmt.hpp"

namespace uppr::models {

/**
 * The in-program structure to represent a row in the `Chat` table.
 */
struct ChatModel {
    // INTEGER PRIMARY KEY
    int id;
    // VARCHAR(45)
    std::string name;
    // VARCHAR(45)
    std::string description;

    static ChatModel from_row(const db::PreparedStmt &stmt) {
        const auto id = stmt.column_int(0);
        const auto name = stmt.column_text(1);
        const auto descr = stmt.column_text(2);

        return {id, std::string{name}, std::string{descr}};
    }
};
} // namespace uppr::models
