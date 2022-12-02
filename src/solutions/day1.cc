#include <algorithm>
#include <fstream>
#include <iostream>
#include <numeric>
#include <sstream>

#include "../aoc.h"
#include "../util.h"

namespace day1 {

std::vector<int> parse(std::istream& is) {
    auto es = parse_entries(is, [](const auto& s) {
        int sum = 0, val;
        for (std::stringstream ss(s); ss >> val;) sum += val;
        return sum;
    });
    std::sort(es.rbegin(), es.rend());
    return es;
}

int solve1(const std::vector<int>& entries) { return entries[0]; }

int solve2(const std::vector<int>& entries) {
    return std::accumulate(entries.begin(), entries.begin() + 3, 0);
}

void test() {
    constexpr auto input = R"(1000
2000
3000

4000

5000
6000

7000
8000
9000

10000)";
    std::stringstream ss(input);
    auto entries = parse(ss);
    assert(solve1(entries) == 24000);
    assert(solve2(entries) == 45000);
}

}  // namespace day1

REGISTER_SOLUTION(day1, day1::parse, day1::solve1, day1::solve2, day1::test);
