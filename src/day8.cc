#include <cstring>
#include <fstream>
#include <iostream>
#include <limits>
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
    auto visible_along = [&](pt2 start, pt2 end, pt2 step) {
        int h = get(g, start);
        for (auto cur = start; cur != end; cur = add(cur, step)) {
            if (cur != start && get(g, cur) >= h) return false;
        }
        return true;
    };
    int visible = 0;
    for (int row = 0; row < g.size(); row++) {
        for (int col = 0; col < g[row].size(); col++) {
            std::pair cur{row, col};
            if (visible_along(cur, {-1, col}, {-1, 0})) visible++;
            else if (visible_along(cur, {g.size(), col}, {1, 0})) visible++;
            else if (visible_along(cur, {row, -1}, {0, -1})) visible++;
            else if (visible_along(cur, {row, g[0].size()}, {0, 1})) visible++;
        }
    }
    return visible;
}

int max_score(const grid& g) {
    auto count_visible = [&](pt2 start, pt2 end, pt2 step) {
        int h = get(g, start), n = 0;
        for (auto cur = add(start, step); cur != end; cur = add(cur, step)) {
            n++;
            if (get(g, cur) >= h) break;
        }
        return n;
    };
    int max_score = std::numeric_limits<int>::min();
    for (int row = 0; row < g.size(); row++) {
        for (int col = 0; col < g[row].size(); col++) {
            std::pair cur{row, col};
            int score = count_visible(cur, {-1, col}, {-1, 0}) *
                        count_visible(cur, {g.size(), col}, {1, 0}) *
                        count_visible(cur, {row, -1}, {0, -1}) *
                        count_visible(cur, {row, g[0].size()}, {0, 1});
            if (score > max_score) max_score = score;
        }
    }
    return max_score;
}

int main(int argc, char* argv[]) {
    if (argc != 2) die("usage: day8 <file>");
    std::ifstream ifs(argv[1]);
    auto g = read_grid(ifs);
    std::cout << count_visible(g) << std::endl;
    std::cout << max_score(g) << std::endl;
}
