#include <fstream>
#include <iostream>
#include <iterator>
#include <numeric>
#include <set>
#include <utility>
#include <vector>

#include "util.h"

using pt2 = std::pair<int, int>;

bool touching(pt2 a, pt2 b) {
    int dx = std::abs(a.first - b.first), dy = std::abs(a.second - b.second);
    return dx <= 1 && dy <= 1;
}

bool in_line(pt2 a, pt2 b) {
    return a.first == b.first || a.second == b.second;
}

int step_dist(pt2 a, pt2 b) {
    return std::abs(a.first - b.first) + std::abs(a.second - b.second);
}

pt2 unit_vec(pt2 from, pt2 to) {
    pt2 vec{to.first - from.first, to.second - from.second};
    if (vec.first != 0 && vec.second != 0) die("unreachable: bad unit vec");
    if (vec.first != 0) vec.first = vec.first / std::abs(vec.first);
    if (vec.second != 0) vec.second = vec.second / std::abs(vec.second);
    return vec;
}

pt2 add(pt2 a, pt2 b) { return {a.first + b.first, a.second + b.second}; }

pt2 move_knot(pt2 cur, pt2 to) {
    if (touching(cur, to)) {
        return cur;
    } else if (in_line(cur, to) && step_dist(cur, to) == 2) {
        return add(cur, unit_vec(cur, to));
    } else {
        static constexpr pt2 kDiags[] = {{1, 1}, {1, -1}, {-1, 1}, {-1, -1}};
        for (int i = 0; i < 4; i++) {
            pt2 nbr = add(cur, kDiags[i]);
            if (touching(nbr, to)) return nbr;
        }
    }
    die("unreachable: bad tail movement");
}

pt2 move_head(pt2 cur_head, char dir) {
    switch (dir) {
    case 'U': cur_head.first++; break;
    case 'R': cur_head.second++; break;
    case 'D': cur_head.first--; break;
    case 'L': cur_head.second--; break;
    default: die("unreachable: bad direction");
    }
    return cur_head;
}

struct instr : std::pair<char, int> {
    using std::pair<char, int>::pair;
};

std::istream& operator>>(std::istream& is, instr& i) {
    if (!(is >> i.first)) return is;
    return is >> i.second;
}

std::vector<instr> parse_instrs(std::istream&& is) {
    return std::vector((std::istream_iterator<instr>(is)),
                       (std::istream_iterator<instr>()));
}

std::set<pt2> move_rope(const std::vector<instr>& instrs, int len) {
    std::vector<pt2> knots(len);
    std::set<pt2> v;
    v.insert(knots.back());
    for (const auto& [dir, dist] : instrs) {
        for (int i = 0; i < dist; i++) {
            knots[0] = move_head(knots[0], dir);
            for (int j = 1; j < len; j++) {
                knots[j] = move_knot(knots[j], knots[j - 1]);
            }
            v.insert(knots.back());
        }
    }
    return v;
}

int main(int argc, char* argv[]) {
    if (argc != 2) die("usage: day9 <file>");
    auto instrs = parse_instrs(std::ifstream(argv[1]));
    std::cout << move_rope(instrs, 2).size() << std::endl;
    std::cout << move_rope(instrs, 10).size() << std::endl;
}
