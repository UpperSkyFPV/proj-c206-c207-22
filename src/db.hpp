#pragma once

#include "commom.hpp"
#include "sqlite3.h"

namespace uppr::db {

class Database;

class Statement {
    friend class Database;

private:
    Statement(sqlite3_stmt *s) : stmt{s} {}

public:
    /**
     * Close database on destruction.
     */
    ~Statement();

    // no copy
    Statement(const Statement &) = delete;
    // no copy
    Statement &operator=(const Statement &) = delete;

    // move
    Statement(Statement &&o);
    // move
    Statement &operator=(Statement &&o);

public:
    /**
     * Call the `sqlite3_step` function directly.
     */
    int raw_step() const;

    const uchar *column_text(usize idx) const;

private:
    /**
     * The prepared statement.
     */
    sqlite3_stmt *stmt;
};

/**
 * A wrapper class for using the SQLite database.
 */
class Database {
public:
    /**
     * Create an SQLite database.
     */
    Database(const char *const filename);

    /**
     * Close database on destruction.
     */
    ~Database();

    // no copy
    Database(const Database &) = delete;
    // no copy
    Database &operator=(const Database &) = delete;

    // move
    Database(Database &&o);
    // move
    Database &operator=(Database &&o);

public:
    /**
     * Get the raw database connection object.
     */
    sqlite3 *raw_getdb() const { return db; }

    /**
     * Get the version string.
     */
    const char *versionstr() const { return sqlite3_libversion(); }

    /**
     * Get the last error that happened.
     */
    const char *get_lasterror() const { return last_error; }

public:
    /**
     * Prepare an SQL statement.
     */
    Statement prepare_new(string_view zsql);

private:
    const char *raw_geterror() const;

private:
    /**
     * The connection to the database.
     */
    sqlite3 *db{nullptr};

    /**
     * Store the last error that happened.
     */
    const char *last_error;
};

} // namespace uppr::db
