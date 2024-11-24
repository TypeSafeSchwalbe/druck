
#pragma once

#include <raylib.h>
#include "rendering.hpp"

namespace druck {

    void init(const char* title, int width, int height, int fps);
    bool is_running();
    void display_buffer(rendering::Surface* buffer);
    void stop();

}