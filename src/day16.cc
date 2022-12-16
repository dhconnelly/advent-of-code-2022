#include <deque>
#include <fstream>
#include <iostream>
#include <map>
#include <regex>
#include <set>
#include <string>
#include <vector>

#include "util.h"

using connections = std::map<std::string, std::vector<std::string>>;
using flow_rates = std::map<std::string, int64_t>;

std::pair<flow_rates, connections> parse(std::istream&& is) {
    flow_rates rates;
    connections conns;
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
edges shortest_dists(const flow_rates& rates, const connections& conns) {
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

std::string key(const std::string& cur, int minute,
                std::set<std::string>& open) {
    std::string k(cur);
    k.push_back('|');
    k.append(std::to_string(minute));
    k.push_back('|');
    for (const auto& valve : open) {
        k.append(valve);
        k.push_back(',');
    }
    return k;
}

int64_t max_released(const edges& edges, const flow_rates& rates,
                     const std::string& cur, int minute, int max_minute,
                     int64_t releasing, std::set<std::string>& open,
                     std::map<std::string, int64_t>& memo) {
    if (minute > max_minute) return 0;
    std::string k = key(cur, minute, open);
    if (auto it = memo.find(k); it != memo.end()) return it->second;
    int64_t released = std::numeric_limits<int64_t>::min();
    // std::cout << "minute " << minute << ", at " << cur << " releasing "
    //<< releasing << std::endl;
    for (const auto& [valve, dist] : edges.at(cur)) {
        if (open.count(valve)) continue;
        if (minute + dist + 1 > max_minute) continue;
        // move there and open it
        // std::cout << "opening " << valve << std::endl;
        open.insert(valve);
        int64_t before = releasing * (dist + 1);
        int64_t after =
            max_released(edges, rates, valve, minute + dist + 1, max_minute,
                         releasing + rates.at(valve), open, memo);
        released = std::max(released, before + after);
        open.erase(valve);
    }
    // what if we do nothing
    released = std::max(released, releasing * (max_minute - minute + 1));
    memo[k] = released;
    return released;
}

int64_t max_released(const edges& edges, const flow_rates& rates) {
    std::set<std::string> open;
    std::map<std::string, int64_t> memo;
    return max_released(edges, rates, "AA", 1, 30, 0, open, memo);
}

int main(int argc, char* argv[]) {
    if (argc != 2) die("usage: day16 <file>");
    auto [flow_rates, connections] = parse(std::ifstream(argv[1]));
    auto dists = shortest_dists(flow_rates, connections);

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

    std::cout << max_released(dists, flow_rates) << std::endl;
}
