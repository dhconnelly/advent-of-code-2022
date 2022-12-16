#include <deque>
#include <fstream>
#include <iostream>
#include <map>
#include <regex>
#include <set>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>

#include "util.h"

using connections_map = std::map<std::string, std::vector<std::string>>;
using flow_rates_map = std::map<std::string, int64_t>;

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
        if (rate == 0) continue;
        idx[valve] = rates.size();
        rates.push_back(rate);
    }
    std::cout << "reduced size from " << rates_map.size() << " to "
              << rates.size() << std::endl;
    dists dists(rates.size(), std::vector<int64_t>(rates.size(), -1));
    for (const auto& [from, es] : edges) {
        if (rates_map.at(from) == 0) continue;
        dists[idx[from]][idx[from]] = 0;
        for (const auto& [to, dist] : es) {
            if (rates_map.at(to) == 0) continue;
            std::cout << "===" << std::endl;
            std::cout << "dist(" << from << ", " << to << ") = " << dist
                      << std::endl;
            std::cout << "dist(" << idx[from] << ", " << idx[to]
                      << ") = " << dist << std::endl;
            std::cout << dists.size() << "," << dists[idx[from]].size()
                      << std::endl;
            dists[idx[from]][idx[to]] = dist;
        }
    }
    return {rates, dists, idx["AA"]};
}

std::string key(int cur, int minute, const std::vector<bool>& open) {
    std::string k;
    k.append(std::to_string(cur));
    k.push_back('|');
    k.append(std::to_string(minute));
    k.push_back('|');
    for (const auto& valve : open) k.push_back(valve ? '1' : '0');
    return k;
}

int64_t max_released(const dists& dists, const flow_rates& rates, int cur,
                     int minute, int max_minute, int64_t releasing,
                     std::vector<bool>& open,
                     std::unordered_map<std::string, int64_t>& memo) {
    if (minute > max_minute) return 0;
    std::string k = key(cur, minute, open);
    if (auto it = memo.find(k); it != memo.end()) return it->second;
    int64_t released = std::numeric_limits<int64_t>::min();
    for (int i = 0; i < dists.size(); i++) {
        int64_t dist = dists[cur][i];
        if (dist < 0) continue;
        if (open[i]) continue;
        if (minute + dist + 1 > max_minute) continue;
        // move there and open it
        open[i] = true;
        int64_t before = releasing * (dist + 1);
        int64_t after =
            max_released(dists, rates, i, minute + dist + 1, max_minute,
                         releasing + rates.at(i), open, memo);
        released = std::max(released, before + after);
        open[i] = false;
    }
    // what if we do nothing
    released = std::max(released, releasing * (max_minute - minute + 1));
    memo[k] = released;
    return released;
}

int64_t max_released(const dists& dists, const flow_rates& rates, int start) {
    std::vector<bool> open(rates.size(), false);
    std::unordered_map<std::string, int64_t> memo;
    return max_released(dists, rates, start, 1, 30, 0, open, memo);
}

int main(int argc, char* argv[]) {
    if (argc != 2) die("usage: day16 <file>");
    auto [flow_rates_map, connections_map] = parse(std::ifstream(argv[1]));
    auto edges = shortest_dists(flow_rates_map, connections_map);
    auto [flow_rates, dists, start] = prune(flow_rates_map, edges);

    /*
        for (const auto& [valve, rate] : flow_rates) {
            std::cout << valve << ": " << rate << std::endl;
        }
        std::cout << "valves:\n";
        for (const auto& [valve, valves] : connections) {
            std::cout << valve << ": ";
            for (const auto& valve : valves) std::cout << valve << ", ";
            std::cout << std::endl;
        }
        for (const auto& [from, valves] : dists) {
            for (const auto& [to, d] : valves) {
                std::cout << from << " -> " << to << ": " << d << std::endl;
            }
        }
    */

    std::cout << max_released(dists, flow_rates, start) << std::endl;
}
