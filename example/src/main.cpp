
#include "examples.hpp"
#include <iostream>

int main(int argc, char** argv) {
    if(argc == 2) {
        auto name = std::string(argv[1]);
        if(name == "triangle") {
            render_triangle();
            return 0;
        } else if(name == "car") {
            render_car();
            return 0;
        }
        std::cout << "Invalid usage - '" << name
            << "' is not a valid option" << std::endl;
    }
    std::cout
        << "Usage:" << std::endl
        << "    'example <name>'" << std::endl
        << "<name> may be one of:" << std::endl
        << "    'triangle' - renders the classic Hello, world! triangle" << std::endl
        << "    'car' - renders a stylized static model of a car" << std::endl;
    return 0;
}