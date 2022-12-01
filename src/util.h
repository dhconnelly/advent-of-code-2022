#ifndef SRC_UTIL_H_
#define SRC_UTIL_H_

#include <cinttypes>
#include <cstdlib>
#include <istream>
#include <ranges>
#include <string_view>
#include <vector>

void die(const std::string_view msg) {
    std::cerr << msg << std::endl;
    std::exit(1);
}

template <typename F>
auto parse_lines(std::istream& is, F parse_line) {
    std::string line;
    std::vector<decltype(parse_line(line))> lines;
    while (std::getline(is, line)) lines.push_back(parse_line(line));
    if (is.bad()) die(strerror(errno));
    return lines;
}

std::vector<std::string> parse_lines(std::ifstream& is) {
    std::function parse = [](const std::string& s) { return s; };
    return parse_lines(is, parse);
}

#endif  // SRC_UTIL_H_
