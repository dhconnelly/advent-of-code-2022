#include <fstream>
#include <iostream>
#include <iterator>
#include <optional>
#include <utility>

#include "util.h"

struct range : std::pair<int, int> {
    using std::pair<int, int>::pair;
};

std::istream& operator>>(std::istream& is, range& r) {
    int lo, hi;
    if (!(is >> lo)) return is;
    if (is.get() != '-') return is;
    if (!(is >> hi)) return is;
    r = {lo, hi};
    return is;
}

struct range_pair : std::pair<range, range> {
    using std::pair<range, range>::pair;
};

std::istream& operator>>(std::istream& is, range_pair& p) {
    range left, right;
    if (!(is >> left)) return is;
    if (is.get() != ',') return is;
    if (!(is >> right)) return is;
    p = {left, right};
    return is;
}

std::optional<range> intersection(const range& r1, const range& r2) {
    range result{std::max(r1.first, r2.first), std::min(r1.second, r2.second)};
    if (result.first > result.second) return {};
    return result;
}

int main(int argc, char* argv[]) {
    if (argc != 2) die("usage: day4 <file>");
    std::ifstream ifs(argv[1]);
    int count_contained = 0, count_overlap = 0;
    for (auto it = std::istream_iterator<range_pair>(ifs);
         it != std::istream_iterator<range_pair>(); ++it) {
        auto r = intersection(it->first, it->second);
        if (!r) continue;
        if (it->first == *r || it->second == *r) count_contained++;
        count_overlap++;
    }
    std::cout << count_contained << std::endl;
    std::cout << count_overlap << std::endl;
}
