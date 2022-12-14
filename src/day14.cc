#include <charconv>
#include <fstream>
#include <iostream>
#include <iterator>
#include <limits>
#include <map>
#include <numeric>
#include <regex>
#include <string>
#include <utility>

#include "util.h"

enum class tile : int { empty = 0, rock = 1, sand = 2 };
using pt = std::pair<int, int>;
using path = std::vector<pt>;
using grid = std::map<pt, tile>;

char char_of(tile t) {
    switch (t) {
        case tile::empty: return '.';
        case tile::rock: return '#';
        case tile::sand: return 'o';
    }
}

template <typename match>
int int_match(match m) {
    int val;
    auto result = std::from_chars<int>(&*m.first, &*m.second, val);
    if (result.ec != std::errc()) die("bad int: " + m.str());
    return val;
}

path parse_path(const std::string& line) {
    path p;
    static const std::regex pat(R"(( -> )?(\d+),(\d+))");
    std::sregex_iterator begin(line.begin(), line.end(), pat), end;
    for (auto cur = begin; cur != end; ++cur) {
        int x = int_match((*cur)[2]), y = int_match((*cur)[3]);
        p.push_back({x, y});
    }
    return p;
}

void fill(grid& g, pt src, pt dst, tile t) {
    int dx = src.first < dst.first ? 1 : src.first > dst.first ? -1 : 0;
    int dy = src.second < dst.second ? 1 : src.second > dst.second ? -1 : 0;
    for (int x = src.first, y = src.second; pt{x, y} != dst; x += dx, y += dy) {
        g[{x, y}] = t;
    }
    g[dst] = t;
}

grid parse(std::istream&& is) {
    grid g;
    for (std::string line; std::getline(is, line);) {
        auto l = parse_path(line);
        for (int i = 1; i < l.size(); i++) {
            fill(g, l[i - 1], l[i], tile::rock);
        }
    }
    return g;
}

void print(grid& g) {
    int min_x = 500, max_x = 500, max_y = 0;
    for (const auto& [p, t] : g) {
        auto [x, y] = p;
        min_x = std::min(min_x, x);
        max_x = std::max(max_x, x);
        max_y = std::max(max_y, y);
    }
    char ch;
    for (int y = 0; y <= max_y; y++) {
        for (int x = min_x; x <= max_x; x++) {
            if (x == 500 && y == 0) ch = '+';
            else ch = char_of(g[{x, y}]);
            std::cout << ch;
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

int max_depth(const grid& g) {
    int max_y = 0;
    for (auto [p, t] : g) {
        if (t != tile::empty && p.second > max_y) max_y = p.second;
    }
    return max_y;
}

pt add_sand(grid& g) {
    pt p{500, 1};
    g[p] = tile::sand;
    return p;
}

pt apply_gravity(grid& g, pt src) {
    auto [x, y] = src;
    if (g[{x, y + 1}] == tile::empty) return {x, y + 1};
    if (g[{x - 1, y + 1}] == tile::empty) return {x - 1, y + 1};
    if (g[{x + 1, y + 1}] == tile::empty) return {x + 1, y + 1};
    return src;
}

int main(int argc, char* argv[]) {
    if (argc != 2) die("usage: day14 <file>");
    auto g = parse(std::ifstream(argv[1]));
    int depth = max_depth(g);
    int sand;
    for (sand = 0;; sand++) {
        pt p = add_sand(g), q = p;
        g[p] = tile::sand;
        while (p.second < depth) {
            q = apply_gravity(g, p);
            if (q == p) break;
            g[p] = tile::empty;
            g[q] = tile::sand;
            p = q;
        }
        if (p.second == depth) break;
    }
    std::cout << sand << std::endl;
}
