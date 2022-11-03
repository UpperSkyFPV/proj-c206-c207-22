/**
 * @file Lots of simple inline functions to check for terminal input.
 */

#pragma once

#include "commom.hpp"

namespace uppr::term {

/**
 * Get the code for if the given character was pressed along with the ctrl key.
 *
 * Example:
 * ```c++
 * if (c == ctrl('q')) {
 *     // ctrl+q was pressed
 * }
 * ```
 */
constexpr inline char ctrl(char c) noexcept { return c & 0x1F; }

/**
 * A very simple check for if a given key is the control of some other.
 *
 * Example:
 * ```c++
 * if (is_ctrl(c, 'q')) {
 *     // ctrl+q was pressed
 * }
 * ```
 */
constexpr inline bool is_ctrl(char c, char match) noexcept {
    return c == ctrl(match);
}

/**
 * If the given character matches the escape control character that starts
 * special keys sequences.
 */
constexpr inline bool is_escseq(char c) noexcept { return c == '\x1B'; }

/**
 * If the given escaped character sequence represents the ESC key.
 */
constexpr inline bool is_esc(string_view s) noexcept { return s.empty(); }

/**
 * If the given escaped character sequence represents the left array key.
 */
constexpr inline bool is_left(string_view s) noexcept { return s == "[D"sv; }

/**
 * If the given escaped character sequence represents the right array key.
 */
constexpr inline bool is_right(string_view s) noexcept { return s == "[C"sv; }

/**
 * If the given escaped character sequence represents the up array key.
 */
constexpr inline bool is_up(string_view s) noexcept { return s == "[A"sv; }

/**
 * If the given escaped character sequence represents the down array key.
 */
constexpr inline bool is_down(string_view s) noexcept { return s == "[B"sv; }

/**
 * If the given escaped character sequence represents the shift-tab key combination.
 */
constexpr inline bool is_shif_tab(string_view s) noexcept { return s == "[Z"sv; }
} // namespace uppr::term
