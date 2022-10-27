#include "conn.hpp"

#include "db/result.hpp"
#include "db/stmt.hpp"
#include "loguru.hpp"
#include "tl/expected.hpp"
#include <cctype>

namespace uppr::db {

Connection Connection::open(const char *filename) {
    sqlite3 *db;

    // Open the database and check for error
    Result r = sqlite3_open(filename, &db);
    if (!r.is_ok()) throw DatabaseError{"Error opening database", r};

    return Connection{db};
}

Connection::~Connection() {
    if (db) {
        LOG_F(9, "destructor Connection@{}", fmt::ptr(db));
        sqlite3_close(db);
    }
}

Connection::Connection(Connection &&o) : db{o.db} { o.db = nullptr; }

Connection &Connection::operator=(Connection &&o) {
    db = o.db;
    o.db = nullptr;

    return *this;
}

PreparedStmt Connection::prepare(string_view sql) const {
    sqlite3_stmt *stmt;
    Result r = sqlite3_prepare_v2(db, sql.data(), sql.size(), &stmt, nullptr);
    if (!r.is_ok())
        throw DatabaseError{"Error preparing statement "s.append(sql), r};

    return PreparedStmt{stmt};
}

std::pair<PreparedStmt, string_view>
Connection::prepare_many(string_view sql) const {
    sqlite3_stmt *stmt;
    const char *tail;

    Result r = sqlite3_prepare_v2(db, sql.data(), sql.size(), &stmt, &tail);
    if (!r.is_ok())
        throw DatabaseError{"Error preparing statement "s.append(sql), r};

    return std::make_pair(PreparedStmt{stmt}, sql.substr(tail - sql.data()));
}

std::pair<PreparedStmt, Result> Connection::execute_one(string_view sql) const {
    auto stmt = prepare(sql);
    const auto r = stmt.execute();

    return std::make_pair(std::move(stmt), r);
}

Result Connection::execute_many(
    string_view sql,
    tl::function_ref<void(const PreparedStmt &)> callback) const {
    LOG_SCOPE_FUNCTION(INFO);

    Result rc;
    while (rc.is_ok() && sql.size() > 0) {
        // Prepare the statement
        auto many_ret = prepare_many(sql);
        auto stmt = std::move(many_ret.first);
        sql = many_ret.second;

        // this happens for comments or white-space
        if (stmt.is_null()) continue;

        while (true) {
            rc = stmt.step();

            if (rc.is_row()) {
                callback(stmt);
            } else {
                // Is this very slow?
                while (sql.size() && std::isspace(sql[0]))
                    sql = sql.substr(1);

                rc = {}; // will make it ok again
                break;
            }
        }
    }

    return rc;
}
} // namespace uppr::db
