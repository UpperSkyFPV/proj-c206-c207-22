#include "except.hpp"
#include "loguru.hpp"
#include "sqlite3.h"

#include <iostream>
#include <unistd.h>

#include "db/conn.hpp"
#include "file.hpp"

using namespace uppr;

int main(int argc, char **argv) {
    // Handle logging to a file (as a terminal application cant printf to log)
    // loguru::g_stderr_verbosity = loguru::Verbosity_OFF;
    loguru::add_file("./output.log", loguru::Truncate, loguru::Verbosity_9);
    loguru::init(argc, argv);

    return 0;
}
