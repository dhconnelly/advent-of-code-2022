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
using grid = std::map<pt, tile>;

char char_of(tile t) {
    switch (t) {
        case tile::empty: return '.';
        case tile::rock: return '#';
        case tile::sand: return 'o';
    }
}

std::vector<pt> parse_path(const std::string& line) {
    std::vector<pt> p;
    static const std::regex pat(R"(( -> )?(\d+),(\d+))");
    for (std::sregex_iterator cur(line.begin(), line.end(), pat), end;
         cur != end; ++cur) {
        p.push_back({int_match(*cur, 2), int_match(*cur, 3)});
    }
    return p;
}

void fill(grid& g, pt src, pt dst, tile t) {
    int dx = src.first < dst.first ? 1 : src.first > dst.first ? -1 : 0;
    int dy = src.second < dst.second ? 1 : src.second > dst.second ? -1 : 0;
    for (pt p = src; p != dst; p.first += dx, p.second += dy) g[p] = t;
    g[dst] = t;
}

grid parse(std::istream&& is) {
    grid g;
    for (std::string line; std::getline(is, line);) {
        auto l = parse_path(line);
        for (int i = 1; i < l.size(); i++) fill(g, l[i - 1], l[i], tile::rock);
    }
    return g;
}

int max_depth(const grid& g) {
    return std::accumulate(g.begin(), g.end(), 0, [](int max, auto& e) {
        auto [p, t] = e;
        return t != tile::empty && p.second > max ? p.second : max;
    });
}

pt apply_gravity(grid& g, pt src) {
    if (auto [x, y] = src; g[{x, y + 1}] == tile::empty) return {x, y + 1};
    else if (g[{x - 1, y + 1}] == tile::empty) return {x - 1, y + 1};
    else if (g[{x + 1, y + 1}] == tile::empty) return {x + 1, y + 1};
    else return src;
}

static constexpr pt kStart{500, 0};

int drop_until(grid g, int max_y, std::function<bool(pt)> done) {
    int sand = 0;
    for (pt p, q; sand == 0 || !done(p); sand++) {
        g[p = kStart] = tile::sand;
        while ((q = apply_gravity(g, p)) != p && p.second < max_y) {
            std::swap(g[p], g[q]);
            p = q;
        }
    }
    return sand;
}

int main(int argc, char* argv[]) {
    if (argc != 2) die("usage: day14 <file>");
    auto g = parse(std::ifstream(argv[1]));

    int d = max_depth(g);
    int until_freefall = drop_until(g, d, [=](pt p) { return p.second == d; });
    std::cout << (until_freefall - 1) << std::endl;

    int d2 = d + 1;
    int until_filled = drop_until(g, d2, [](pt p) { return p == kStart; });
    std::cout << until_filled << std::endl;
}
