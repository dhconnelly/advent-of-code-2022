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

compartment intersect(const compartment& left, const compartment& right) {
    compartment c;
    for (char item : right) {
        if (left.count(item)) c.insert(item);
    }
    return c;
}

void merge_into(compartment& into, const compartment& from) {
    for (char ch : from) into.insert(ch);
}

int main(int argc, char* argv[]) {
    if (argc != 2) die("usage: day3 <file>");
    std::ifstream ifs(argv[1]);
    if (!ifs.good()) die(strerror(errno));
    std::string line;
    int sum = 0;
    int group_count = 0;
    compartment group_common;
    int badges_sum = 0;
    while (std::getline(ifs, line)) {
        auto rucksack = parse_rucksack(line);
        auto common = intersect(rucksack.first, rucksack.second);
        sum += priority(*common.begin());

        merge_into(rucksack.first, rucksack.second);
        if (group_count == 0) group_common = rucksack.first;
        else group_common = intersect(rucksack.first, group_common);
        ++group_count;

        if (group_count == 3) {
            badges_sum += priority(*group_common.begin());
            group_common.clear();
            group_count = 0;
        }
    }
    if (!ifs.eof()) die(strerror(errno));
    std::cout << sum << std::endl;
    std::cout << badges_sum << std::endl;
}
