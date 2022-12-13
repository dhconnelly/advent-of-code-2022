#ifndef SRC_UTIL_H_
#define SRC_UTIL_H_

#include <istream>
#include <string>
#include <string_view>

std::string& eatline(std::istream& is, std::string& buf);
[[noreturn]] void die(const std::string_view msg);

#endif  // SRC_UTIL_H_
