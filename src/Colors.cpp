#include <stdint.h>
#include <cmath>

// Extern declarations
extern float turbo_srgb_floats[256][3];

namespace sanji {

namespace colors {

typedef struct {
    double r;       // a fraction between 0 and 1
    double g;       // a fraction between 0 and 1
    double b;       // a fraction between 0 and 1
} rgb;

typedef struct {
    double h;       // angle in degrees
    double s;       // a fraction between 0 and 1
    double v;       // a fraction between 0 and 1
} hsv;

// From: https://stackoverflow.com/questions/3018313/algorithm-to-convert-rgb-to-hsv-and-hsv-to-rgb-in-range-0-255-for-both
uint32_t hsv_to_rgb(const double h, const double s, const double v) {
    hsv         in{h*360.0,s,v};
    double      hh, p, q, t, ff;
    long        i;
    rgb         out;

    if(in.s <= 0.0) {       // < is bogus, just shuts up warnings
        out.r = in.v;
        out.g = in.v;
        out.b = in.v;
        return (static_cast<uint32_t>(out.r*(0xffu<<16u))&0xff0000)+(static_cast<uint32_t>(out.g*(0xffu<<8u))&0xff00u)+(static_cast<uint32_t>(out.b*0xffu)&0xffu);
    }
    hh = in.h;
    if(hh >= 360.0) hh = 0.0;
    hh /= 60.0;
    i = (long)hh;
    ff = hh - i;
    p = in.v * (1.0 - in.s);
    q = in.v * (1.0 - (in.s * ff));
    t = in.v * (1.0 - (in.s * (1.0 - ff)));

    switch(i) {
    case 0:
        out.r = in.v;
        out.g = t;
        out.b = p;
        break;
    case 1:
        out.r = q;
        out.g = in.v;
        out.b = p;
        break;
    case 2:
        out.r = p;
        out.g = in.v;
        out.b = t;
        break;

    case 3:
        out.r = p;
        out.g = q;
        out.b = in.v;
        break;
    case 4:
        out.r = t;
        out.g = p;
        out.b = in.v;
        break;
    case 5:
    default:
        out.r = in.v;
        out.g = p;
        out.b = q;
        break;
    }
    return (static_cast<uint32_t>(out.r*(0xffu<<16u))&0xff0000)+(static_cast<uint32_t>(out.g*(0xffu<<8u))&0xff00u)+(static_cast<uint32_t>(out.b*0xffu)&0xffu);
}

// s is assumed to be in the open interval [0,1)
uint32_t to_turbo_rgb(double s) {
    s         = std::min(1.0,std::max(0.0,s));
    uint8_t i = std::min(255.0,std::floor(s*256.0));
    return (static_cast<uint32_t>(std::floor(turbo_srgb_floats[i][0]*256.0))<<16)
          +(static_cast<uint32_t>(std::floor(turbo_srgb_floats[i][1]*256.0))<<8)
          +static_cast<uint32_t>(std::floor(turbo_srgb_floats[i][2]*256.0));
}

};

};
