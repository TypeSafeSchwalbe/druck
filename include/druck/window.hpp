
#pragma once

#include <raylib.h>
#include "rendering.hpp"

namespace druck::window {

    void init(const char* title, int width, int height, int fps);
    bool should_close();
    int width();
    int height();
    druck::math::Vec<2> size();
    double delta_time();
    void display_buffer(rendering::Surface& buffer);
    void close();

}