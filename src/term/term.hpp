#pragma once

#include "commom.hpp"
#include "fmt/color.h"
#include "fmt/core.h"
#include "meta/string.hpp"
#include "term/key.hpp"
#include "termios.hpp"
#include <cstdio>
#include <unistd.h>
#include <vector>

namespace uppr::term {

/**
 * A terminal graphics control class.
 */
class Term {
public:
    /**
     * Create the `Term` class from input and output files.
     *
     * This constructor will also get the current termios state from the
     * terminal. On destruction, the terminal will be returned back to this
     * state.
     */
    Term(int input_file_descr, FILE *output_file);
    ~Term();

public:
    // termios control functions

    /**
     * Set the terminal configuration to the internally stored configuration (we
     * latched it at construction time).
     */
    void restore_termios() const;

    /**
     * Hide the cursor.
     */
    void hide_cursor() const;

    /**
     * Show the cursor.
     */
    void show_cursor() const;

    /**
     * Clear the terminal.
     */
    void clear_term() const;

    /**
     * Move the cursor to the given X, Y coordinates in the screen.
     *
     * NOTE: The normal coordinates in terminals are **1** based, but this
     * function corrects it to **0** based.
     */
    void move_cursor(usize x, usize y) const;

    /**
     * Move the cursor to the home position.
     */
    void home_cursor() const { fwrite(out, "\x1B[H"sv); }

    /**
     * Set the terminal timeout and minimum bytes before completion.
     */
    void set_termios_control(cc_t time, cc_t min);

    /**
     * Commit the current input termios configuration.
     */
    void commit_termios(bool flush = true) const;

private:
    // private termios control functions

    /**
     * Push the alternate screen.
     */
    void enable_alternative() const;

    /**
     * Pop the alternate screen.
     */
    void disable_alternative() const;

    /**
     * Save the current cursor position.
     */
    void save_cursor() const;

    /**
     * Restore the last saved cursor position.
     */
    void restore_cursor() const;

    /**
     * Save the current screen contents.
     */
    void save_screen() const;

    /**
     * Restore the last saved screen contents.
     */
    void restore_screen() const;

    /**
     * Do the full sequence of configurations to 'uncook' the terminal.
     */
    void uncook_termios();

    /**
     * Setup the `current_termios` struct to be 'uncooked'.
     */
    void uncook_current_termios_s();

public:
    // output functions

    /**
     * wrapper for fwrite function.
     */
    void fwrite(FILE *f, string_view text, bool flush = false) const;

    template <typename... Args>
    void print(fmt::format_string<Args...> fmt, Args &&...args) const {
        vprint(fmt, fmt::make_format_args(args...));
    }

    template <typename... Args>
    void print(const fmt::text_style& ts, fmt::format_string<Args...> fmt, Args &&...args) const {
        vprint(ts, fmt, fmt::make_format_args(args...));
    }

    template <typename... Args>
    void print(usize x, usize y, fmt::format_string<Args...> fmt, Args &&...args) const {
        move_cursor(x, y);
        vprint(fmt, fmt::make_format_args(args...));
    }

    template <typename... Args>
    void print(usize x, usize y, const fmt::text_style& ts, fmt::format_string<Args...> fmt, Args &&...args) const {
        move_cursor(x, y);
        vprint(ts, fmt, fmt::make_format_args(args...));
    }

    void vprint(fmt::string_view fmt, fmt::format_args args) const {
        fmt::vprint(out, fmt, args);
        fflush(out);
    }

    void vprint(const fmt::text_style& ts, fmt::string_view fmt, fmt::format_args args) const {
        fmt::vprint(out, ts, fmt, args);
        fflush(out);
    }

public:
    // input functions

    /**
     * Wraper for `read` function that gets a single character.
     */
    char readc() const;

    /**
     * Wrapper for `read` that atempts to fill the given buffer, and returns a
     * new span with what was actually read.
     */
    string_view read(span<char> buf) const;

public:
    // Term class control functions

    /**
     * Update width and height of the terminal.
     */
    void update_size();

    /**
     * Get the width and height of the terminal.
     */
    constexpr std::pair<ushort, ushort> get_size() const {
        return {width, height};
    }

private:
    /**
     * Width of the terminal.
     */
    uint width{};

    /**
     * Height of the terminal.
     */
    uint height{};

private:
    /**
     * Set the input delay/timeout.
     */
    u16 in_time{};

    /**
     * Set the minimum buffered ammount of characters before read.
     */
    u16 in_min{};

private:
    /**
     * The file that we use for input.
     *
     * NOTE: this is not a `FILE` because we want **raw** and **unfiltered**
     * access to the characters. The C file API gets in the way too often.
     */
    int in;

    /**
     * The file that we use for output.
     */
    FILE *out;

    /**
     * Store what was termios like before we messed with it.
     */
    termios old_termios;

    /**
     * Store the current termios, so we dont need to do a full API call to
     * `get_termios` every time we hit a special ambiguous character.
     */
    termios current_termios;
};
} // namespace uppr::term
