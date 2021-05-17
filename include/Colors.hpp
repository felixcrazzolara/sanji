#pragma once

namespace sanji {

namespace colors {

/* Named colors */
inline constexpr uint32_t BLACK  = 0x000000;
inline constexpr uint32_t WHITE  = 0xffffff;
inline constexpr uint32_t BLUE   = 0x1f77b4;
inline constexpr uint32_t ORANGE = 0xff7f0e;
inline constexpr uint32_t GREEN  = 0x2ca02c;
inline constexpr uint32_t RED    = 0xd62728;
inline constexpr uint32_t PURPLE = 0x9467bd;
inline constexpr uint32_t BROWN  = 0x8c564b;
inline constexpr uint32_t PINK   = 0xe377c2;
inline constexpr uint32_t GRAY   = 0x7f7f7f;
inline constexpr uint32_t OLIVE  = 0xbcbd22;
inline constexpr uint32_t CYAN   = 0x17becf;

uint32_t hsv_to_rgb(const double h, const double s, const double v);

uint32_t to_turbo_rgb(double s);

/* Named color maps */
inline constexpr int TURBO = 1;

};

};
