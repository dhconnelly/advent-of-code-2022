#include <algorithm>
#include <array>
#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
#include <optional>
#include <regex>
#include <sstream>
#include <string>
#include <string_view>
#include <thread>
#include <tuple>
#include <utility>
#include <vector>

#include "util.h"

enum mineral { ore = 0, clay = 1, obsidian = 2, geode = 3 };
// TODO: are constexpr maps supported?
constexpr std::string_view kRobots[] = {"ore", "clay", "obsidian", "geode"};
int lookup(const std::string_view name) {
    return std::find(kRobots, kRobots + 4, name) - kRobots;
}

using counts = std::array<int, 4>;
counts zero() { return {0, 0, 0, 0}; }

void print(std::ostream& os, counts amts) {
    os << '[';
    for (int i = 0; i < 4; i++) os << ' ' << amts[i] << ' ';
    os << ']';
}

struct blueprint {
    int id;
    std::array<counts, 4> costs;
};

std::ostream& operator<<(std::ostream& os, const blueprint& bp) {
    os << "[" << bp.id << " | ";
    for (int i = 0; i < 4; i++) {
        std::cout << "( ";
        for (int j = 0; j < 4; j++) os << bp.costs[i][j] << ' ';
        std::cout << ") ";
    }
    return os << "]";
}

std::istream& operator>>(std::istream& is, blueprint& bp) {
    std::string line;
    if (!std::getline(is, line)) return is;
    std::smatch m;

    // id
    static const std::regex id_pat(R"(Blueprint (\d+))");
    if (!std::regex_search(line, m, id_pat)) die("no id");
    bp.id = int_match(m, 1);

    // robots
    static const std::regex robot_pat(R"(Each (\w+) robot costs ([ \w\d]+)\.)");
    std::sregex_iterator it(line.begin(), line.end(), robot_pat), end;
    for (int i = 0; i < 4; i++) {
        if (it == end) die("bad robots: " + line);
        auto m = *it++;
        if (m[1].str() != kRobots[i]) die("bad robot: " + m[1].str());

        // costs
        bp.costs[i] = {0, 0, 0, 0};
        static const std::regex costs_pat(R"((\d+) (\w+))");
        std::sregex_iterator it2(m[2].first, m[2].second, costs_pat), end2;
        for (; it2 != end2; it2++) {
            bp.costs[i][lookup((*it2)[2].str())] = int_match(*it2, 1);
        }
    }
    return is;
}

bool positive(const counts& x) {
    return std::all_of(x.begin(), x.end(), [](int x) { return x >= 0; });
}

int forecast(const counts& have, const counts& robots, int steps_left, int i) {
    return have[i] + robots[i] * steps_left;
}

counts forecast(counts have, const counts& robots, int steps_left) {
    for (int i = 0; i < 4; i++) have[i] += steps_left * robots[i];
    return have;
}

counts remove(counts from, const counts& amts) {
    for (int i = 0; i < 4; i++) from[i] -= amts[i];
    return from;
}

counts collect(counts have, const counts& robots) {
    for (int i = 0; i < 4; i++) have[i] += robots[i];
    return have;
}

bool can_afford(const counts& cost, const counts& balance) {
    for (int i = 0; i < 4; i++)
        if (cost[i] > 0 && balance[i] < cost[i]) return false;
    return true;
}

counts incr(counts robots, int i) {
    robots[i]++;
    return robots;
}

using key = std::tuple<int, int, int, int, int, int, int, int, int>;
key k(const counts& balance, const counts& robots, int steps) {
    return {balance[0], balance[1], balance[2], balance[3], robots[0],
            robots[1],  robots[2],  robots[3],  steps};
}

bool reachable(const blueprint& bp, counts balance, counts robots,
               int steps_left, std::map<key, bool>& memo) {
    if (steps_left <= 0) return positive(balance);
    if (positive(forecast(balance, robots, steps_left))) return true;
    auto key = k(balance, robots, steps_left);
    if (auto it = memo.find(key); it != memo.end()) return it->second;
    for (int i = 3; i >= 0; i--) {
        if (can_afford(bp.costs[i], balance) &&
            reachable(bp, collect(remove(balance, bp.costs[i]), robots),
                      incr(robots, i), steps_left - 1, memo)) {
            return memo[key] = true;
        }
    }
    return memo[key] = reachable(bp, collect(balance, robots), robots,
                                 steps_left - 1, memo);
}

void max_geodes(const blueprint& bp, int max_steps, int64_t& result) {
    std::map<key, bool> memo;
    for (int n = 1;; n++) {
        if (!reachable(bp, {0, 0, 0, -n}, {1, 0, 0, 0}, max_steps, memo)) {
            result = n - 1;
            return;
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) die("usage: day19 <file>");
    std::ifstream ifs(argv[1]);
    std::istream_iterator<blueprint> begin(ifs), end;
    std::vector<blueprint> blueprints(begin, end);

    // compute the quality scores in parallel
    std::vector<int64_t> max(blueprints.size());
    std::vector<std::thread> threads;
    for (int i = 0; i < blueprints.size(); i++) {
        threads.emplace_back(max_geodes, blueprints[i], 24, std::ref(max[i]));
    }
    int64_t sum = 0;
    for (int i = 0; i < blueprints.size(); i++) {
        threads[i].join();
        sum += blueprints[i].id * max[i];
    }
    std::cout << sum << std::endl;
}
