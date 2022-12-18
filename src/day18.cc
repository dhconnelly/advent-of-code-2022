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

int main(int argc, char* argv[]) {
    if (argc != 2) die("usage: day18 <file>");
    std::ifstream ifs(argv[1]);
    std::istream_iterator<pt3> begin(ifs), end;
    std::vector<pt3> pts(begin, end);
    grid g = grid_of(pts);
}
