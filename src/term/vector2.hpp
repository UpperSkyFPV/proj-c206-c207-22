#pragma once

#include "commom.hpp"

namespace uppr::term {

/**
 * A very simple 2D vector class.
 */
template <typename T>
struct Vector2 {
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

    /**
     * Add to another vector.
     */
    constexpr Vector2 move(T x, T y) const noexcept {
        return *this + Vector2{x, y};
    }

    /**
     * Negate the vector (which means negate both x and y).
     */
    constexpr Vector2 operator-() const noexcept { return {-x, -y}; }

    /**
     * Add to another vector.
     */
    constexpr Vector2 operator+(const Vector2 &o) const noexcept {
        return {x + o.x, y + o.y};
    }

    /**
     * Subtract another vector.
     */
    constexpr Vector2 operator-(const Vector2 &o) const noexcept {
        return {x - o.x, y - o.y};
    }

    /**
     * Add to a scalar.
     */
    constexpr Vector2 operator+(T scalar) const noexcept {
        return {x + scalar, y + scalar};
    }

    /**
     * Subtract a scalar.
     */
    constexpr Vector2 operator-(T scalar) const noexcept {
        return {x - scalar, y - scalar};
    }

    /**
     * Divide by another vector (is this actually correct?)
     */
    constexpr Vector2 operator/(const Vector2 &o) const noexcept {
        return {x / o.x, y / o.y};
    }

    constexpr Vector2 &operator+=(const Vector2 &o) noexcept {
        x += o.x;
        y += o.y;

        return *this;
    }

    constexpr Vector2 &operator+=(T scalar) noexcept {
        x += scalar;
        y += scalar;

        return *this;
    }

    constexpr Vector2 &operator-=(const Vector2 &o) noexcept {
        x -= o.x;
        y -= o.y;

        return *this;
    }

    constexpr Vector2 &operator-=(T scalar) noexcept {
        x -= scalar;
        y -= scalar;

        return *this;
    }

    constexpr auto operator<=>(const Vector2 &) const = default;

    T x{};
    T y{};
};

using Transform = Vector2<int>;
using Size = Vector2<usize>;
} // namespace uppr::term
