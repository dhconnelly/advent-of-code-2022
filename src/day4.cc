#include <algorithm>
#include <fstream>
#include <iostream>
#include <numeric>
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

std::ostream& operator<<(std::ostream& os, const range& r) {
    return os << '[' << r.first << '-' << r.second << ']';
}

int main(int argc, char* argv[]) {
    if (argc != 2) die("usage: day4 <file>");
    std::ifstream ifs(argv[1]);
    std::cout << std::accumulate(
                     std::istream_iterator<range_pair>(ifs),
                     std::istream_iterator<range_pair>(), 0,
                     [](int count, const auto& pair) {
                         auto r = intersection(pair.first, pair.second);
                         if (r && (*r == pair.first || *r == pair.second))
                             count++;
                         return count;
                     })
              << std::endl;
}
