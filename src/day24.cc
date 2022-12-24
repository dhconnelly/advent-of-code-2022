#include <fstream>
#include <iostream>

#include "util.h"

using tile = char;
using tile_members = std::vector<tile>;
using grid = std::vector<std::vector<tile_members>>;

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

int main(int argc, char* argv[]) {
    if (argc != 2) die("usage: day24 <file>");
    auto g = parse(std::ifstream(argv[1]));
    print(g);
}
