#ifndef SRC_UTIL_H_
#define SRC_UTIL_H_

#include <cstdlib>
#include <string_view>

void die(const std::string_view msg) {
    std::cerr << msg << std::endl;
    std::exit(1);
}

constexpr int kFoo = 7;

#endif  // SRC_UTIL_H_
