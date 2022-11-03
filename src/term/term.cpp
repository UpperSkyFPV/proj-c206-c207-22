#include "loguru.hpp"
#include "term.hpp"
#include "term/termios.hpp"

#include <chrono>
#include <unistd.h>

namespace uppr::term {

Term::Term(int input_file_desc, FILE *output_file)
    : in{input_file_desc}, out{output_file},
      old_termios(get_termios(in)), current_termios{old_termios} {
    // Get the size of the terminal and save it
    update_size();

    uncook_termios();
}

Term::~Term() { cook_termios(); }

void Term::restore_termios() const { set_termios(in, old_termios); }

void Term::save_cursor() const { fwrite(out, "\x1B[s"sv, true); }

void Term::restore_cursor() const { fwrite(out, "\x1B[u"sv, true); }

void Term::hide_cursor() const { fwrite(out, "\x1B[?25l"sv, true); }

void Term::show_cursor() const { fwrite(out, "\x1B[?25h"sv, true); }

void Term::clear_term() const { fwrite(out, "\x1B[2J"sv, true); }

void Term::move_cursor(usize x, usize y) const {
    // x and y are inverted in the command
    fprintf(out, "\x1B[%lu;%luH", y + 1, x + 1);
    fflush(out);
}

void Term::set_termios_control(cc_t time, cc_t min) {
    LOG_F(WARNING, "set_termios_control {}, {}", time, min);
    in_time = time;
    in_min = min;

    current_termios.c_cc[VTIME] = in_time;
    current_termios.c_cc[VMIN] = in_min;
}

void Term::commit_termios(bool flush) const {
    set_termios(in, current_termios, flush ? TCSAFLUSH : TCSANOW);
}

void Term::enable_alternative() const { fwrite(out, "\x1B[?1049h"sv, true); }

void Term::disable_alternative() const { fwrite(out, "\x1B[?1049l"sv, true); }

void Term::save_screen() const { fwrite(out, "\x1B[?47h"sv, true); }

void Term::restore_screen() const { fwrite(out, "\x1B[?47l"sv, true); }

void Term::cook_termios() {
    restore_termios();

    disable_alternative();
    restore_screen();
    restore_cursor();
    show_cursor();
}

void Term::uncook_termios() {
    uncook_current_termios_s();
    set_termios_control(in_time, in_min);

    commit_termios();

    // Push the alternate, so that we can do whatever we want.
    hide_cursor();
    save_cursor();
    save_screen();
    enable_alternative();
}

void Term::cook_termios_with_preserved_alt() {
    LOG_SCOPE_FUNCTION(9);
    restore_termios();

    restore_cursor();
    show_cursor();
}

void Term::uncook_termios_and_preserve_alt() {
    LOG_SCOPE_FUNCTION(9);
    uncook_current_termios_s();
    set_termios_control(in_time, in_min);

    commit_termios();

    // Push the alternate, so that we can do whatever we want.
    hide_cursor();
    save_cursor();
}

void Term::uncook_current_termios_s() {
    //   ECHO: Stop the terminal from displaying pressed keys.
    // ICANON: Disable canonical ("cooked") input mode. Allows us to read
    // inputs
    //         byte-wise instead of line-wise.
    //   ISIG: Disable signals for Ctrl-C (SIGINT) and Ctrl-Z (SIGTSTP), so
    //   we
    //         can handle them as "normal" escape sequences.
    // IEXTEN: Disable input preprocessing. This allows us to handle Ctrl-V,
    //         which would otherwise be intercepted by some terminals.
    current_termios.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN);

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
    current_termios.c_iflag &= ~(IXON | ICRNL | BRKINT | INPCK | ISTRIP);

    // Disable output processing. Common output processing includes prefixing
    // newline with a carriage return.
    current_termios.c_oflag &= ~(OPOST);

    // Set the character size to 8 bits per byte. Likely has no efffect on
    // anything remotely modern.
    current_termios.c_cflag |= CS8;
}

void Term::fwrite(FILE *f, string_view text, bool flush) const {
    ::fwrite(text.data(), sizeof(string_view::value_type), text.size(), out);
    if (flush) fflush(f);
}

char Term::readc() const {
    char c;
    ::read(in, &c, 1);

    return c;
}

string_view Term::read(span<char> buf) const {
    const auto n = ::read(in, buf.data(), buf.size());
    if (n <= 0) return {};

    return {buf.data(), static_cast<usize>(n)};
}

void Term::update_size() {
    const auto [w, h] = get_term_size(fileno(out));

    width = w;
    height = h;
}
} // namespace uppr::term
