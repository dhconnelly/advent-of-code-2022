#include <algorithm>
#include <array>
#include <fstream>
#include <iostream>
#include <iterator>
#include <numeric>
#include <optional>
#include <regex>
#include <sstream>
#include <string>
#include <string_view>
#include <thread>
#include <tuple>
#include <unordered_map>
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
    if (!std::getline(is, line)) {
        is.setstate(std::istream::failbit);
        return is;
    }
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

static constexpr int kMax = std::numeric_limits<int>::max();

int64_t when_affordable(const counts& balance, const counts& robots,
                        const counts& cost) {
    int64_t max = 0;
    for (int i = 0; i < 4; i++) {
        int64_t deficit = cost[i] - balance[i];
        if (cost[i] == 0 || deficit <= 0) continue;
        if (deficit > 0 && robots[i] == 0) return kMax;
        int64_t t = deficit / robots[i];
        max = std::max(max, deficit % robots[i] == 0 ? t : t + 1);
    }
    return max;
}

inline bool positive(const counts& x) {
    for (int i = 0; i < 4; i++)
        if (x[i] < 0) return false;
    return true;
}

inline bool positive_forecast(const counts& balance, const counts& robots,
                              int steps_left) {
    for (int i = 0; i < 4; i++)
        if ((balance[i] + steps_left * robots[i]) < 0) return false;
    return true;
}

inline uint64_t k(const counts& balance, const counts& robots, int steps) {
    uint64_t x = static_cast<uint8_t>(balance[0]);
    x = (x << 7) | static_cast<uint8_t>(balance[1]);
    x = (x << 7) | static_cast<uint8_t>(balance[2]);
    x = (x << 7) | static_cast<uint8_t>(balance[3]);
    x = (x << 7) | static_cast<uint8_t>(robots[0]);
    x = (x << 7) | static_cast<uint8_t>(robots[1]);
    x = (x << 7) | static_cast<uint8_t>(robots[2]);
    x = (x << 7) | static_cast<uint8_t>(robots[3]);
    x = (x << 6) | static_cast<uint8_t>(steps);
    return x;
}

bool build(const blueprint& bp, const counts& max_robots, counts& balance,
           counts& robots, int steps_left,
           std::unordered_map<uint64_t, bool>& memo) {
    if (steps_left <= 0) return positive(balance);
    if (positive_forecast(balance, robots, steps_left)) return true;
    uint64_t key = k(balance, robots, steps_left);
    if (auto it = memo.find(key); it != memo.end()) return it->second;

    // which bot to build next?
    // full disclosure: needed a hint here to frame it this way
    for (int i = 3; i >= 0; i--) {
        if (robots[i] >= max_robots[i]) continue;
        int64_t t = when_affordable(balance, robots, bp.costs[i]);
        if (t >= steps_left) continue;

        // buy and backtrack if unsuccessful
        for (int j = 0; j < 4; j++) {
            balance[j] += robots[j] * (t + 1) - bp.costs[i][j];
        }
        robots[i]++;
        if (build(bp, max_robots, balance, robots, steps_left - t - 1, memo)) {
            return memo[key] = true;
        }
        robots[i]--;
        for (int j = 0; j < 4; j++) {
            balance[j] -= robots[j] * (t + 1) - bp.costs[i][j];
        }
    }

    return memo[key] = false;
}

// this was another hint
counts max_robots(const blueprint& bp) {
    counts max_robots = {0, 0, 0, kMax};
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            max_robots[j] = std::max(max_robots[j], bp.costs[i][j]);
        }
    }
    return max_robots;
}

void max_geodes(const blueprint& bp, int max_steps, int& result) {
    std::unordered_map<uint64_t, bool> memo;
    for (int n = 1;; n++) {
        counts robots = {1, 0, 0, 0};
        counts want = {0, 0, 0, -n};
        if (!build(bp, max_robots(bp), want, robots, max_steps, memo)) {
            result = n - 1;
            return;
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) die("usage: day19 <file>");
    std::ifstream ifs(argv[1]);
    std::istream_iterator<blueprint> begin(ifs), end;
    const std::vector<blueprint> blueprints(begin, end);

    // part 1
    int64_t sum = 0;
    for (int i = 0; i < blueprints.size(); i++) {
        int result;
        max_geodes(blueprints[i], 24, result);
        sum += blueprints[i].id * result;
    }
    std::cout << sum << std::endl;

    // part 2 in threads
    int t = std::min(3LU, blueprints.size());
    std::vector<std::thread> threads;
    std::vector<int> results(t);
    for (int i = 0; i < t; i++) {
        threads.emplace_back(max_geodes, blueprints[i], 32,
                             std::ref(results[i]));
    }
    int64_t prod = 1;
    for (int i = 0; i < t; i++) {
        threads[i].join();
        prod *= results[i];
    }
    std::cout << prod << std::endl;
}
