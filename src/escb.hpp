#pragma once

#include "commom.hpp"

namespace uppr {

struct EscapeBuilder {
public:
    constexpr EscapeBuilder(int no) : fileno{no} {}

    // General ASCII Codes

    // Terminal bell
    EscapeBuilder bel() const { return put("\a"sv); }
    // Backspace
    EscapeBuilder bs() const { return put("\b"sv); }
    // Horizontal tab
    EscapeBuilder ht() const { return put("\t"sv); }
    // Linefeed (newline)
    EscapeBuilder lf() const { return put("\n"sv); }
    // Vertical tab
    EscapeBuilder vt() const { return put("\v"sv); }
    // Formfeed (also, new page)
    EscapeBuilder ff() const { return put("\f"sv); }
    // Carriage return
    EscapeBuilder cr() const { return put("\r"sv); }
    // ESC (escape sequence start character)
    EscapeBuilder esc() const { return put("\x1B["sv); }
    // DEL
    EscapeBuilder del() const { return put(0x7F); }

    // Cursor controls

    // Move cursor to 0,0
    EscapeBuilder home() const { return put("\x1B[H"); }
    // Move cursor to x,y
    EscapeBuilder move(int x, int y) const;

    // Colors!

    // Reset colors
    EscapeBuilder reset() const { return put("\x1B[0m"sv); }
    // Set bold colors
    EscapeBuilder bold() const { return put("\x1B[1m"sv); }
    // Remove bold colors
    EscapeBuilder nobold() const { return put("\x1B[22m"sv); }
    // Set dim/faint colors
    EscapeBuilder dim() const { return put("\x1B[2m"sv); }
    // Remove dim/faint colors
    EscapeBuilder nodim() const { return put("\x1B[22m"sv); }
    // Set italic
    EscapeBuilder italic() const { return put("\x1B[3m"sv); }
    // Remove italic
    EscapeBuilder noitalic() const { return put("\x1B[23m"sv); }
    // Set underline
    EscapeBuilder underline() const { return put("\x1B[4m"sv); }
    // Remove underline
    EscapeBuilder nounderline() const { return put("\x1B[24m"sv); }
    // Set blinking
    EscapeBuilder blinking() const { return put("\x1B[5m"sv); }
    // Remove blinking
    EscapeBuilder noblinking() const { return put("\x1B[25m"sv); }
    // Set inverse
    EscapeBuilder inverse() const { return put("\x1B[7m"sv); }
    // Remove inverse
    EscapeBuilder noinverse() const { return put("\x1B[27m"sv); }
    // Set invisible
    EscapeBuilder invisible() const { return put("\x1B[8m"sv); }
    // Remove invisible
    EscapeBuilder noinvisible() const { return put("\x1B[28m"sv); }
    // Set strikethrough
    EscapeBuilder strikethrough() const { return put("\x1B[9m"sv); }
    // Remove strikethrough
    EscapeBuilder nostrikethrough() const { return put("\x1B[29m"sv); }

    /**
     * Put a string view to the file.
     *
     * @return this, so that the method can be chained.
     */
    EscapeBuilder put(string_view code) const;

    /**
     * Put a single lonelly character to the file.
     */
    EscapeBuilder put(char c) const { return put({&c, 1}); }

private:
    int fileno;
};
} // namespace uppr
