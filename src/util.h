#ifndef SRC_UTIL_H_
#define SRC_UTIL_H_

#include <cinttypes>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <istream>
#include <ranges>
#include <sstream>
#include <string_view>
#include <vector>

[[noreturn]] void die(const std::string_view msg);

template <typename F>
auto parse_lines(std::istream& is, F parse_line) {
    std::string line;
    std::vector<decltype(parse_line(line))> lines;
    while (std::getline(is, line)) lines.push_back(parse_line(line));
    if (is.bad()) die(strerror(errno));
    return lines;
}

template <typename F>
auto parse_entries(std::istream& is, F parse,
                   const std::string_view sep = "\n\n",
                   bool trim_trailing_newline = true) {
    if (!is.good()) die(strerror(errno));
    std::vector<decltype(parse(std::string()))> entries;

    // parse entries separated by `sep`
    std::string buf;
    int sepi = 0;
    for (int ch = is.get(); is.good(); ch = is.get()) {
        if (ch == sep[sepi]) {
            sepi++;
        } else if (sepi > 0) {
            buf.append(sep.substr(0, sepi));
            buf.push_back(ch);
            sepi = 0;
        } else {
            buf.push_back(ch);
        }
        if (sepi == sep.size()) {
            sepi = 0;
            entries.push_back(parse(buf));
            buf.clear();
        }
    }

    // collect trailing entry
    if (sepi > 0 && (sepi > 1 || sep[0] != '\n' || !trim_trailing_newline))
        buf.append(sep.substr(0, sepi));
    if (!buf.empty()) entries.push_back(parse(buf));

    if (is.bad()) die(strerror(errno));
    return entries;
}

#endif  // SRC_UTIL_H_
