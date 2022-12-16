#include <deque>
#include <fstream>
#include <iostream>
#include <regex>
#include <set>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>

#include "util.h"

using connections = std::vector<std::vector<bool>>;
using flow_rates = std::vector<int64_t>;

std::tuple<flow_rates, connections, int> parse(std::istream&& is) {
    std::unordered_map<std::string, std::vector<std::string>> conns;
    std::unordered_map<std::string, int64_t> rates;
    std::unordered_map<std::string, int> idx;
    for (std::string line; std::getline(is, line);) {
        static const std::regex pat(
            R"(Valve (\w+) has flow rate=(\d+); tunnels? leads? to valves? (.*)$)");
        std::smatch m;
        if (!std::regex_match(line, m, pat)) die("bad line: " + line);
        auto valve = m[1].str();
        idx[valve] = idx.size();
        rates[valve] = std::stoi(m[2].str());
        auto valves = m[3].str();
        static const std::regex valve_pat(R"((, )?(\w+))");
        for (std::sregex_iterator it(valves.begin(), valves.end(), valve_pat),
             end;
             it != end; it++) {
            conns[valve].push_back((*it)[2].str());
        }
    }
    connections connsi(conns.size(), std::vector<bool>(conns.size(), false));
    flow_rates ratesi(rates.size());
    for (const auto& [k, vs] : conns) {
        for (const auto& v : vs) connsi[idx[k]][idx[v]] = true;
    }
    for (const auto& [k, r] : rates) ratesi[idx[k]] = r;
    return {ratesi, connsi, idx["AA"]};
}

using dists = std::vector<std::vector<int>>;
dists shortest_dists(const flow_rates& rates, const connections& conns) {
    dists dists(rates.size(), std::vector<int>(rates.size(), -1));
    for (int i = 0; i < conns.size(); i++) {
        auto& edges = dists[i];
        dists[i][i] = 0;
        std::deque<std::pair<int, int>> q;
        q.emplace_back(i, 0);
        std::vector<bool> v(rates.size(), false);
        v[i] = true;
        while (!q.empty()) {
            auto [cur, d] = q.front();
            q.pop_front();
            for (int j = 0; j < conns.size(); j++) {
                if (!conns[cur][j]) continue;
                if (v[j]) continue;
                v[j] = true;
                edges[j] = d + 1;
                q.emplace_back(j, d + 1);
            }
        }
    }
    return dists;
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

int64_t max_released(const dists& edges, const flow_rates& rates, int cur,
                     int minute, int max_minute, int64_t releasing,
                     std::vector<bool>& open,
                     std::unordered_map<std::string, int64_t>& memo) {
    if (minute > max_minute) return 0;
    std::string k = key(cur, minute, open);
    if (auto it = memo.find(k); it != memo.end()) return it->second;
    int64_t released = std::numeric_limits<int64_t>::min();
    for (int i = 0; i < rates.size(); i++) {
        int64_t dist = edges[cur][i];
        if (dist < 0) continue;
        if (open[i]) continue;
        if (minute + dist + 1 > max_minute) continue;
        // move there and open it
        open[i] = true;
        int64_t before = releasing * (dist + 1);
        int64_t after =
            max_released(edges, rates, i, minute + dist + 1, max_minute,
                         releasing + rates.at(i), open, memo);
        released = std::max(released, before + after);
        open[i] = false;
    }
    // what if we do nothing
    released = std::max(released, releasing * (max_minute - minute + 1));
    memo[k] = released;
    return released;
}

int64_t max_released(const dists& edges, const flow_rates& rates, int start) {
    std::vector<bool> open(rates.size(), false);
    std::unordered_map<std::string, int64_t> memo;
    return max_released(edges, rates, start, 1, 30, 0, open, memo);
}

int main(int argc, char* argv[]) {
    if (argc != 2) die("usage: day16 <file>");
    auto [flow_rates, connections, start] = parse(std::ifstream(argv[1]));
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

    std::cout << max_released(dists, flow_rates, start) << std::endl;
}
