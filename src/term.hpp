#pragma once

#include <termios.h>
#include <unistd.h>

#include "commom.hpp"
#include "io.hpp"

namespace uppr {

/**
 * Wrap the `termios.h` C interface.
 *
 * Enters uncooked terminal mode on construction and restores terminal on
 * destruction.
 *
 * Very useful resources:
 * - [terminal
 * codes](https://gist.github.com/fnky/458719343aabd01cfb17a3a4f7296797)
 * - [termios reference](https://man7.org/linux/man-pages/man3/termios.3.html)
 * - [aio
 * tutorial](https://zig.news/lhp/want-to-create-a-tui-application-the-basics-of-uncooked-terminal-io-17gm)
 */
class Term {
public:
    Term(int fileno_);
    ~Term();

public:
    /**
     * Get a single character from the file.
     */
    char getc() const;

    /**
     * Read characters into the buffer, returns the resulting buffer.
     *
     * Wraps the `read(3)` `` function.
     */
    span<char> read(span<char> buf) const;

    /**
     * Write characters to the screen.
     */
    void write(string_view str) const;

public:
    void move_cursor(int x, int y) const;

public:
    /**
     * Reset termios timeouts to the given values.
     */
    void reinit_timeouts(u8 time, u8 min, bool use_now = false) const;

private:
    /**
     * Initialize everything.
     */
    void init();

    /**
     * Get the termios config.
     */
    termios get_term() const;

    /**
     * Set the termios config.
     */
    void set_term(const termios *const t, bool use_now = false) const;

    /**
     * Init RAW mode for the given termios configuration.
     */
    void init_term_raw(termios *raw) const;

    /**
     * Configures termious timeouts.
     */
    void init_timeouts(termios *raw, u8 time, u8 min) const;

    /**
     * Init alternate screen and hide cursor.
     */
    void init_term_alt() const;

    /**
     * Reset termios config back to before constructing the `Term` object and
     * restores screen.
     */
    void deinit_term() const;

private:
    /**
     * Wrapper for `std::fflush`.
     */
    void flush() const { /* fflush(file); */
    }

private:
    // Where to write the characters to
    int fileno;

    // The original state of termios
    termios og_term;
};
} // namespace uppr
