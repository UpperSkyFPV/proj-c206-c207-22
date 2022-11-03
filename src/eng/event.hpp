#pragma once

#include <compare>
namespace uppr::eng {

struct Event {
    enum class NonChar : char {
        none = 0,
        esc,
        shift_tab,
    };

    char ch{};
    NonChar nch{};

    constexpr Event(char c) : ch{c} {}
    constexpr Event(NonChar nc) : nch{nc} {}
    constexpr Event(char c, NonChar nc) : ch{c}, nch{nc} {}

    constexpr auto operator<=>(const Event &) const = default;

    constexpr operator char() const { return ch; }
    constexpr operator NonChar() const { return nch; }
};
} // namespace uppr::eng
