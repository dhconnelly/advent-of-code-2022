#include "util.h"

#include <cstdlib>
#include <iostream>

[[noreturn]] void die(const std::string_view msg) {
    std::cerr << msg << std::endl;
    std::exit(1);
}
