#include <algorithm>
#include <array>
#include <deque>
#include <fstream>
#include <iostream>
#include <optional>
#include <set>
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

std::ostream& operator<<(std::ostream& os, const entity& e) {
    return os << "[e=" << e.shape << ", p=" << e.pos.first << ','
              << e.pos.second << "]";
}

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

std::string row_at(const chamber& c, int row) {
    std::string s(9, 0);
    s[0] = s[8] = '|';
    for (int64_t col = 0; col < 7; col++) s[col + 1] = at(c, {row, col});
    return s;
}

void print(const chamber& c) {
    for (int64_t row = max_row(c); row >= 0; row--) {
        printf("%3lld", row);
        std::cout << row_at(c, row) << std::endl;
    }
    std::cout << "   +-------+" << std::endl;
}

pt insertion_point(const chamber& c, int shape) {
    return {max_row(c) + kShapes[shape].size() + 2, 2};
}

void add_shape(chamber& c, int shape, pt pos) {
    c.push_back(entity{.shape = shape, .pos = pos});
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

bool blocked(const chamber& c, int64_t row) {
    for (int col = 0; col < 7; col++) {
        if (at(c, {row, col}) == '.') return false;
    }
    return true;
}

pt add(pt a, pt b) { return {a.first + b.first, a.second + b.second}; }

int min_reachable_row(chamber& c, int max, pt from, std::set<pt>& v) {
    int min = from.first;
    static constexpr pt kDirs[] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
    for (int i = 0; i < 4; i++) {
        pt nbr = add(from, kDirs[i]);
        if (nbr.first < 0 || nbr.first > max) continue;
        if (nbr.second < 0 || nbr.second >= 7) continue;
        if (at(c, nbr) == '#' || v.count(nbr)) continue;
        v.insert(nbr);
        min = std::min(min, min_reachable_row(c, max, nbr, v));
    }
    return min;
}

int min_reachable_row(chamber& c, int max, pt from) {
    std::set<pt> v;
    return min_reachable_row(c, max, from, v);
}

void prune(chamber& c, pt from) {
    int min_row = min_reachable_row(c, from.first, from);
    if (min_row == 0) return;
    int pruned =
        std::erase_if(c, [=](auto e) { return e.pos.first < min_row; });
    if (pruned > 0) {
        for (auto& e : c) e.pos.first -= min_row;
    }
}

int height_when(const std::string& dirs,
                std::function<bool(int, const chamber&)> done) {
    chamber c;
    int height = 0;
    int n, dir;
    for (n = 0, dir = 0; !done(n, c); n++) {
        int shape = n % 5;
        int height_before = max_row(c);
        prune(c, {height_before + 1, 0});
        height += height_before - max_row(c);
        add_shape(c, shape, insertion_point(c, shape));
        while (true) {
            apply(c, c.size() - 1, dirs[dir++ % dirs.size()]);
            if (!apply(c, c.size() - 1, 'v')) break;
        }
    }
    return height + max_row(c);
}

int main(int argc, char* argv[]) {
    if (argc != 2) die("usage: day17 <file>");
    auto dirs = parse(std::ifstream(argv[1]));
    std::cout << height_when(dirs, [](int n, const chamber& c) {
        return n >= 2022;
    }) << std::endl;
    /*
        c = chamber{};
        height = 0;
        for (shape = 0, dir = 0; shape < 5000; shape++) {
            auto pt = insertion_point(c, shape % 5);
            height += prune(c, pt);
            if (shape > 0 && c.empty()) break;
            add_shape(c, shape % 5, pt);
            while (true) {
                apply(c, c.size() - 1, dirs[dir++ % dirs.size()]);
                if (!apply(c, c.size() - 1, 'v')) break;
            }
        }
        int n = shape + 1;
        std::cout << "purged to empty with height " << height << " after " <<
       shape
                  << std::endl;

        c = chamber{};
        for (shape = 0, dir = 0; shape <= n; shape++) {
            auto pt = insertion_point(c, shape % 5);
            add_shape(c, shape % 5, pt);
            while (true) {
                apply(c, c.size() - 1, dirs[dir++ % dirs.size()]);
                if (!apply(c, c.size() - 1, 'v')) break;
            }
        }
        print(c);
    */
}
