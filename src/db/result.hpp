#pragma once

#include "sqlite3.h"
#include <exception>
#include <stdexcept>

namespace uppr::db {

/**
 * Represent an SQLite result, which might be an error or just a status.
 */
class Result {
public:
    constexpr Result() : code{SQLITE_OK} {}
    constexpr Result(int c) : code{c} {}

    /**
     * Get the actual code.
     */
    constexpr int get() const { return code; }

    /**
     * If the code is equal to `SQLITE_OK`.
     */
    constexpr bool is_ok() const { return code == SQLITE_OK; }

    /**
     * If the code is equal to `SQLITE_DONE`.
     */
    constexpr bool is_done() const { return code == SQLITE_DONE; }

    /**
     * If the code is equal to `SQLITE_ROW`.
     */
    constexpr bool is_row() const { return code == SQLITE_ROW; }

    /**
     * Convert the code to a string.
     */
    const char *str() const { return sqlite3_errstr(code); }

private:
    /**
     * The result code.
     */
    int code;
};

class DatabaseError : std::exception {
public:
    DatabaseError(const std::string &msg, Result r) : message{msg}, result{r} {}

    const char *what() const noexcept override { return message.c_str(); }

    Result get_result() const noexcept { return result; }

private:
    std::string message;
    Result result;
};
} // namespace uppr::db
