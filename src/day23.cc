#include <array>
#include <fstream>
#include <iostream>
#include <limits>
#include <map>
#include <optional>
#include <set>
#include <sstream>
#include <utility>

#include "util.h"

struct pt {
    int row, col;
};
bool operator<(pt a, pt b) {
    return std::pair{a.row, a.col} < std::pair{b.row, b.col};
}
std::ostream& operator<<(std::ostream& os, pt p) {
    return os << '[' << p.row << ',' << p.col << ']';
}
pt add(pt a, pt b) { return {a.row + b.row, a.col + b.col}; }

bool any_adjacent(const std::set<pt>& elves, pt p) {
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            if (i == 0 && j == 0) continue;
            if (elves.count({p.row + i, p.col + j})) return true;
        }
    }
    return false;
}

struct decision {
    std::array<pt, 3> must_be_empty;
    pt step;
};
constexpr decision kDecisions[] = {
    {std::array{pt{-1, 0}, pt{-1, 1}, pt{-1, -1}}, {-1, 0}},
    {std::array{pt{1, 0}, pt{1, 1}, pt{1, -1}}, {1, 0}},
    {std::array{pt{0, -1}, pt{-1, -1}, pt{1, -1}}, {0, -1}},
    {std::array{pt{0, 1}, pt{-1, 1}, pt{1, 1}}, {0, 1}},
};

bool satisfies(const std::set<pt>& elves, pt elf, const decision& d) {
    for (pt dir : d.must_be_empty) {
        pt nbr = add(elf, dir);
        if (elves.count(nbr)) return false;
    }
    return true;
}

std::optional<pt> propose(const std::set<pt>& elves, pt elf, int i) {
    if (!any_adjacent(elves, elf)) return {};
    for (int j = 0; j < 4; j++) {
        int k = (i + j) % 4;
        const auto& decision = kDecisions[k];
        if (satisfies(elves, elf, decision)) {
            return add(elf, decision.step);
        }
    }
    return {};
}

std::map<pt, std::vector<pt>> propose(const std::set<pt>& elves, int i) {
    std::map<pt, std::vector<pt>> all;
    for (pt elf : elves) {
        if (auto pt = propose(elves, elf, i); pt) all[*pt].push_back(elf);
    }
    return all;
}

bool tick(std::set<pt>& elves, int i) {
    auto proposals = propose(elves, i);
    bool moved = false;
    for (auto [dest, candidates] : proposals) {
        if (candidates.size() == 1) {
            elves.erase(candidates[0]);
            elves.insert(dest);
            moved = true;
        }
    }
    return moved;
}

std::set<pt> parse(std::istream&& is) {
    std::set<pt> elves;
    std::string line;
    for (int row = 0; std::getline(is, line); row++) {
        for (int col = 0; col < line.size(); col++) {
            if (line[col] == '#') elves.insert({.row = row, .col = col});
        }
    }
    return elves;
}

constexpr int kMinInt = std::numeric_limits<int>::min();
constexpr int kMaxInt = std::numeric_limits<int>::max();

std::pair<pt, pt> bounds(const std::set<pt>& elves) {
    int min_row = kMaxInt, max_row = kMinInt;
    int min_col = kMaxInt, max_col = kMinInt;
    for (pt p : elves) {
        min_row = std::min(min_row, p.row), max_row = std::max(max_row, p.row);
        min_col = std::min(min_col, p.col), max_col = std::max(max_col, p.col);
    }
    return {{min_row, min_col}, {max_row, max_col}};
}

void print(const std::set<pt>& elves) {
    auto [min, max] = bounds(elves);
    std::cout << "======================\n";
    for (int row = min.row - 1; row <= max.row + 1; row++) {
        for (int col = min.col - 1; col <= max.col + 1; col++) {
            if (elves.count({row, col})) std::cout << '#';
            else std::cout << '.';
        }
        std::cout << std::endl;
    }
}

int size(const std::pair<pt, pt>& bounds) {
    return (bounds.second.row - bounds.first.row + 1) *
           (bounds.second.col - bounds.first.col + 1);
}

int main(int argc, char* argv[]) {
    if (argc != 2) die("usage: day23 <file>");
    std::set<pt> elves = parse(std::ifstream(argv[1]));

    std::set<pt> elves1 = elves;
    for (int i = 0; i < 10; i++) tick(elves1, i);
    std::cout << (size(bounds(elves1)) - elves1.size()) << std::endl;

    std::set<pt> elves2 = elves;
    int i = 0;
    while (tick(elves2, i++))
        ;
    std::cout << i << std::endl;
}
