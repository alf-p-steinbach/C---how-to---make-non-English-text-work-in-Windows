#pragma once
#include <limits.h>         // CHAR_BIT

namespace cppm {
    using Byte = unsigned char;
    constexpr int bits_per_byte = CHAR_BIT;
}  // namespace cppm
