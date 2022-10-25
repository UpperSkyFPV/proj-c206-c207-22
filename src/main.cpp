#include "db.hpp"
#include "loguru.hpp"
#include "sqlite3.h"
#include <unistd.h>

int main(int argc, char **argv) {
    // Handle logging to a file (as a terminal application cant printf to log)
    // loguru::g_stderr_verbosity = loguru::Verbosity_OFF;
    loguru::add_file("./output.log", loguru::Truncate, loguru::Verbosity_9);
    loguru::init(argc, argv);

    uppr::db::Database db{":memory:"};
    if (db.get_lasterror() != nullptr) {
        LOG_F(ERROR, "Could not open database: %s", db.get_lasterror());
        return 1;
    }

    LOG_F(INFO, "SQLite version: %s", db.versionstr());

    const auto res = db.prepare_new("SELECT SQLITE_VERSION()");
    if (db.get_lasterror() != nullptr) {
        LOG_F(ERROR, "Failed to fetch data: %s", db.get_lasterror());
        return 1;
    }

    if (res.raw_step() == SQLITE_ROW) {
        LOG_F(INFO, "-> %s", res.column_text(0));
    }

    return 0;
}
