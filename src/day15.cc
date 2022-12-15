#include <algorithm>
#include <fstream>
#include <iostream>
#include <numeric>
#include <regex>
#include <set>
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

int dist(pt a, pt b) {
    return std::abs(b.first - a.first) + std::abs(b.second - a.second);
}

using interval = std::pair<int, int>;

interval merge(interval a, interval b) {
    return {std::min(a.first, b.first), std::max(a.second, b.second)};
}

std::vector<interval> merge(std::vector<interval>& intervals) {
    if (intervals.empty()) return {};
    std::vector<interval> merged;
    std::sort(intervals.begin(), intervals.end(),
              [](auto l, auto r) { return l.first < r.first; });
    merged.push_back(intervals[0]);
    for (int i = 1; i < intervals.size(); i++) {
        auto& back = merged.back();
        if (intervals[i].first <= back.second) back = merge(back, intervals[i]);
        else merged.push_back(intervals[i]);
    }
    return merged;
}

int covered(const std::vector<std::pair<pt, pt>>& pairs, int y) {
    std::vector<interval> intervals;
    for (auto [sensor, beacon] : pairs) {
        int d = dist(sensor, beacon);
        int dy = std::abs(sensor.second - y);
        int dx = d - dy;
        if (dx < 0) continue;  // doesn't cover row y at all
        intervals.emplace_back(sensor.first - dx, sensor.first + dx);
    }
    intervals = merge(intervals);
    return std::accumulate(
        intervals.begin(), intervals.end(), 0, [](int sum, auto interval) {
            return sum + (interval.second - interval.first) + 1;
        });
}

int beacons(const std::vector<std::pair<pt, pt>>& pairs, int row) {
    std::set<pt> v;
    int beacons = 0;
    for (auto [sensor, beacon] : pairs) {
        if (beacon.second == row && !v.count(beacon)) {
            beacons++;
            v.insert(beacon);
        }
    }
    return beacons;
}

int main(int argc, char* argv[]) {
    if (argc != 2) die("usage: day15 <file>");
    auto pairs = parse(std::ifstream(argv[1]));
    int y = 2000000;
    std::cout << (covered(pairs, y) - beacons(pairs, y)) << std::endl;
}
