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
std::map<std::string, std::vector<edge>> shortest_dists(
    const flow_rates& rates, const connections& conns) {
    std::map<std::string, std::vector<edge>> dists;
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

int main(int argc, char* argv[]) {
    if (argc != 2) die("usage: day16 <file>");
    auto [flow_rates, connections] = parse(std::ifstream(argv[1]));
    for (const auto& [valve, rate] : flow_rates) {
        std::cout << valve << ": " << rate << std::endl;
    }
    std::cout << "valves:\n";
    for (const auto& [valve, valves] : connections) {
        std::cout << valve << ": ";
        for (const auto& valve : valves) std::cout << valve << ", ";
        std::cout << std::endl;
    }
    auto dists = shortest_dists(flow_rates, connections);
    for (const auto& [from, valves] : dists) {
        for (const auto& [to, d] : valves) {
            std::cout << from << " -> " << to << ": " << d << std::endl;
        }
    }
}
