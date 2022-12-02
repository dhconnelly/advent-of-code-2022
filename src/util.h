#ifndef SRC_UTIL_H_
#define SRC_UTIL_H_

#include <string_view>

[[noreturn]] void die(const std::string_view msg);

#endif  // SRC_UTIL_H_
