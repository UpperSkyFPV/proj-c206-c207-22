#include "commom.hpp"
#include "fmt/color.h"
#include "screen.hpp"
#include <algorithm>
#include <bits/ranges_algo.h>

namespace uppr::term {
void TermScreen::commit() {
    // minor optimization that probably is not needed
    if (!needs_repaint()) return;
    mark_clean();

    // Get the start of the buffer
    auto ch = buffer.begin();
    auto start = ch;

    // Get the style of the first pixel
    fmt::text_style prev_style = ch->get_style();

    // Convenience function to print all pixels until the given one with
    // `prev_style`
    const auto flush_buffered = [&](auto ch) {
        for (; start != ch; start++)
            term.print(prev_style, "{}", start->getc());
    };

    // Move the cursor to the top-left
    term.home_cursor();

    // For each pixel
    const auto [w, h] = term.get_size();
    for (usize j{}; j < h; j++) {
        for (usize i{}; i < w; i++) {
            const auto new_style = ch->get_style();

            if (prev_style != new_style) {
                // Only print when the style changes (avoid re printing color
                // control sequences many times)
                flush_buffered(ch);
                prev_style = new_style;
            }

            // move to the next pixel
            ch++;
        }
    }

    flush_buffered(buffer.end());
    term.flush();
}

void TermScreen::setc(usize x, usize y, const Pixel &pixel) {
    const auto [w, h] = term.get_size();

    // Abort on invalid size
    if (!valid_coords(w, h, x, y)) return;

    buffer[ctoidx(x, y)] = pixel;
    mark_dirty();
}

void TermScreen::hline(usize sx, usize ex, usize y, const Pixel &fill) {
    const auto [w, h] = term.get_size();

    // Abort on invalid coords and truncate lines that are to long
    if (!valid_coords(w, h, sx, y)) return;
    ex = std::min(ex, static_cast<usize>(w));

    for (; sx != ex; sx++) {
        buffer[ctoidx(sx, y)] = fill;
    }
}

void TermScreen::vline(usize x, usize sy, usize ey, const Pixel &fill) {
    const auto [w, h] = term.get_size();

    // Abort on invalid coords and truncate lines that are to long
    if (!valid_coords(w, h, sy, ey)) return;
    ey = std::min(ey, static_cast<usize>(w));

    for (; sy != ey; sy++) {
        buffer[ctoidx(x, sy)] = fill;
    }
}

void TermScreen::vprint(usize x, usize y, fmt::string_view fmt,
                        fmt::format_args args) {
    const auto [w, h] = term.get_size();
    if (!valid_coords(w, h)) return;

    const auto it = buffer.begin() + ctoidx(x, y);
    fmt::vformat_to_n(it, w - x, fmt, args);
}

void TermScreen::vprint(usize x, usize y, fmt::text_style style,
                        fmt::string_view fmt, fmt::format_args args) {
    const auto [w, h] = term.get_size();
    if (!valid_coords(w, h)) return;

    auto it = buffer.begin() + ctoidx(x, y);
    const auto end = fmt::vformat_to_n(it, w - x, fmt, args);

    for (; it != end.out; it++) {
        it->set_style(style);
    }
}

void TermScreen::update_size() {
    term.update_size();
    update_buffer_size();
}

void TermScreen::clear() {
    const auto [w, h] = term.get_size();

    buffer.resize(w * h);
    std::ranges::fill(buffer, Pixel{});

    mark_dirty();
}

void TermScreen::update_buffer_size() {
    LOG_SCOPE_FUNCTION(9);

    clear();
}
} // namespace uppr::term
