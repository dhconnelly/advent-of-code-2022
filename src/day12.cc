#include <deque>
#include <fstream>
#include <iostream>
#include <limits>
#include <set>
#include <tuple>
#include <utility>
#include <vector>

#include "util.h"

using pt2 = std::pair<int, int>;
using grid = std::vector<std::vector<int>>;
pt2 add(pt2 a, pt2 b) { return {a.first + b.first, a.second + b.second}; }
int at(const grid& g, pt2 pt) { return g[pt.first][pt.second]; }

bool in_bounds(const grid& g, pt2 pt) {
    return pt.first >= 0 && pt.first < g.size() && pt.second >= 0 &&
           pt.second < g[0].size();
}

int shortest_path(const grid& g, const std::vector<pt2>& start, pt2 end) {
    std::deque<std::pair<pt2, int>> q(start.size());
    for (const pt2& pt : start) q.push_back({pt, 0});
    std::set<pt2> v;
    for (const pt2& pt : start) v.insert(pt);
    while (!q.empty()) {
        auto [cur, dist] = q.front();
        int h = at(g, cur);
        q.pop_front();
        static constexpr pt2 kDirs[] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
        for (int i = 0; i < 4; i++) {
            pt2 nbr = add(cur, kDirs[i]);
            if (!in_bounds(g, nbr) || (at(g, nbr) - h > 1)) continue;
            if (nbr == end) return dist + 1;
            if (v.count(nbr)) continue;
            v.insert(nbr);
            q.push_back({nbr, dist + 1});
        }
    }
    return std::numeric_limits<int>::max();
}

std::vector<pt2> find_lowest(const grid& g) {
    std::vector<pt2> pts;
    for (int row = 0; row < g.size(); row++) {
        for (int col = 0; col < g[col].size(); col++) {
            if (g[row][col] == 0) pts.push_back({row, col});
        }
    }
    return pts;
}

std::tuple<grid, pt2, pt2> parse(std::istream&& is) {
    grid g;
    pt2 start, end;
    std::string line;
    for (int row = 0; std::getline(is, line); row++) {
        g.emplace_back();
        for (int col = 0; col < line.size(); col++) {
            if (char ch = line[col]; ch == 'S') {
                start = {row, col};
                g.back().push_back(0);
            } else if (ch == 'E') {
                end = {row, col};
                g.back().push_back('z' - 'a');
            } else {
                g.back().push_back(ch - 'a');
            }
        }
    }
    return {g, start, end};
}

int main(int argc, char* argv[]) {
    if (argc != 2) die("usage: day12 <file>");
    auto [g, start, end] = parse(std::ifstream(argv[1]));
    std::cout << shortest_path(g, {start}, end) << std::endl;
    std::cout << shortest_path(g, find_lowest(g), end) << std::endl;
}
