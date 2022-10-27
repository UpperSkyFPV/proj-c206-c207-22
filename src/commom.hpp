#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <span>
#include <string>
#include <string_view>
#include <vector>
#include <memory>

#include "loguru.hpp"

namespace uppr {

using uchar = unsigned char;
using ushort = unsigned short;

using u8 = uint8_t;
using i8 = int8_t;
using u16 = uint16_t;
using i16 = int16_t;
using u32 = uint32_t;
using i32 = int32_t;

using usize = std::size_t;

using std::array;
using std::span;
using std::string_view;

using namespace std::string_view_literals;
using namespace std::string_literals;

template <usize N>
constexpr string_view span_as_strv(span<char, N> s) {
    return {s.data(), s.size()};
}
} // namespace uppr
