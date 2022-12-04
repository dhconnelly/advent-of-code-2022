#include <algorithm>
#include <fstream>
#include <iostream>
#include <numeric>
#include <optional>
#include <utility>
#include <vector>

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
    auto begin = std::istream_iterator<range_pair>(ifs);
    auto end = std::istream_iterator<range_pair>();
    auto [count1, count2] = std::accumulate(
        begin, end, std::pair<int, int>(), [](auto counts, auto pair) {
            auto r = intersection(pair.first, pair.second);
            if (!r) return counts;
            if (pair.first == *r || pair.second == *r) counts.first++;
            counts.second++;
            return counts;
        });
    std::cout << count1 << std::endl;
    std::cout << count2 << std::endl;
}
