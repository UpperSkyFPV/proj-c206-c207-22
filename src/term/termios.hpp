#pragma once

#include <sys/ioctl.h>
#include <termios.h>

#include "commom.hpp"
#include "meta/string.hpp"

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

inline std::pair<ushort, ushort> get_term_size(int fd) {
    winsize w;
    ioctl(fd, TIOCGWINSZ, &w);

    return {w.ws_col, w.ws_row};
}

static constexpr auto ESC = "\x1B["sv;

} // namespace uppr::term
