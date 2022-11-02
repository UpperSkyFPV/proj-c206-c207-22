#pragma once

#include "commom.hpp"
#include "db/stmt.hpp"
#include "result.hpp"
#include "sqlite3.h"
#include "tl/expected.hpp"
#include "tl/function_ref.hpp"
#include <memory>

namespace uppr::db {

/**
 * A connection to the SQLite database.
 */
class Connection {
    /**
     * Create an SQLite database.
     */
    constexpr Connection(sqlite3 *db_) : db{db_} {}

public:
    /**
     * Open a connection.
     *
     * @param filename The name of the database file, defaults to in-memory.
     */
    static Connection open(const char *filename = ":memory:");

    /**
     * Open a connection.
     *
     * @param filename The name of the database file, defaults to in-memory.
     */
    static std::shared_ptr<Connection>
    open_ptr(const char *filename = ":memory:");

    /**
     * Close database on destruction.
     */
    ~Connection();

    // no copy
    Connection(const Connection &) = delete;
    // no copy
    Connection &operator=(const Connection &) = delete;

    // move
    Connection(Connection &&o);
    // move
    Connection &operator=(Connection &&o);

public:
    /**
     * Prepare one SQL statement.
     */
    PreparedStmt prepare(string_view sql) const;

    /**
     * Prepare a potentially multiple SQL statement, returning the statement and
     * the rest of the string that was not executed right now.
     */
    std::pair<PreparedStmt, string_view> prepare_many(string_view sql) const;

    /**
     * Prepare and execute one SQL statement.
     */
    std::pair<PreparedStmt, Result> execute_one(string_view sql) const;

    Result
    execute_many(string_view sql,
                 tl::function_ref<void(const PreparedStmt &)> callback) const;

public:
    /**
     * Get the underlying `sqlite3` object.
     */
    sqlite3 *get_sqlite3() const { return db; }

    /**
     * Get the library version string.
     */
    const char *versionstr() const { return sqlite3_libversion(); }

    /**
     * Get an error code assossiated with the `sqlite3` object.
     */
    Result errcode() const { return sqlite3_errcode(db); }

private:
    /**
     * The connection to the database.
     */
    sqlite3 *db{nullptr};
};
} // namespace uppr::db
