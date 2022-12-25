#include <cinttypes>
#include <fstream>
#include <iostream>
#include <string>

#include "util.h"

int64_t value(char c) {
    switch (c) {
        case '-': return -1;
        case '=': return -2;
        default: return c - '0';
    }
}

int64_t parse(const std::string& line) {
    int64_t sum = 0;
    int64_t place = 1;
    static constexpr int64_t base = 5;
    for (auto it = line.rbegin(); it != line.rend(); ++it) {
        char c = *it;
        sum += value(c) * place;
        place *= base;
    }
    return sum;
}

int64_t parse(std::istream&& is) {
    int64_t sum = 0;
    for (std::string line; std::getline(is, line);) sum += parse(line);
    return sum;
}

int main(int argc, char* argv[]) {
    if (argc != 2) die("usage: day25 <file>");
    int64_t sum = parse(std::ifstream(argv[1]));
    std::cout << sum << std::endl;
}
