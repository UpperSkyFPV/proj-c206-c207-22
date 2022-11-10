#include "app/perf-scene.hpp"
#include "chat-scene.hpp"
#include "eng/box-scene.hpp"
#include "eng/engine.hpp"
#include "eng/stack-scene.hpp"
#include "except.hpp"
#include "fmt/color.h"
#include "loguru.hpp"
#include "root.hpp"
#include "screen.hpp"
#include "sidebar-scene.hpp"
#include "sqlite3.h"

#include <csignal>
#include <cstdio>
#include <iostream>
#include <memory>
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
    // Handle logging to a file (as a terminal application cant printf to log)
    loguru::g_stderr_verbosity = loguru::Verbosity_OFF;
    loguru::add_file("./output.log", loguru::Truncate, loguru::Verbosity_9);
    loguru::init(argc, argv);

    term = std::make_shared<uppr::term::TermScreen>(fileno(stdin), stdout);
    signal(SIGWINCH, handle_winch);

    uppr::app::start_app(term);

    return 0;
}

void handle_winch(int sig) {
    signal(SIGWINCH, SIG_IGN);

    // Reinitialize the window to update data structures.
    term->update_size();

    signal(SIGWINCH, handle_winch);
}
