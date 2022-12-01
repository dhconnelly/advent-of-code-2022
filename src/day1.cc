#include <algorithm>
#include <fstream>
#include <iostream>
#include <numeric>

#include "util.h"

std::vector<int> parse_groups(const std::vector<std::string>& lines) {
    std::vector<int> groups(1);
    for (const auto& line : lines) {
        if (line.empty()) groups.push_back(0);
        else groups.back() += std::stoi(line);
    }
    return groups;
}

int main(int argc, char* argv[]) {
    if (argc != 2) die("usage: day1 <input>");
    std::ifstream ifs(argv[1]);
    auto lines = parse_lines(ifs);
    auto groups = parse_groups(lines);
    std::sort(groups.begin(), groups.end());
    std::cout << *groups.rbegin() << std::endl;
    std::cout << std::accumulate(groups.rbegin(), groups.rbegin() + 3, 0)
              << std::endl;
}
