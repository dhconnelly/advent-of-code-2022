#include <cstring>
#include <fstream>
#include <iostream>
#include <numeric>
#include <set>
#include <utility>
#include <vector>

#include "util.h"

using grid = std::vector<std::vector<int>>;
using pt2 = std::pair<int, int>;

grid read_grid(std::istream& is) {
    grid g(1);
    for (int ch = is.get(); is; ch = is.get()) {
        if (is.peek() == -1) break;
        else if (ch == '\n') g.emplace_back();
        else g.back().push_back(ch - '0');
    }
    if (!is.eof()) die(strerror(errno));
    return g;
}

pt2 add(pt2 a, pt2 b) { return {a.first + b.first, a.second + b.second}; }
int get(const grid& g, pt2 at) { return g[at.first][at.second]; }

int count_visible(const grid& g) {
    std::set<pt2> visible;
    auto count_along = [&](pt2 start, pt2 end, pt2 step) {
        int max;
        for (auto cur = start; cur != end; cur = add(cur, step)) {
            if (int h = get(g, cur); cur == start || h > max) {
                visible.insert(cur);
                max = h;
            }
        }
    };
    for (int row = 0; row < g.size(); row++) {
        count_along({row, 0}, {row, g[0].size()}, {0, 1});
        count_along({row, g[0].size() - 1}, {row, -1}, {0, -1});
    }
    for (int col = 0; col < g[0].size(); col++) {
        count_along({0, col}, {g.size(), col}, {1, 0});
        count_along({g.size() - 1, col}, {-1, col}, {-1, 0});
    }
    return visible.size();
}

int main(int argc, char* argv[]) {
    if (argc != 2) die("usage: day8 <file>");
    std::ifstream ifs(argv[1]);
    auto g = read_grid(ifs);
    std::cout << count_visible(g) << std::endl;
}
