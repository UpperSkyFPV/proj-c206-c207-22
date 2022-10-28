#pragma once

#include "commom.hpp"
#include "fmt/color.h"

namespace uppr::term {

/**
 * Represent a 'pixel' in the screen.
 *
 * In our case a pixel is simply a character assossiated with a printing style.
 */
class Pixel {
public:
    /**
     * Constructs a space with default styles.
     */
    constexpr Pixel() = default;
    
    /**
     * Constructs with the given character and default styles.
     */
    constexpr Pixel(char c) : ch{c} {}

    /**
     * Constructs with the given character and styles.
     */
    constexpr Pixel(char c, const fmt::text_style &s) : ch{c}, style{s} {}

public:
    /**
     * Get the character stored inside.
     */
    constexpr char getc() const noexcept { return ch; }

    /**
     * Get the text style stored inside.
     */
    constexpr fmt::text_style get_style() const noexcept { return style; }

    /**
     * Set the text style stored inside.
     */
    constexpr void set_style(const fmt::text_style& s) noexcept { style = s; }

    // Compare objects
    constexpr bool operator==(const Pixel&) const = default;

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
