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

struct explorer {
    int valve;
    int remaining;
    bool free;
};

std::string key(const explorer& e, int minute, const std::vector<bool>& open) {
    std::string k;
    k.append(std::to_string(e.valve));
    k.push_back('|');
    k.append(std::to_string(e.remaining));
    k.push_back('|');
    k.push_back(e.free ? '1' : '0');
    k.push_back('|');
    k.append(std::to_string(minute));
    k.push_back('|');
    for (const auto& valve : open) k.push_back(valve ? '1' : '0');
    return k;
}

int64_t releasing(const std::vector<bool>& open, const flow_rates& rates) {
    int64_t sum = 0;
    for (int i = 0; i < open.size(); i++) {
        if (open[i]) sum += rates[i];
    }
    return sum;
}

int64_t max_released(const dists& dists, const flow_rates& rates, explorer e,
                     int minute, int max_minute, std::vector<bool>& open,
                     std::unordered_map<std::string, int64_t>& memo) {
    if (minute > max_minute) return 0;
    std::string k = key(e, minute, open);
    if (auto it = memo.find(k); it != memo.end()) return it->second;
    bool flipped = false;
    if (!e.free && e.remaining == 0) {
        open[e.valve] = true;
        e.free = true;
        flipped = true;
    }
    int64_t per_tick = releasing(open, rates);
    int64_t released = std::numeric_limits<int64_t>::min();
    for (int i = 0; i < dists.size(); i++) {
        explorer next = e;
        if (e.free) {
            int64_t dist = dists[e.valve][i];
            if (dist < 0) continue;
            if (open[i]) continue;
            // start moving
            next.valve = i;
            next.remaining = dist + 1;
            next.free = false;
        }
        int go = dists[e.valve][i] + 1;  // 1
        if (minute + go > max_minute) continue;

        // move there and open it
        int64_t before = per_tick * go;
        next.remaining -= go;
        int64_t after = max_released(dists, rates, next, minute + go,
                                     max_minute, open, memo);
        released = std::max(released, before + after);
    }
    if (flipped) open[e.valve] = false;
    // what if we do nothing
    released = std::max(released, per_tick * (max_minute - minute + 1));
    memo[k] = released;
    return released;
}

int64_t max_released(const dists& dists, const flow_rates& rates, int start) {
    std::vector<bool> open(rates.size(), false);
    std::unordered_map<std::string, int64_t> memo;
    explorer e{.free = true, .remaining = 0, .valve = start};
    return max_released(dists, rates, e, 1, 30, open, memo);
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
    std::cout << max_released(dists, flow_rates, start) << std::endl;
}
