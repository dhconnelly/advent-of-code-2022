#include <algorithm>
#include <cinttypes>
#include <concepts>
#include <fstream>
#include <functional>
#include <iostream>
#include <iterator>
#include <limits>
#include <numeric>
#include <optional>
#include <set>
#include <tuple>
#include <vector>

#include "util.h"

struct pt3 {
    int64_t x, y, z;
};
std::istream& operator>>(std::istream& is, pt3& pt) {
    char comma;
    return is >> pt.x >> comma >> pt.y >> comma >> pt.z;
}
std::ostream& operator<<(std::ostream& os, const pt3& pt) {
    return os << '[' << pt.x << ',' << pt.y << ',' << pt.z << ']';
}
pt3 operator+(pt3 a, pt3 b) { return {a.x + b.x, a.y + b.y, a.z + b.z}; }
bool operator<(pt3 a, pt3 b) {
    using tuple = std::tuple<int64_t, int64_t, int64_t>;
    return tuple(a.x, a.y, a.z) < tuple(b.x, b.y, b.z);
}

// you should see the template and C++20 concepts goo i tried to write here
pt3 upper_bound(const std::vector<pt3>& pts) {
    static constexpr int64_t kMin = std::numeric_limits<int64_t>::min();
    pt3 ub{kMin, kMin, kMin};
    for (pt3 pt : pts) {
        ub.x = std::max(ub.x, pt.x);
        ub.y = std::max(ub.y, pt.y);
        ub.z = std::max(ub.z, pt.z);
    }
    return ub;
}

using grid = std::vector<std::vector<std::vector<bool>>>;

grid empty(size_t dx, size_t dy, size_t dz) {
    return std::vector(dx, std::vector(dy, std::vector<bool>(dz, false)));
}

grid grid_of(const std::vector<pt3>& pts) {
    auto [x, y, z] = upper_bound(pts);
    grid g = empty(x + 1, y + 1, z + 1);
    for (pt3 pt : pts) g[pt.x][pt.y][pt.z] = true;
    return g;
}

bool in_bounds(const grid& g, pt3 pt) {
    if (pt.x < 0 || pt.y < 0 || pt.z < 0) return false;
    if (pt.x >= g.size() || pt.y >= g[0].size() || pt.z >= g[0][0].size())
        return false;
    return true;
}

bool has(const grid& g, pt3 pt) {
    return in_bounds(g, pt) && g[pt.x][pt.y][pt.z];
}

static constexpr pt3 kDirs[] = {
    {-1, 0, 0}, {1, 0, 0}, {0, -1, 0}, {0, 1, 0}, {0, 0, -1}, {0, 0, 1},
};

int64_t surface_area(pt3 pt, std::function<bool(pt3)> count_nbr) {
    int64_t area = 6;
    for (int dir = 0; dir < 6; dir++) {
        pt3 nbr = pt + kDirs[dir];
        if (!count_nbr(nbr)) area -= 1;
    }
    return area;
}

int64_t droplet_area(const grid& g, std::function<bool(pt3)> count_nbr) {
    int64_t sum = 0;
    for (int64_t x = 0; x < g.size(); x++) {
        for (int64_t y = 0; y < g[0].size(); y++) {
            for (int64_t z = 0; z < g[0][0].size(); z++) {
                pt3 pt{x, y, z};
                if (has(g, pt)) sum += surface_area(pt, count_nbr);
            }
        }
    }
    return sum;
}

void explore_exterior(grid& exterior, const grid& g, std::set<pt3>& v, pt3 pt) {
    v.insert(pt);
    exterior[pt.x][pt.y][pt.z] = true;
    for (int dir = 0; dir < 6; dir++) {
        pt3 nbr = pt + kDirs[dir];
        if (v.count(nbr) || !in_bounds(exterior, nbr) || has(g, nbr)) continue;
        explore_exterior(exterior, g, v, nbr);
    }
}

grid exterior_of(const grid& g, const std::vector<pt3>& pts) {
    pt3 ub = upper_bound(pts);
    grid exterior = empty(ub.x + 2, ub.y + 2, ub.z + 2);
    std::set<pt3> v;
    explore_exterior(exterior, g, v, ub + pt3{1, 1, 1});
    return exterior;
}

int main(int argc, char* argv[]) {
    if (argc != 2) die("usage: day18 <file>");
    std::ifstream ifs(argv[1]);
    std::istream_iterator<pt3> begin(ifs), end;
    std::vector<pt3> pts(begin, end);

    grid droplet = grid_of(pts);
    std::cout << droplet_area(droplet, [&](pt3 pt) {
        return !has(droplet, pt);
    }) << std::endl;

    grid exterior = exterior_of(droplet, pts);
    std::cout << droplet_area(droplet, [&](pt3 pt) {
        return has(exterior, pt) || !in_bounds(droplet, pt);
    }) << std::endl;
}
