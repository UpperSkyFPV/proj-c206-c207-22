#pragma once

#include <sys/ioctl.h>
#include <termios.h>

#include "commom.hpp"

namespace uppr::term {

/**
 * Get the current termios config of `fd` (from `tcgetattr`).
 */
inline termios get_termios(int fd) {
    termios t;
    tcgetattr(fd, &t);

    return t;
}

/**
 * Set the current termios config of `fd` (with `tcsetattr`).
 */
inline void set_termios(int fd, const termios &t, int opt = TCSAFLUSH) {
    tcsetattr(fd, opt, &t);
}

/**
 * Get the size of the terminal from the system using `TIOCGWINSZ`.
 */
inline std::pair<ushort, ushort> get_term_size(int fd) {
    winsize w;
    ioctl(fd, TIOCGWINSZ, &w);

    return {w.ws_col, w.ws_row};
}

/**
 * This is the starting escape sequence for most commands.
 */
static constexpr auto ESC = "\x1B["sv;

} // namespace uppr::term
