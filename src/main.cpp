#include "app/test-scene.hpp"
#include "eng/engine.hpp"
#include "except.hpp"
#include "fmt/color.h"
#include "loguru.hpp"
#include "screen.hpp"
#include "sqlite3.h"

#include <csignal>
#include <cstdio>
#include <iostream>
#include <memory>
#include <unistd.h>

#include "db/conn.hpp"
#include "os/file.hpp"
#include "term/key.hpp"
#include "term/term.hpp"

std::shared_ptr<uppr::term::TermScreen> term;

void handle_winch(int sig);

int main(int argc, char **argv) {
    // Handle logging to a file (as a terminal application cant printf to log)
    loguru::g_stderr_verbosity = loguru::Verbosity_OFF;
    loguru::add_file("./output.log", loguru::Truncate, loguru::Verbosity_9);
    loguru::init(argc, argv);

    term = std::make_shared<uppr::term::TermScreen>(fileno(stdin), stdout);
    term->set_termios_control(1, 0);
    signal(SIGWINCH, handle_winch);

    const auto initial_scene = std::make_shared<uppr::app::TestScene>();
    uppr::eng::Engine engine{30, term, initial_scene};

    engine.run();

    return 0;
#if 0
    term->clear_term();

    const auto [w, h] = term->get_size();
    LOG_F(INFO, "w={}, h={}", w, h);

    while (true) {
        const auto c = term->readc();
        if (uppr::term::is_ctrl(c, 'q')) break;

        if (uppr::term::is_escseq(c)) {
            term->set_termios_control(1, 0);
            term->commit_termios(false);

            std::array<char, 8> buf;
            const auto sequence = term->read(buf);

            term->set_termios_control(0, 1);
            term->commit_termios(false);

            if (uppr::term::is_esc(sequence)) {
                LOG_F(INFO, "ESC");
            } else if (uppr::term::is_left(sequence)) {
                LOG_F(INFO, "left");
            } else if (uppr::term::is_right(sequence)) {
                LOG_F(INFO, "right");
            } else if (uppr::term::is_up(sequence)) {
                LOG_F(INFO, "up");
            } else if (uppr::term::is_down(sequence)) {
                LOG_F(INFO, "down");
            }
        }

        term->move_cursor(w / 2, h / 2);
        fmt::print("char '{0:d}' ({0})",
                   fmt::styled(c, fmt::fg(fmt::color::red)));
        fflush(stdout);
    }

    return 0;
#endif
}

void handle_winch(int sig) {
    signal(SIGWINCH, SIG_IGN);

    // Reinitialize the window to update data structures.
    term->update_size();

    signal(SIGWINCH, handle_winch);
}
