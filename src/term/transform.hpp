#pragma once

#include "commom.hpp"

namespace uppr::term {

/**
 * A transform class is just 2D coordinates, but intended to use for localizing
 * coodinates.
 *
 * Say we have the following screen configuration:
 * ```
 * (0, 0)
 * +--------------------------------------------------+
 * |                                                  |
 * |    +-------------+                               |
 * |    |             |                               |
 * |    | *           |                               |
 * |    |             |                               |
 * |    +-------------+                               |
 * |                                                  |
 * +--------------------------------------------------+
 *                                                    (50, 7)
 * ```
 *
 * To position the litte start, we could use its absoulute coordinates (7, 3),
 * or we could define using its local coordinates (1, 1). The second one is
 * obviously preffered, but to do that we need to know the transform of its
 * parent (the smaller box) and of itself. That is the purpose of this class.
 */
template <typename T>
struct BasicTransform {
    /**
     * Get the x, y coordinates.
     */
    constexpr std::pair<T, T> get() const noexcept { return {x, y}; }

    /**
     * Get the x coordinate.
     */
    constexpr T getx() const noexcept { return x; }

    /**
     * Get the y coordinate.
     */
    constexpr T gety() const noexcept { return y; }

    constexpr BasicTransform operator+(const BasicTransform &o) const noexcept {
        return {x + o.x, y + o.y};
    }

    constexpr auto operator<=>(const BasicTransform &) const = default;

    T x{};
    T y{};
};

using Transform = BasicTransform<int>;
} // namespace uppr::term
