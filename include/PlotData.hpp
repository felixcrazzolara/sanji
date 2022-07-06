/* 
 * Author: Felix Crazzolara
 */ 
#pragma once

#include "types.hpp"

namespace sanji_ {

using Style = std::unordered_map<string,double>;

struct PlotData {

PlotData(const uint priority, const vec_ptr x, const Style& style) :
    priority(priority),
    x(x),
    style(style)
{}

uint    priority;
vec_ptr x;
Style   style;

};

struct LineData : public PlotData {

LineData(const uint priority, const vec_ptr x, const mat_ptr y, const Style& style) :
    PlotData(priority,x,style),
    y(y)
{}

mat_ptr y;

};

struct ArrowData : public PlotData {

ArrowData(const uint priority, const vec_ptr x, const vec_ptr y, const vec_ptr u, const vec_ptr v,
          const Style& style) :
    PlotData(priority,x,style),
    y(y),
    u(u),
    v(v)
{}

vec_ptr y;
vec_ptr u;
vec_ptr v;

};

};
