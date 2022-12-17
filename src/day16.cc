#include <deque>
#include <fstream>
#include <iostream>
#include <map>
#include <optional>
#include <regex>
#include <set>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>

#include "util.h"

using connections_map = std::map<std::string, std::vector<std::string>>;
using flow_rates_map = std::map<std::string, int64_t>;
using edge = std::pair<std::string, int64_t>;
using edges = std::map<std::string, std::vector<edge>>;

edges shortest_dists(const flow_rates_map& rates,
                     const connections_map& conns) {
    edges dists;
    for (const auto& [valve, valves] : conns) {
        auto& edges = dists[valve];
        std::deque<edge> q;
        q.emplace_back(valve, 0);
        std::set<std::string> v;
        v.insert(valve);
        while (!q.empty()) {
            auto [cur, d] = q.front();
            q.pop_front();
            for (const auto& nbr : conns.at(cur)) {
                if (v.count(nbr)) continue;
                v.insert(nbr);
                edges.emplace_back(nbr, d + 1);
                q.emplace_back(nbr, d + 1);
            }
        }
    }
    return dists;
}

using dists = std::vector<std::vector<int64_t>>;
using flow_rates = std::vector<int64_t>;
std::tuple<flow_rates, dists, int> prune(const flow_rates_map& rates_map,
                                         const edges& edges) {
    flow_rates rates;
    std::map<std::string, int> idx;
    for (const auto& [valve, rate] : rates_map) {
        if (rate == 0 && valve != "AA") continue;
        idx[valve] = rates.size();
        rates.push_back(rate);
    }
    dists dists(rates.size(), std::vector<int64_t>(rates.size(), -1));
    for (const auto& [from, es] : edges) {
        if (rates_map.at(from) == 0 && from != "AA") continue;
        dists[idx[from]][idx[from]] = 0;
        for (const auto& [to, dist] : es) {
            if (rates_map.at(to) == 0) continue;
            dists[idx[from]][idx[to]] = dist;
        }
    }
    return {rates, dists, idx["AA"]};
}

struct volcano_explorer {
    enum class state { free, collecting };
    state state;
    int valve;
    int mins_until_collected;
};

std::optional<int> tick(volcano_explorer& e) {
    if (e.state == volcano_explorer::state::free) {
        return {};
    } else if (e.mins_until_collected > 0) {
        e.mins_until_collected--;
        return {};
    } else {
        e.state = volcano_explorer::state::free;
        return e.valve;
    }
}

struct volcano {
    const dists dists;
    const flow_rates rates;
    std::vector<bool> open_valves;
    volcano_explorer explorers[2];
};

std::string key(const volcano& v, int minute) {
    std::string k;
    k.append(std::to_string(minute));
    k.push_back('|');
    for (bool b : v.open_valves) k.push_back(b ? '1' : '0');
    k.push_back('(');
    for (int i = 0; i < 2; i++) {
        const auto& e = v.explorers[i];
        k.push_back(e.state == volcano_explorer::state::free ? '1' : '0');
        k.push_back('|');
        k.append(std::to_string(e.valve));
        if (e.state == volcano_explorer::state::collecting) {
            k.push_back('|');
            k.append(std::to_string(e.mins_until_collected));
        }
        k.push_back(')');
    }
    return k;
}

int64_t releasing(const volcano& v) {
    int64_t sum = 0;
    for (int valve = 0; valve < v.open_valves.size(); valve++) {
        if (v.open_valves[valve]) sum += v.rates[valve];
    }
    return sum;
}

bool targeting(const volcano_explorer& e, int valve) {
    return e.state == volcano_explorer::state::collecting && e.valve == valve;
}

bool stuck(const volcano_explorer& explorer, int valve) {
    return explorer.state == volcano_explorer::state::collecting &&
           explorer.valve != valve;
}

void move(volcano_explorer& e, int valve, int dist) {
    if (e.state == volcano_explorer::state::collecting) {
        if (e.valve != valve) die("stuck");
    } else {
        e.state = volcano_explorer::state::collecting;
        e.mins_until_collected = dist;
        e.valve = valve;
    }
}

bool all(const volcano& v) {
    for (int i = 0; i < v.open_valves.size(); i++) {
        if (v.rates[i] > 0 && !v.open_valves[i]) return false;
    }
    return true;
}

int64_t max_released(volcano& v, int minute, int max_minute,
                     std::unordered_map<std::string, int64_t>& memo) {
    if (minute > max_minute) return 0;
    auto k = key(v, minute);
    if (auto it = memo.find(k); it != memo.end()) return it->second;

    // update current
    auto e1 = v.explorers[0], e2 = v.explorers[1];
    auto valve1 = tick(v.explorers[0]);
    if (valve1.has_value()) v.open_valves[*valve1] = true;
    auto valve2 = tick(v.explorers[1]);
    if (valve2.has_value()) v.open_valves[*valve2] = true;
    int64_t released = releasing(v);

    // try not moving
    int64_t max_after =
        all(v) ? max_released(v, minute + 1, max_minute, memo) : 0;

    // try moving
    for (int valve1 = 0; valve1 < v.open_valves.size(); valve1++) {
        for (int valve2 = 0; valve2 < v.open_valves.size(); valve2++) {
            if (valve1 == valve2) continue;
            if (v.open_valves[valve1] || v.open_valves[valve2]) continue;
            auto e1 = v.explorers[0], e2 = v.explorers[1];
            if (targeting(e1, valve2) || targeting(e2, valve1)) continue;
            if (stuck(e1, valve1) ||
                (e1.valve == valve1 &&
                 e1.state == volcano_explorer::state::free)) {
                continue;
            }
            if (stuck(e2, valve2) ||
                (e2.valve == valve2 &&
                 e2.state == volcano_explorer::state::free)) {
                continue;
            }
            int64_t dist1 = v.dists[e1.valve][valve1],
                    dist2 = v.dists[e2.valve][valve2];
            if (dist1 < 0 || dist2 < 0) continue;

            move(v.explorers[0], valve1, dist1);
            move(v.explorers[1], valve2, dist2);
            max_after = std::max(max_after,
                                 max_released(v, minute + 1, max_minute, memo));
            v.explorers[0] = e1, v.explorers[1] = e2;
        }
    }

    // reset
    if (valve1.has_value()) {
        v.open_valves[*valve1] = false;
        v.explorers[0] = e1;
    }
    if (valve2.has_value()) {
        v.open_valves[*valve2] = false;
        v.explorers[1] = e2;
    }

    memo[k] = released + max_after;
    return released + max_after;
}

int64_t max_released(const dists& dists, const flow_rates& rates, int start,
                     int max_minute) {
    volcano v{.dists = dists, .rates = rates};
    v.open_valves.resize(rates.size());
    v.explorers[0] = {volcano_explorer::state::free, start, 0};
    v.explorers[1] = {volcano_explorer::state::free, start, 0};
    std::unordered_map<std::string, int64_t> memo;
    return max_released(v, 1, max_minute, memo);
}

std::pair<flow_rates_map, connections_map> parse(std::istream&& is) {
    flow_rates_map rates;
    connections_map conns;
    for (std::string line; std::getline(is, line);) {
        static const std::regex pat(
            R"(Valve (\w+) has flow rate=(\d+); tunnels? leads? to valves? (.*)$)");
        std::smatch m;
        if (!std::regex_match(line, m, pat)) die("bad line: " + line);
        auto valve = m[1].str();
        rates[valve] = std::stoi(m[2].str());
        auto valves = m[3].str();
        static const std::regex valve_pat(R"((, )?(\w+))");
        for (std::sregex_iterator it(valves.begin(), valves.end(), valve_pat),
             end;
             it != end; it++) {
            conns[valve].push_back((*it)[2].str());
        }
    }
    return {rates, conns};
}

int main(int argc, char* argv[]) {
    if (argc != 2) die("usage: day16 <file>");
    auto [flow_rates_map, connections_map] = parse(std::ifstream(argv[1]));
    auto edges = shortest_dists(flow_rates_map, connections_map);
    auto [flow_rates, dists, start] = prune(flow_rates_map, edges);
    std::cout << max_released(dists, flow_rates, start, 26) << std::endl;
}
