#pragma once

#include <tuple>

namespace sanji {

namespace colors {

/* Type definitions */
using Color = std::tuple<uint8_t,uint8_t,uint8_t>;

/* Named colors */
inline constexpr Color BLACK  = Color{0x00,0x00,0x00};
inline constexpr Color WHITE  = Color{0xff,0xff,0xff};
inline constexpr Color BLUE   = Color{0x1f,0x77,0xb4};
inline constexpr Color ORANGE = Color{0xff,0x7f,0x0e};
inline constexpr Color GREEN  = Color{0x2c,0xa0,0x2c};
inline constexpr Color RED    = Color{0xd6,0x27,0x28};
inline constexpr Color PURPLE = Color{0x94,0x67,0xbd};
inline constexpr Color BROWN  = Color{0x8c,0x56,0x4b};
inline constexpr Color PINK   = Color{0xe3,0x77,0xc2};
inline constexpr Color GRAY   = Color{0x7f,0x7f,0x7f};
inline constexpr Color OLIVE  = Color{0xbc,0xbd,0x22};
inline constexpr Color CYAN   = Color{0x17,0xbe,0xcf};

};

};
