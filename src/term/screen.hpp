#pragma once

#include "box-options.hpp"
#include "fmt/color.h"
#include "fmt/core.h"
#include "pixel.hpp"
#include "term.hpp"
#include "vector2.hpp"

#include <termios.h>

namespace uppr::term {

/**
 * Add a buffer between the terminal driver and user code, so that more
 * interesting things can be done in a way more eficient way then direct
 * printing of control sequences.
 *
 * Terminal printing is the slowest part of the whole process, so by reducing
 * the ammount of things we send to it we gain a lot of speed.
 */
class TermScreen {
public:
    /**
     * Construct the screen. Parameters are given directly to the `Term` class.
     */
    TermScreen(int input_file_descr, FILE *output_file)
        : term{input_file_descr, output_file} {
        update_buffer_size();
        term.clear_term();
    }

    /**
     * Actually commit the buffer to the terminal screen.
     *
     * If the `dirty` flag is not set, then this call has absolutally no effect.
     */
    void commit();

public:
    /**
     * Get the size of the terminal.
     */
    constexpr auto get_size() const noexcept { return term.get_size(); }

    /**
     * Read a single character from the terminal.
     */
    char readc() const { return term.readc(); }

    /**
     * Read a single character from the terminal.
     */
    string_view read(span<char> buf) const { return term.read(buf); }

    /**
     * Set a pixel in the buffer at the given coordinates.
     */
    void setc(int x, int y, const Pixel &pixel);

    /**
     * Set a pixel in the buffer with the given transform.
     */
    void setc(const Transform &t, const Pixel &pixel) {
        setc(t.getx(), t.gety(), pixel);
    }

    /**
     * Set a sequence of pixels in a single line to the result of the formatted
     * string.
     *
     * **WARNING**: Styling should not be used in the arguments here!
     */
    template <typename... Args>
    void print(int x, int y, fmt::text_style style,
               fmt::format_string<Args...> fmt, Args &&...args) {
        vprint(x, y, style, fmt, fmt::make_format_args(args...));
    }

    /**
     * Set a sequence of pixels in a single line to the result of the formatted
     * string.
     *
     * **WARNING**: Styling should not be used in the arguments here!
     */
    template <typename... Args>
    void print(const Transform &t, fmt::text_style style,
               fmt::format_string<Args...> fmt, Args &&...args) {
        vprint(t.getx(), t.gety(), style, fmt, fmt::make_format_args(args...));
    }

    /**
     * Set a sequence of pixels in a single line to the result of the formatted
     * string with a single style.
     *
     * **WARNING**: Styling should not be used in the arguments here!
     */
    template <typename... Args>
    void print(int x, int y, fmt::format_string<Args...> fmt, Args &&...args) {
        vprint(x, y, fmt, fmt::make_format_args(args...));
    }

    /**
     * Set a sequence of pixels in a single line to the result of the formatted
     * string with a single style.
     *
     * **WARNING**: Styling should not be used in the arguments here!
     */
    template <typename... Args>
    void print(const Transform &t, fmt::format_string<Args...> fmt,
               Args &&...args) {
        vprint(t.getx(), t.gety(), fmt, fmt::make_format_args(args...));
    }

    /**
     * Draw a box on the given postion with the given size
     */
    void box(const Transform &tl, usize width, usize height,
             const BoxOptions &opt);

    /**
     * Draw a horizontal line.
     */
    void hline(int sx, int ex, int y, const Pixel &fill);

    /**
     * Draw a vertical line.
     */
    void vline(int x, int sy, int ey, const Pixel &fill);

    void vprint(int x, int y, fmt::string_view fmt, fmt::format_args args);
    void vprint(int x, int y, fmt::text_style style, fmt::string_view fmt,
                fmt::format_args args);

    /**
     * Read a line from the user. **DANGEROUS**!
     *
     * This function completally removes the spacial _uncooked_ handling of the
     * terminal for getting text input. If the user sends a signal like `ctrl+c`
     * while this is active, then the program dies horribly!
     */
    std::string inputline(Transform t, usize max_length);

public:
    /**
     * Set the delay and minimum threshold for input.
     */
    void set_termios_control(cc_t time, cc_t min) {
        term.set_termios_control(time, min);
    }

    void commit_termios(bool flush) { term.commit_termios(flush); }

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

    /**
     * Exit uncooked mode for text input.
     */
    void danger_uncook() { term.danger_uncook(); }

    /**
     * Enter uncooked mode for text input.
     */
    void danger_cook() { term.danger_cook(); }

    /**
     * Exit uncooked mode for text input.
     */
    void danger_uncook_alt() { term.danger_uncook_and_preserve_alt(); }

    /**
     * Enter uncooked mode for text input.
     */
    void danger_cook_alt() { term.danger_cook_with_preserved_alt(); }

    /**
     * Move the cursor directly inside the screen.
     */
    void raw_move_cursor(usize x, usize y) const { term.move_cursor(x, y); }

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

    /**
     * If the given coordinates are valid.
     */
    constexpr bool valid_coords(int x, int y) const noexcept {
        const auto [w, h] = term.get_size();
        return valid_coords(w, h, x, y);
    }

    /**
     * If the given coordinates are valid with the given width and height.
     */
    constexpr bool valid_coords(usize w, usize h, int x, int y) const noexcept {
        return x >= 0 && y >= 0 && x <= w && y <= h;
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
