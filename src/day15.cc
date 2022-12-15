#include <algorithm>
#include <fstream>
#include <iostream>
#include <limits>
#include <numeric>
#include <regex>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include "util.h"

using pt = std::pair<int64_t, int64_t>;
using interval = std::pair<int64_t, int64_t>;

interval merge(interval a, interval b) {
    return {std::min(a.first, b.first), std::max(a.second, b.second)};
}

void merge(std::vector<interval>& intervals) {
    if (intervals.empty()) return;
    std::vector<interval> merged;
    std::sort(intervals.begin(), intervals.end(),
              [](auto l, auto r) { return l.first < r.first; });
    merged.push_back(intervals[0]);
    for (size_t i = 1; i < intervals.size(); i++) {
        auto& back = merged.back();
        if (intervals[i].first <= back.second) back = merge(back, intervals[i]);
        else merged.push_back(intervals[i]);
    }
    intervals = merged;
}

void clamp(std::vector<interval>& intervals, int64_t min_x, int64_t max_x) {
    for (auto it = intervals.begin(); it != intervals.end();) {
        if (it->first >= min_x) break;
        else if (it->second < min_x) it = intervals.erase(it++);
        else if (it->first < min_x) {
            it->first = min_x;
            break;
        }
    }
    for (auto it = intervals.rbegin(); it != intervals.rend();) {
        if (it->second <= max_x) break;
        else if (it->first > max_x) intervals.erase((++it).base());
        else if (it->second > max_x) {
            it->second = max_x;
            break;
        }
    }
}

int64_t dist(pt a, pt b) {
    return std::abs(b.first - a.first) + std::abs(b.second - a.second);
}

std::vector<interval> coverage(
    const std::vector<std::pair<pt, pt>>& pairs, int64_t y,
    int64_t min_x = std::numeric_limits<int64_t>::min(),
    int64_t max_x = std::numeric_limits<int64_t>::max()) {
    std::vector<interval> intervals;
    for (auto [sensor, beacon] : pairs) {
        int64_t d = dist(sensor, beacon);
        int64_t dy = std::abs(sensor.second - y);
        int64_t dx = d - dy;
        if (dx < 0) continue;  // doesn't cover row y at all
        intervals.emplace_back(sensor.first - dx, sensor.first + dx);
    }
    merge(intervals);
    clamp(intervals, min_x, max_x);
    return intervals;
}

int64_t covered(const std::vector<interval>& intervals) {
    return std::accumulate(
        intervals.begin(), intervals.end(), 0, [](int64_t sum, auto interval) {
            return sum + (interval.second - interval.first) + 1;
        });
}

int64_t beacons(const std::vector<std::pair<pt, pt>>& pairs, int64_t row) {
    std::set<pt> v;
    int64_t beacons = 0;
    for (auto [sensor, beacon] : pairs) {
        if (beacon.second == row && !v.count(beacon)) {
            beacons++;
            v.insert(beacon);
        }
    }
    return beacons;
}

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

int main(int argc, char* argv[]) {
    if (argc != 2) die("usage: day15 <file>");
    const auto pairs = parse(std::ifstream(argv[1]));

    // part 1
    int64_t y = 2000000;
    auto intervals = coverage(pairs, y);
    std::cout << (covered(intervals) - beacons(pairs, y)) << std::endl;

    // part 2
    int64_t max = 4000000, x = -1;
    for (y = 0; y <= max; y++) {
        auto intervals = coverage(pairs, y, 0, max);
        if (intervals.size() == 2) {
            x = intervals[0].second + 1;
            break;
        }
    }
    if (x < 0) die("not found");
    std::cout << (4000000 * x + y) << std::endl;
}
