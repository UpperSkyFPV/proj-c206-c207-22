#include "db.hpp"

namespace uppr::db {

Database::Database(const char *const filename) {
    if (sqlite3_open(filename, &db) != SQLITE_OK)
        last_error = raw_geterror();
    else
        last_error = nullptr;
}

Database::~Database() {
    if (db) sqlite3_close(db);
}

Database::Database(Database &&o) : db{o.db} { o.db = nullptr; }

Database &Database::operator=(Database &&o) {
    db = o.db;
    o.db = nullptr;

    return *this;
}

Statement Database::prepare_new(string_view zsql) {
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, zsql.data(), zsql.size(), &stmt, 0) != SQLITE_OK)
        last_error = raw_geterror();
    else
        last_error = nullptr;

    return {stmt};
}

const char *Database::raw_geterror() const { return sqlite3_errmsg(db); }

Statement::~Statement() {
    if (stmt != nullptr) sqlite3_finalize(stmt);
}

Statement::Statement(Statement &&o) : stmt{o.stmt} { o.stmt = nullptr; }

Statement &Statement::operator=(Statement &&o) {
    stmt = o.stmt;
    o.stmt = nullptr;

    return *this;
}

int Statement::raw_step() const { return sqlite3_step(stmt); }

const uchar *Statement::column_text(usize idx) const {
    return sqlite3_column_text(stmt, idx);
}
} // namespace uppr::db
