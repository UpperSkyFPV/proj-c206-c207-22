#pragma once

#include "fmt/color.h"
#include "fmt/core.h"
#include "pixel.hpp"
#include "term.hpp"
#include <termios.h>

namespace uppr::term {

/**
 * Add a buffer between the terminal driver and user code.
 */
class TermScreen {
public:
    TermScreen(int input_file_descr, FILE *output_file)
        : term{input_file_descr, output_file} {
        update_buffer_size();

        term.clear_term();
    }

    void commit();

public:
    constexpr auto get_size() const noexcept { return term.get_size(); }

    /**
     * Read a character from the terminal.
     */
    auto readc() const { return term.readc(); }

    /**
     * Set a pixel in the buffer.
     */
    void setc(usize x, usize y, const Pixel &pixel);

    /**
     * Set a sequence of pixels in a single line to the result of the formatted
     * string.
     *
     * **WARNING**: Styling should not be used in the arguments here!
     */
    template <typename... Args>
    void print(usize x, usize y, fmt::text_style style,
               fmt::format_string<Args...> fmt, Args &&...args) {
        vprint(x, y, style, fmt, fmt::make_format_args(args...));
    }

    /**
     * Set a sequence of pixels in a single line to the result of the formatted
     * string with a single style.
     *
     * **WARNING**: Styling should not be used in the arguments here!
     */
    template <typename... Args>
    void print(usize x, usize y, fmt::format_string<Args...> fmt,
               Args &&...args) {
        vprint(x, y, fmt, fmt::make_format_args(args...));
    }

    /**
     * Draw a horizontal line.
     */
    void hline(usize sx, usize ex, usize y, const Pixel &fill);

    void vprint(usize x, usize y, fmt::string_view fmt, fmt::format_args args);
    void vprint(usize x, usize y, fmt::text_style style, fmt::string_view fmt,
                fmt::format_args args);

public:
    /**
     * Set the delay and minimum threshold for input.
     */
    void set_termios_control(cc_t time, cc_t min) {
        term.set_termios_control(time, min);
    }

    /**
     * Update the size of terminal.
     *
     * This should be called from the `SIGWINCH` signal to update based on
     * real-time changes.
     */
    void update_size();

    /**
     * Clear the screen.
     */
    void clear();

private:
    /**
     * Update the size of the pixel buffer to match the size of the terminal.
     */
    void update_buffer_size();

    /**
     * Convert x,y coordinates to an index that we can use to index the pixel
     * buffer.
     */
    constexpr usize ctoidx(usize x, usize y) const noexcept {
        const auto [w, _] = term.get_size();

        return y * w + x;
    }

    constexpr bool valid_coords(usize x, usize y) const noexcept {
        const auto [w, h] = term.get_size();

        return x <= w && y <= h;
    }

    constexpr bool valid_coords(usize w, usize h, usize x,
                                usize y) const noexcept {
        return x <= w && y <= h;
    }

    /**
     * If we need to actually re-print everything to the terminal.
     */
    constexpr auto needs_repaint() const noexcept { return dirty; }

    /**
     * Reset the flag to say that we just painted.
     */
    constexpr void mark_clean() { dirty = false; }

    /**
     * Set the flag to say that we need to repaint.
     */
    constexpr void mark_dirty() { dirty = true; }

private:
    /**
     * The intermediate buffer where we store the characters before committing
     * to the terminal.
     */
    std::vector<Pixel> buffer;

    /**
     * An optimization to not re-draw frames that are exactly the same.
     */
    bool dirty{true};

    /**
     * The actual terminal driver.
     */
    Term term;
};
} // namespace uppr::term
