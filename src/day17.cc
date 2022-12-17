#include <algorithm>
#include <array>
#include <fstream>
#include <iostream>
#include <optional>
#include <string>
#include <vector>

#include "util.h"

using shape = std::vector<std::string>;

static const shape kShapes[] = {
    {"####"},
    {".#.", "###", ".#."},
    {"..#", "..#", "###"},
    {"#", "#", "#", "#"},
    {"##", "##"},
};

using pt = std::pair<int64_t, int64_t>;  // row, col
static constexpr pt kOblivion{-999, -999};

struct entity {
    int shape;
    pt pos;
};

using chamber = std::vector<entity>;

std::optional<char> at(const entity& e, pt pos) {
    int64_t row = e.pos.first - pos.first;
    int64_t col = pos.second - e.pos.second;
    const auto& shape = kShapes[e.shape];
    if (row < 0 || row >= shape.size()) return {};
    if (col < 0 || col >= shape[0].size()) return {};
    return shape[row][col];
}

char at(const chamber& c, pt pos) {
    for (const auto& e : c)
        if (auto ch = at(e, pos); ch.has_value() && (*ch) != '.') return *ch;
    return '.';
}

int64_t max_row(const chamber& c) {
    auto cmp = [](entity e1, entity e2) { return e1.pos.first < e2.pos.first; };
    auto it = std::max_element(c.begin(), c.end(), cmp);
    return it == c.end() ? 0 : it->pos.first + 1;
}

void print(const chamber& c) {
    for (int64_t row = max_row(c); row >= 0; row--) {
        std::cout << '|';
        for (int64_t col = 0; col < 7; col++) std::cout << at(c, {row, col});
        std::cout << '|';
        std::cout << std::endl;
    }
    std::cout << "+-------+" << std::endl;
}

void add_shape(chamber& c, int i) {
    pt pos{max_row(c) + kShapes[i].size() + 2, 2};
    c.push_back(entity{.shape = i, .pos = pos});
}

pt shift(pt pos, char dir) {
    switch (dir) {
        case '>': pos.second++; return pos;
        case '<': pos.second--; return pos;
        case 'v': pos.first--; return pos;
    }
    die("bad dir: " + std::to_string(dir));
}

bool fits(const chamber& c, const entity& e) {
    const auto& shape = kShapes[e.shape];
    for (int64_t row = 0; row < shape.size(); row++) {
        for (int64_t col = 0; col < shape[0].size(); col++) {
            pt pos{e.pos.first - row, e.pos.second + col};
            if (shape[row][col] != '.' && (at(c, pos) != '.' || pos.first < 0 ||
                                           pos.second < 0 || pos.second >= 7)) {
                // collision
                return false;
            }
        }
    }
    return true;
}

bool apply(chamber& c, int ei, char dir) {
    pt orig = c[ei].pos;
    c[ei].pos = kOblivion;
    pt shifted = shift(orig, dir);
    if (fits(c, {.shape = c[ei].shape, .pos = shifted})) {
        c[ei].pos = shifted;
        return true;
    } else {
        c[ei].pos = orig;
        return false;
    }
}

std::string parse(std::istream&& is) {
    std::string dirs;
    return eatline(is, dirs);
}

int main(int argc, char* argv[]) {
    if (argc != 2) die("usage: day17 <file>");
    auto dirs = parse(std::ifstream(argv[1]));
    chamber c;
    int dir = 0;
    for (int shape = 0; shape < 2022; shape++) {
        add_shape(c, shape % 5);
        while (true) {
            apply(c, c.size() - 1, dirs[dir++ % dirs.size()]);
            if (!apply(c, c.size() - 1, 'v')) break;
        }
    }
    std::cout << max_row(c) << std::endl;
}
