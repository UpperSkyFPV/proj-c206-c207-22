#pragma once

#include "commom.hpp"
#include "fmt/color.h"

namespace uppr::term {

class Pixel {
public:
    constexpr Pixel() = default;
    constexpr Pixel(char c) : ch{c} {}
    constexpr Pixel(char c, const fmt::text_style &s) : ch{c}, style{s} {}

public:
    constexpr char getc() const noexcept { return ch; }
    constexpr fmt::text_style get_style() const noexcept { return style; }

    constexpr void set_style(const fmt::text_style& s) noexcept { style = s; }

private:
    /**
     * The character stored in the pixel.
     */
    char ch{' '};

    /**
     * The formatting style of the character.
     */
    fmt::text_style style;
};
} // namespace uppr::term
