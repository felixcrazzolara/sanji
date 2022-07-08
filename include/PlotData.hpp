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

class LineDataWrapper {
public:

LineDataWrapper() :
    has_data_(false)
{}

vector<LineData>& data() {
    return data_;
}

const vector<LineData>& c_data() const {
    return data_;
}

void add(const uint priority, const vec_ptr x, const mat_ptr y, const Style& style) {
    data_.emplace_back(priority, x, y, style);

    if (x->size() > 0) {
        has_data_ = true;
    }
}

bool hasData() const {
    return has_data_;
}

private:

vector<LineData> data_;
bool             has_data_;

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

class ArrowDataWrapper {
public: 

ArrowDataWrapper() :
    has_data_(false)
{}

vector<ArrowData>& data() {
    return data_;
}

const vector<ArrowData>& c_data() const {
    return data_;
}

void add(const uint priority, const vec_ptr x, const vec_ptr y, const vec_ptr u, const vec_ptr v,
         const Style& style) {
    data_.emplace_back(priority, x, y, u, v, style);

    if (x->size() > 0) {
        has_data_ = true;
    }
}

bool hasData() const {
    return has_data_;
}

private:

vector<ArrowData> data_;
bool              has_data_;

};

};
