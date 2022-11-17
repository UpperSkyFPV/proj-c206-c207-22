#pragma once

#include "commom.hpp"
#include "stmt.hpp"

namespace uppr::models {

/**
 * The in-program structure to represent a row in the `Message` table.
 */
struct MessageModel {
    // INTEGER PRIMARY KEY
    int id;
    // VARCHAR(255)
    std::string content;
    // TINYINT
    bool sent;
    // TINYINT
    bool received;
    // VARCHAR(45)
    std::string error;
    // INT
    int in_chat;
    // INT
    int sent_by;

    static MessageModel from_row(const db::PreparedStmt &stmt) {
        const auto id = stmt.column_int(0);
        const auto content = stmt.column_text(1);
        const auto sent = stmt.column_int(2);
        const auto received = stmt.column_int(3);
        const auto error = stmt.column_text(4);
        const auto in_chat = stmt.column_int(5);
        const auto sent_by = stmt.column_int(6);

        return {id,
                std::string{content},
                static_cast<bool>(sent),
                static_cast<bool>(received),
                std::string{error},
                in_chat,
                sent_by};
    }
};
} // namespace uppr::models
