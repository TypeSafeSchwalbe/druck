
#pragma once

#include <string>

namespace druck::logging {

    void raylib(int log_level, const char* text, va_list args);

    void info(std::string message);

    void warning(std::string message);
    
    void error(std::string message);

}