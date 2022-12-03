#include <fstream>
#include <iostream>
#include <string>
#include <string_view>
#include <unordered_set>
#include <utility>

#include "util.h"

int priority(char item) {
    if ('a' <= item && item <= 'z') return 1 + (item - 'a');
    if ('A' <= item && item <= 'Z') return 27 + (item - 'A');
    die("bad item");
}

using compartment = std::unordered_set<char>;
using rucksack = std::pair<compartment, compartment>;

compartment parse_compartment(const std::string_view s) {
    compartment c;
    for (char item : s) c.insert(item);
    return c;
}

rucksack parse_rucksack(const std::string_view s) {
    int split = s.size() / 2;
    auto left = s.substr(0, split);
    auto right = s.substr(split);
    return {parse_compartment(left), parse_compartment(right)};
}

char intersect(const compartment& left, const compartment& right) {
    for (char item : right) {
        if (left.count(item)) return item;
    }
    die("no intersection");
}

int main(int argc, char* argv[]) {
    if (argc != 2) die("usage: day3 <file>");
    std::ifstream ifs(argv[1]);
    if (!ifs.good()) die(strerror(errno));
    std::string line;
    int sum = 0;
    while (std::getline(ifs, line)) {
        auto rucksack = parse_rucksack(line);
        auto item = intersect(rucksack.first, rucksack.second);
        sum += priority(item);
    }
    if (!ifs.eof()) die(strerror(errno));
    std::cout << sum << std::endl;
}
