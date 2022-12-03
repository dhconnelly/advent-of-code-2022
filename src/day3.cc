#include <cstring>
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

compartment merge(const compartment& left, const compartment& right) {
    compartment c(left);
    for (char ch : right) c.insert(ch);
    return c;
}

class group {
public:
    void update(const compartment& contents) {
        if (common_.empty()) common_ = contents;
        else common_ = intersect(common_, contents);
        ++size_;
    }

    bool done() const { return size_ == 3; }
    char badge() const { return *common_.begin(); }

private:
    int size_ = 0;
    compartment common_;
};

int main(int argc, char* argv[]) {
    if (argc != 2) die("usage: day3 <file>");
    std::ifstream ifs(argv[1]);
    if (!ifs.good()) die(strerror(errno));
    int duplicates_sum = 0;
    int badges_sum = 0;
    group group;
    std::string line;
    while (std::getline(ifs, line)) {
        auto rucksack = parse_rucksack(line);
        auto common = intersect(rucksack.first, rucksack.second);
        duplicates_sum += priority(*common.begin());
        group.update(merge(rucksack.first, rucksack.second));
        if (group.done()) {
            badges_sum += priority(group.badge());
            group = {};
        }
    }
    if (!ifs.eof()) die(strerror(errno));
    std::cout << duplicates_sum << std::endl;
    std::cout << badges_sum << std::endl;
}
