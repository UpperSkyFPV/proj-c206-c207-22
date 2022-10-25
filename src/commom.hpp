#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <span>
#include <string_view>

namespace uppr {

using uchar = unsigned char;

using u8 = uint8_t;

using usize = std::size_t;

using std::array;
using std::span;
using std::string_view;

using namespace std::string_view_literals;

template <usize N>
constexpr string_view span_as_strv(span<char, N> s) {
    return {s.data(), s.size()};
}
} // namespace uppr
