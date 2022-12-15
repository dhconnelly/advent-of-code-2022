#include "util.h"

#include <cerrno>
#include <charconv>
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

int int_match(const std::smatch& m, int k) {
    int val;
    auto result = std::from_chars<int>(&*m[k].first, &*m[k].second, val);
    if (result.ec != std::errc()) die("bad int: " + m.str());
    return val;
}
