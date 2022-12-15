#include <fstream>
#include <iostream>
#include <regex>
#include <string>
#include <utility>
#include <vector>

#include "util.h"

using pt = std::pair<int, int>;

std::vector<std::pair<pt, pt>> parse(std::istream&& is) {
    std::vector<std::pair<pt, pt>> pairs;
    for (std::string line; std::getline(is, line);) {
        static const std::regex pat(R"(x=(-?\d+), y=(-?\d+))");
        std::sregex_iterator it(line.begin(), line.end(), pat);
        pt sensor(int_match(*it, 1), int_match(*it, 2));
        ++it;
        pt beacon(int_match(*it, 1), int_match(*it, 2));
        pairs.emplace_back(sensor, beacon);
    }
    return pairs;
}

std::ostream& print(pt p) {
    return std::cout << '[' << p.first << ',' << p.second << ']';
}

int main(int argc, char* argv[]) {
    if (argc != 2) die("usage: day15 <file>");
    auto pairs = parse(std::ifstream(argv[1]));
    for (auto [sensor, beacon] : pairs) {
        print(sensor);
        print(beacon);
        std::cout << std::endl;
    }
}
