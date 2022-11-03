#pragma once

#include "commom.hpp"
#include "db/result.hpp"
#include "sqlite3.h"
#include "tl/expected.hpp"

namespace uppr::db {

// forward declare
class Connection;

/**
 * Store an SQL prepared statement.
 */
class PreparedStmt {
    friend class Connection;

    // private constructor to only be used by friend `Connection`.
    constexpr PreparedStmt(sqlite3_stmt *s) : stmt{s} {}

public:
    /**
     * Close database on destruction.
     */
    ~PreparedStmt();

    // no copy
    PreparedStmt(const PreparedStmt &) = delete;
    // no copy
    PreparedStmt &operator=(const PreparedStmt &) = delete;

    // move
    PreparedStmt(PreparedStmt &&o);
    // move
    PreparedStmt &operator=(PreparedStmt &&o);

public:
    constexpr bool is_null() const noexcept { return stmt == nullptr; }

    /**
     * Call `step` on the statement.
     *
     * The return type is `<Result, Result>` because the `Result` code can
     * represent both an erro and a simple status code, so we diferentiate it
     * here in the hope of making the external API bettern.
     */
    Result step() const;

    /**
     * Call `step` on the statement after a reset.
     *
     * The return type is `<Result, Result>` because the `Result` code can
     * represent both an erro and a simple status code, so we diferentiate it
     * here in the hope of making the external API bettern.
     */
    Result execute() const;

    /**
     * Get the number of columns.
     */
    usize column_count() const;

    /**
     * Get name of the requested column.
     */
    string_view column_name(usize idx) const;

    /**
     * Get the value of a column that contains an integer.
     */
    int column_int(usize idx) const;

    /**
     * Get the value of a column that contains text.
     */
    string_view column_text(usize idx) const;

    /**
     * Bind an integer parameter.
     */
    void bind_int(usize idx, int value) const;

    /**
     * Bind a text parameter.
     */
    void bind_text(usize idx, string_view value) const;

private:
    /**
     * Reset the statement so that we can run it again.
     */
    void reset_stmt() const;

private:
    /**
     * The SQLite statement object.
     */
    sqlite3_stmt *stmt;
};
} // namespace uppr::db
