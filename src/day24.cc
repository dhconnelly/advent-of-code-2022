#include <algorithm>
#include <fstream>
#include <iostream>
#include <set>
#include <string>
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

void print(const grid& g) {
    for (int row = 0; row < g.size(); row++) {
        for (int col = 0; col < g[row].size(); col++) {
            const auto& at_tile = g[row][col];
            if (at_tile.size() == 0) std::cout << '.';
            else if (at_tile.size() == 1) std::cout << at_tile[0];
            else std::cout << at_tile.size();
        }
        std::cout << std::endl;
    }
}

void print(pt p) { std::cout << '[' << p.first << ',' << p.second << ']'; }

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

int main(int argc, char* argv[]) {
    if (argc != 2) die("usage: day24 <file>");
    auto g = parse(std::ifstream(argv[1]));
    print(g);
    auto [src, dst] = find_entrance_exit(g);
    print(src);
    print(dst);
    std::cout << std::endl;
}
