#include <algorithm>
#include <deque>
#include <fstream>
#include <iostream>
#include <set>
#include <sstream>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

#include "util.h"

using tile = char;
using tile_members = std::vector<tile>;
using grid = std::vector<std::vector<tile_members>>;
using pt = std::pair<int, int>;

grid parse(std::istream&& is) {
    grid g;
    std::string line;
    for (int row = 0; std::getline(is, line); row++) {
        g.emplace_back();
        for (int col = 0; col < line.size(); col++) {
            g.back().emplace_back();
            if (line[col] != '.') g.back().back().push_back(line[col]);
        }
    }
    return g;
}

void print(const grid& g, std::ostream& os = std::cout) {
    for (int row = 0; row < g.size(); row++) {
        for (int col = 0; col < g[row].size(); col++) {
            const auto& at_tile = g[row][col];
            if (at_tile.size() == 0) os << '.';
            else if (at_tile.size() == 1) os << at_tile[0];
            else os << at_tile.size();
        }
        os << std::endl;
    }
}

void print(pt p, std::ostream& os = std::cout) {
    os << '[' << p.first << ',' << p.second << ']';
}

std::pair<pt, pt> find_entrance_exit(const grid& g) {
    auto find_empty = [](const auto& c) {
        auto it = std::find_if(c.begin(), c.end(), [](const auto& at_tile) {
            return at_tile.empty();
        });
        if (it == c.end()) die("no empty tile found");
        return it;
    };
    size_t entrance_col = find_empty(g.front()) - g.front().begin();
    size_t exit_col = find_empty(g.back()) - g.back().begin();
    return {{0, entrance_col}, {g.size() - 1, exit_col}};
}

// TODO: only actually need to keep track of the iteration modulo cycle length
using key = std::string;
key k(const grid& g) {
    std::stringstream s;
    print(g, s);
    return s.str();
}

pt add(pt a, pt b) { return {a.first + b.first, a.second + b.second}; }

tile_members& at(grid& g, pt p) { return g[p.first][p.second]; }

enum dir { kDirsUp = 0, kDirsDown = 1, kDirsLeft = 2, kDirsRight = 3 };
constexpr pt kDirs[] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}, {0, 0}};
pt dir_for(char x) {
    switch (x) {
        case '<': return kDirs[kDirsLeft];
        case '^': return kDirs[kDirsUp];
        case '>': return kDirs[kDirsRight];
        case 'v': return kDirs[kDirsDown];
    }
    die("bad dir");
}

pt wrap(const grid& g, pt p) {
    if (p.first == 0) p.first = g.size() - 2;
    if (p.first == g.size() - 1) p.first = 1;
    if (p.second == 0) p.second = g.back().size() - 2;
    if (p.second == g.back().size() - 1) p.second = 1;
    return p;
}

void step(grid& g, grid& g_next) {
    for (int row = 0; row < g.size(); row++) {
        for (int col = 0; col < g[row].size(); col++) {
            auto& cur = at(g_next, {row, col});
            if (cur.empty() || (cur.size() == 1 && cur[0] == '#')) continue;
            cur.clear();
        }
    }
    for (int row = 0; row < g.size(); row++) {
        for (int col = 0; col < g[row].size(); col++) {
            pt p{row, col};
            for (auto storm : at(g, p)) {
                if (storm == '#') break;
                pt q = wrap(g, add(p, dir_for(storm)));
                at(g_next, q).push_back(storm);
            }
        }
    }
}

bool in_bounds(const grid& g, pt p) {
    return p.first >= 0 && p.first < g.size() && p.second >= 0 &&
           p.second < g[p.first].size();
}

int shortest_path(std::vector<grid>& grids, pt src, pt dst) {
    std::set<std::pair<pt, int>> v;
    v.emplace(src, 0);
    std::deque<std::pair<pt, int>> q;
    q.emplace_back(src, 0);
    while (!q.empty()) {
        auto [cur, d] = q.front();
        auto& g = grids[d % grids.size()];
        auto& g_next = grids[(d + 1) % grids.size()];
        q.pop_front();
        for (int i = 0; i < 5; i++) {
            pt nbr = add(cur, kDirs[i]);
            if (!in_bounds(g, nbr) || !at(g_next, nbr).empty()) continue;
            if (nbr == dst) return d + 1;
            int cycle = (d + 1) % grids.size();
            if (v.count({nbr, cycle})) continue;
            v.insert({nbr, cycle});
            q.emplace_back(nbr, d + 1);
        }
    }
    die("no path");
}

std::vector<grid> cycles(grid g) {
    std::vector<grid> cycles;
    cycles.push_back(g);
    grid g2 = g;
    int i = 0;
    std::set<std::string> v;
    v.insert(k(g));
    for (;; i++) {
        step(g, g2);
        g = g2;
        auto key = k(g);
        if (v.count(key)) break;
        v.insert(key);
        cycles.push_back(g);
    }
    return cycles;
}

int main(int argc, char* argv[]) {
    if (argc != 2) die("usage: day24 <file>");
    auto g = parse(std::ifstream(argv[1]));
    auto [src, dst] = find_entrance_exit(g);
    auto grids = cycles(g);
    std::cout << shortest_path(grids, src, dst) << std::endl;
}
