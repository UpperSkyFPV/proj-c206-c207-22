#include "db/result.hpp"
#include "loguru.hpp"
#include "stmt.hpp"

namespace uppr::db {

PreparedStmt::~PreparedStmt() {
    if (stmt) {
        LOG_F(9, "destructor PreparedStmt@{}", fmt::ptr(stmt));
        sqlite3_finalize(stmt);
    }
}

PreparedStmt::PreparedStmt(PreparedStmt &&o) : stmt{o.stmt} {
    o.stmt = nullptr;
}

PreparedStmt &PreparedStmt::operator=(PreparedStmt &&o) {
    stmt = o.stmt;
    o.stmt = nullptr;

    return *this;
}

void PreparedStmt::reset_stmt() const {
    Result r = sqlite3_reset(stmt);
    if (!r.is_ok()) throw DatabaseError{"Error resetting statement", r};

    r = sqlite3_clear_bindings(stmt);
    if (!r.is_ok())
        throw DatabaseError{"Error clearing bindings from statement", r};
}

Result PreparedStmt::step() const {
    Result r = sqlite3_step(stmt);
    if (!(r.is_ok() || r.is_done() || r.is_row()))
        throw DatabaseError{"Error running step on statement", r};

    return r;
}

Result PreparedStmt::execute() const {
    reset_stmt();
    return step();
}

usize PreparedStmt::column_count() const { return sqlite3_column_count(stmt); }

string_view PreparedStmt::column_name(usize idx) const {
    return sqlite3_column_name(stmt, idx);
}

int PreparedStmt::column_int(usize idx) const {
    return sqlite3_column_int(stmt, idx);
}

string_view PreparedStmt::column_text(usize idx) const {
    const auto ustr = sqlite3_column_text(stmt, idx);

    return reinterpret_cast<const char *>(ustr);
}

void PreparedStmt::bind_int(usize idx, int value) const {
    const auto result = sqlite3_bind_int(stmt, idx, value);

    if (result != SQLITE_OK)
        throw DatabaseError{"Error binding integer value", result};
}
} // namespace uppr::db
