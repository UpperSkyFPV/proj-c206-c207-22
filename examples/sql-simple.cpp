#include "except.hpp"
#include "loguru.hpp"
#include "sqlite3.h"

#include <iostream>
#include <unistd.h>

#include "db/conn.hpp"
#include "os/file.hpp"

using namespace uppr;

int main(int argc, char **argv) {
    loguru::init(argc, argv);

    const auto db = db::Connection::open("db");
    LOG_F(INFO, "SQLite version: %s", db.versionstr());

    const auto tables_sql = file::read_file_text("res/tables.sql");

    except::wrap_fatal_exception([&] {
        db.execute_many(tables_sql, [](const auto &stmt) {
            LOG_F(INFO, "many callback PreparedStmt@%p", &stmt);
        });
    });

    db.execute_one("INSERT INTO User(name) VALUES ('Test')"sv);

    const auto sel = db.prepare("SELECT * FROM User"sv);
    for (;;) {
        const auto s = sel.step();
        if (!s.is_row()) break;

        auto name = sel.column_name(0);
        const auto id = sel.column_int(0);

        name = sel.column_name(1);
        const auto uname = sel.column_text(1);

        const auto nrows = sel.column_count();
        LOG_F(INFO, "[%lu] %.*s: %d, %.*s: %.*s", nrows,
              static_cast<int>(name.size()), name.data(), id,
              static_cast<int>(name.size()), name.data(),
              static_cast<int>(uname.size()), uname.data());
    }

    return 0;
}
