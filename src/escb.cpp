#include "escb.hpp"
#include <cstdarg>
#include <cstdio>
#include <unistd.h>

namespace uppr {

EscapeBuilder EscapeBuilder::move(int x, int y) const {
    dprintf(fileno, "\x1B[%d;%dH", x, y);

    return *this;
}

EscapeBuilder EscapeBuilder::put(string_view code) const {
    ::write(fileno, code.data(), code.size());

    return *this;
}
} // namespace uppr
