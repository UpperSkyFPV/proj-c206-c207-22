#pragma once

#include "commom.hpp"

#include <cstdarg>

namespace uppr::io {

/**
 * Generic interface for reading stuff.
 */
template <typename T = char>
class Reader {
public:
    using item_type = T;

    virtual ~Reader() {}

    /**
     * Read from the source (or at least try to).
     *
     * Tries to read enougth items to fill the buffer, which is not garanteed.
     * Returns a span of only the filled items in the buffer.
     */
    virtual span<T> read(span<T> buf) = 0;

    /**
     * Read one item from the source.
     */
    char getc() {
        char c;
        read({&c, 1});
        return c;
    }
};

/**
 * Generic interface for writing stuff.
 */
template <typename T = char>
class Writer {
public:
    using item_type = T;

    virtual ~Writer() {}

    /**
     * Write items to destination.
     *
     * @returns How many items were actually written.
     */
    virtual usize write(span<const T> data) = 0;

    /**
     * Write a formatted string to the destination. This does not need to be
     * implemented for every writer, and a missing implementation does nothing.
     */
    void writef(const char *const fmt, ...) {
        va_list va;
        va_start(va, fmt);

        vwritef(fmt, va);

        va_end(va);
    }

    /**
     * Write a formatted string to the destination. This does not need to be
     * implemented for every writer, and a missing implementation does nothing.
     */
    virtual void vwritef(const char *const fmt, va_list args) {}

    /**
     * Write a single character to the destination.
     */
    usize putc(char c) { return write({&c, 1}); }
};

/**
 * Generic interface for reading and writing stuff.
 */
template <typename T = char>
class ReaderWriter : public Reader<T>, public Writer<T> {
public:
    using item_type = T;

    virtual ~ReaderWriter() {}
};

/**
 * Concrete implementation for reading and writing to a POSIX file descriptor.
 */
class FileReaderWriter : public ReaderWriter<char> {
public:
    /**
     * Construct, wrapping a given file descriptor.
     */
    FileReaderWriter(int fd_) : fd{fd_} {}

    span<item_type> read(span<item_type> buf) override;
    usize write(span<const item_type> data) override;
    void vwritef(const char *const fmt, va_list args) override;

private:
    // The file descriptor
    int fd;
};
} // namespace uppr::io
