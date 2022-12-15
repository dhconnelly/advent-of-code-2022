#ifndef SRC_UTIL_H_
#define SRC_UTIL_H_

#include <istream>
#include <regex>
#include <string>
#include <string_view>

int int_match(const std::smatch& m, int k);
std::string& eatline(std::istream& is, std::string& buf);
[[noreturn]] void die(const std::string_view msg);

#endif  // SRC_UTIL_H_
