#include "term.hpp"

#include "loguru.hpp"
#include <cstdio>
#include <termios.h>

namespace uppr {

Term::Term(int fileno_) : fileno{fileno_} { init(); }

Term::~Term() { deinit_term(); }

char Term::getc() const {
    char c;
    read({&c, 1});
    return c;
}

std::span<char> Term::read(span<char> buf) const {
    const auto count = ::read(fileno, buf.data(), buf.size());
    if (count <= 0) return {};

    return buf.subspan(0, count);
}

void Term::write(string_view str) const {
    ::write(fileno, str.data(), str.size());
}

void Term::move_cursor(int x, int y) const {
    dprintf(fileno, "\x1B[%d;%dH", x, y);
}

void Term::reinit_timeouts(u8 time, u8 min, bool use_now) const {
    auto term = get_term();
    init_timeouts(&term, time, min);
    set_term(&term, use_now);
}

void Term::init() {
    LOG_SCOPE_F(INFO, "initializing terminal");

    // Get the termios state so we can reset it later
    og_term = get_term();
    auto raw = og_term;

    // Enter raw mode
    init_term_raw(&raw);
    init_term_alt();

    // commit our changes
    set_term(&raw);
}

termios Term::get_term() const {
    termios t;
    tcgetattr(fileno, &t);

    return t;
}

void Term::set_term(const termios *const t, bool use_now) const {
    tcsetattr(fileno, use_now ? TCSANOW : TCSAFLUSH, t);
}

void Term::init_term_raw(termios *raw) const {
    //   ECHO: Stop the terminal from displaying pressed keys.
    // ICANON: Disable canonical ("cooked") input mode. Allows us to read inputs
    //         byte-wise instead of line-wise.
    //   ISIG: Disable signals for Ctrl-C (SIGINT) and Ctrl-Z (SIGTSTP), so we
    //         can handle them as "normal" escape sequences.
    // IEXTEN: Disable input preprocessing. This allows us to handle Ctrl-V,
    //         which would otherwise be intercepted by some terminals.
    raw->c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN);

    //   IXON: Disable software control flow. This allows us to handle Ctrl-S
    //         and Ctrl-Q.
    //  ICRNL: Disable converting carriage returns to newlines. Allows us to
    //         handle Ctrl-J and Ctrl-M.
    // BRKINT: Disable converting sending SIGINT on break conditions. Likely has
    //         no effect on anything remotely modern.
    //  INPCK: Disable parity checking. Likely has no effect on anything
    //         remotely modern.
    // ISTRIP: Disable stripping the 8th bit of characters. Likely has no effect
    //         on anything remotely modern.
    raw->c_iflag &= ~(ICRNL | BRKINT | INPCK | ISTRIP);

    // Disable output processing. Common output processing includes prefixing
    // newline with a carriage return.
    raw->c_oflag = OPOST;

    // Set the character size to 8 bits per byte. Likely has no efffect on
    // anything remotely modern.
    raw->c_cflag |= CS8;

    // timeout of read and minimum characters in buffer
    init_timeouts(raw, 0, 1);
}

void Term::init_timeouts(termios *raw, u8 time, u8 min) const {
    raw->c_cc[VTIME] = time;
    raw->c_cc[VMIN] = min;
}

void Term::init_term_alt() const {
    // hide the cursor
    write("\x1B[?25l");
    // Save cursor position.
    write("\x1B[s");
    // Save screen.
    write("\x1B[?47h");
    // Enable alternative buffer.
    write("\x1B[?1049h");

    flush();
}

void Term::deinit_term() const {
    set_term(&og_term);

    // Disable alternative buffer
    write("\x1B[?1049l");
    // Restore screen.
    write("\x1B[?47l");
    // Restore cursor position.
    write("\x1B[u");
    // Show the cursor
    write("\x1B[?25h");
}
} // namespace uppr
