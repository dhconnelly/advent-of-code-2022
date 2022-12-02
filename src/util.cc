#include "util.h"

void die(const std::string_view msg) {
    std::cerr << msg << std::endl;
    std::exit(1);
}
