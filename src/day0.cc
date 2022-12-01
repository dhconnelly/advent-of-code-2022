#include <fstream>
#include <iostream>
#include <numeric>

#include "util.h"

int main(int argc, char* argv[]) {
    if (argc != 2) die("usage: day0 <input>");
    std::ifstream ifs(argv[1]);
    auto nums = parse_lines(ifs, [](const auto& s) { return std::stol(s); });
    auto sum = std::accumulate(nums.begin(), nums.end(), 0);
    std::cout << sum << std::endl;
    return 0;
}
