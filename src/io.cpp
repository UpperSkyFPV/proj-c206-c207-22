#include "io.hpp"
#include <cstdio>
#include <unistd.h>

#include "printf.h"

namespace uppr {

auto io::FileReaderWriter::read(span<item_type> buf) -> std::span<item_type> {
    const auto n = ::read(fd, buf.data(), buf.size());
    // Errors when reading make an empty span. This does not handle all edge
    // cases, but is sufficient for us.
    if (n < 0) return {};

    return buf.subspan(0, n);
}

usize io::FileReaderWriter::write(span<const item_type> data) {
    return ::write(fd, data.data(), data.size());
}

void io::FileReaderWriter::vwritef(const char *const fmt, va_list args) {
    const auto out = [](char c, void *ctx) {
        static_cast<FileReaderWriter *>(ctx)->putc(c);
    };

    vfctprintf(out, this, fmt, args);
}
} // namespace uppr
