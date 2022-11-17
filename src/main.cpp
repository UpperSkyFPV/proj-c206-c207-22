#include "app/perf-scene.hpp"
#include "chat-scene.hpp"
#include "eng/box-scene.hpp"
#include "eng/engine.hpp"
#include "eng/stack-scene.hpp"
#include "except.hpp"
#include "fmt/color.h"
#include "loguru.hpp"
#include "result.hpp"
#include "root.hpp"
#include "screen.hpp"
#include "sidebar-scene.hpp"
#include "sqlite3.h"

#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>
#include <unistd.h>

#include "db/conn.hpp"
#include "os/file.hpp"
#include "state.hpp"
#include "term/key.hpp"
#include "term/term.hpp"
#include "vector2.hpp"

std::shared_ptr<uppr::term::TermScreen> term;

void handle_winch(int sig);

int main(int argc, char **argv) {
    const auto env_port = std::getenv("PORT");
    const auto env_name = std::getenv("NAME");
    const auto actual_port = env_port ? std::stoi(env_port) : 8080;
    const auto actual_name =
        env_name ? std::string{env_name} : std::string{"user 0"};

    const auto log_filename = fmt::format("output{}.log", actual_port);

    // Handle logging to a file (as a terminal application cant printf to log)
    loguru::g_stderr_verbosity = loguru::Verbosity_OFF;
    loguru::add_file(log_filename.c_str(), loguru::Truncate,
                     loguru::Verbosity_8);
    loguru::init(argc, argv);

    term = std::make_shared<uppr::term::TermScreen>(fileno(stdin), stdout);
    signal(SIGWINCH, handle_winch);

    try {
        uppr::app::start_app(term, actual_port, actual_name);
    } catch (const uppr::db::DatabaseError &e) {
        LOG_F(ERROR, "Database error: [{}, {}]", e.what(),
              e.get_result().str());
    }

    return 0;
}

void handle_winch(int sig) {
    signal(SIGWINCH, SIG_IGN);

    // Reinitialize the window to update data structures.
    term->update_size();

    signal(SIGWINCH, handle_winch);
}
