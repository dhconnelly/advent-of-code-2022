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

// you should see the template and C++20 concepts goo i tried to write here
pt3 upper_bound(const std::vector<pt3>& pts) {
    static constexpr int64_t kMin = std::numeric_limits<int64_t>::min();
    pt3 ub{kMin, kMin, kMin};
    for (pt3 pt : pts) {
        ub.x = std::max(ub.x, pt.x);
        ub.y = std::max(ub.x, pt.y);
        ub.z = std::max(ub.x, pt.z);
    }
    return ub;
}

using grid = std::vector<std::vector<std::vector<bool>>>;
grid grid_of(const std::vector<pt3>& pts) {
    auto [x, y, z] = upper_bound(pts);
    grid g(x + 1, std::vector(y + 1, std::vector<bool>(z + 1, false)));
    for (pt3 pt : pts) g[pt.x][pt.y][pt.z] = true;
    return g;
}
bool has(const grid& g, pt3 pt) {
    if (pt.x < 0 || pt.y < 0 || pt.z < 0) return false;
    if (pt.x >= g.size() || pt.y >= g[0].size() || pt.z >= g[0][0].size())
        return false;
    return g[pt.x][pt.y][pt.z];
}

int64_t surface_area(const grid& g, pt3 pt) {
    int64_t area = 6;
    static constexpr pt3 kDirs[] = {
        {-1, 0, 0}, {1, 0, 0}, {0, -1, 0}, {0, 1, 0}, {0, 0, -1}, {0, 0, 1},
    };
    for (int dir = 0; dir < 6; dir++) {
        pt3 nbr = pt + kDirs[dir];
        if (has(g, nbr)) area -= 1;
    }
    return area;
}

int64_t surface_area(const grid& g, const std::vector<pt3>& pts) {
    return std::accumulate(pts.begin(), pts.end(), 0, [&](int sum, pt3 p) {
        return sum + surface_area(g, p);
    });
}

int main(int argc, char* argv[]) {
    if (argc != 2) die("usage: day18 <file>");
    std::ifstream ifs(argv[1]);
    std::istream_iterator<pt3> begin(ifs), end;
    std::vector<pt3> pts(begin, end);
    grid g = grid_of(pts);
    std::cout << surface_area(g, pts) << std::endl;
}
