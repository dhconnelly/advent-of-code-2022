#include "util.h"

#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <iostream>

[[noreturn]] void die(const std::string_view msg) {
    std::cerr << msg << std::endl;
    std::exit(1);
}

std::string& eatline(std::istream& is, std::string& buf) {
    if (!std::getline(is, buf)) die(strerror(errno));
    return buf;
}
