#include <algorithm>
#include <array>
#include <cctype>
#include <fstream>
#include <iostream>
#include <limits>
#include <map>
#include <utility>
#include <vector>

#include "util.h"

enum class step_type { turn, move };
struct step {
    step_type typ;
    char dir;
    int dist;
};

std::istream& operator>>(std::istream& is, step& s) {
    if (char c = is.peek(); std::isdigit(c)) {
        s.typ = step_type::move;
        std::string dist;
        while (std::isdigit(is.peek())) dist.push_back(is.get());
        s.dist = std::stoi(dist);
    } else if (c == 'L' || c == 'R') {
        s.typ = step_type::turn;
        s.dir = is.get();
    } else {
        is.setstate(std::istream::failbit);
    }
    return is;
}

std::ostream& operator<<(std::ostream& os, const step& s) {
    switch (s.typ) {
        case step_type::move: return os << s.dist;
        case step_type::turn: return os << s.dir;
    }
}

using grid = std::vector<std::string>;
using path = std::vector<step>;
using pt = std::pair<int, int>;
static constexpr pt oblivion{-1, -1};
using adjlist = std::array<pt, 4>;
using adjmat = std::vector<std::vector<adjlist>>;

pt wrap(const grid& g, pt p) {
    auto [row, col] = p;
    if (row < 0) row = g.size() - 1;
    else if (row >= g.size()) row = 0;
    if (col < 0) col = g[0].size() - 1;
    else if (col >= g[0].size()) col = 0;
    return {row, col};
}

enum class facing : int { right = 0, down = 1, left = 2, up = 3 };
static constexpr pt kDirs[] = {{0, 1}, {1, 0}, {0, -1}, {-1, 0}};

adjlist find_nbrs(const grid& g, int row, int col) {
    adjlist nbrs;
    for (int i = 0; i < 4; i++) {
        pt nbr = wrap(g, {row + kDirs[i].first, col + kDirs[i].second});
        while (g[nbr.first][nbr.second] == ' ') {
            nbr.first += kDirs[i].first;
            nbr.second += kDirs[i].second;
            nbr = wrap(g, nbr);
        }
        nbrs[i] = g[nbr.first][nbr.second] == '.' ? nbr : oblivion;
    }
    return nbrs;
}

pt cube_nbr(const grid& g, int row, int col, pt dir, int width) {
    // easy case: it's right there
    if (row >= 0 && row < g.size() && col >= 0 && col < g[0].size()) {
        return {row + dir.first, col + dir.second};
    }
    static constexpr pt left = kDirs[int(facing::left)],
                        right = kDirs[int(facing::right)],
                        up = kDirs[int(facing::up)],
                        down = kDirs[int(facing::down)];
    return {row, col};
}

int cube_width(const grid& g) {
    size_t width = std::numeric_limits<int>::max();
    for (const auto& row : g) {
        auto begin = row.find_first_not_of(' ');
        auto end = row.find_last_not_of(' ');
        width = std::min(width, end - begin + 1);
    }
    return width;
}

adjlist find_nbrs_cube(const grid& g, int row, int col) {
    adjlist nbrs;
    int width = cube_width(g);
    for (int i = 0; i < 4; i++) {
        pt nbr = cube_nbr(g, row, col, kDirs[i], width);
        nbrs[i] = g[nbr.first][nbr.second] == '.' ? nbr : oblivion;
    }
    return nbrs;
}

using nbrs_strategy = std::function<adjlist(const grid&, int, int)>;
adjmat build_adjmat(const grid& g, nbrs_strategy find_nbrs) {
    adjmat nbrs(g.size(), std::vector<adjlist>(g[0].size()));
    for (int row = 0; row < nbrs.size(); row++) {
        for (int col = 0; col < nbrs[0].size(); col++) {
            if (g[row][col] != '.') continue;
            nbrs[row][col] = find_nbrs(g, row, col);
        }
    }
    return nbrs;
}

void print(const pt& pt) {
    std::cout << '(' << pt.first << ", " << pt.second << ')';
}

void print(const adjmat& nbrs) {
    for (int row = 0; row < nbrs.size(); row++) {
        for (int col = 0; col < nbrs[row].size(); col++) {
            print({row, col});
            std::cout << " -> [ ";
            for (const auto& nbr : nbrs[row][col]) {
                print(nbr);
                std::cout << ' ';
            }
            std::cout << "]\n";
        }
    }
}

class maze {
public:
    maze(grid g, adjmat nbrs) : g_(g), nbrs_(nbrs) {}
    char at(pt p) const { return g_[p.first][p.second]; }
    const adjlist& nbrs(pt p) const { return nbrs_[p.first][p.second]; }

private:
    grid g_;
    adjmat nbrs_;
};

std::pair<grid, path> parse(std::istream&& is) {
    grid g;
    size_t width = 0;
    for (std::string line; std::getline(is, line) && !line.empty();) {
        g.push_back(line);
        width = std::max(width, line.size());
    }
    for (auto& row : g) {
        if (row.size() < width) row.resize(width, ' ');
    }
    path p;
    for (step s; is >> s;) p.push_back(s);
    return {g, p};
}

void print(const grid& g) {
    for (auto row : g) std::cout << row << std::endl;
    std::cout << std::endl;
}

pt find_start(const grid& g) {
    int col = 0;
    while (g[0][col] != '.') col++;
    return {0, col};
}

using pos = std::pair<pt, facing>;

pos move(const maze& m, pos cur, step s) {
    if (s.typ == step_type::move) {
        for (int i = 0; i < s.dist; i++) {
            pt nbr = m.nbrs(cur.first)[int(cur.second)];
            if (nbr == oblivion) break;
            cur.first = nbr;
        }
    } else {
        int dir = int(cur.second) + 4;
        cur.second = facing((dir + (s.dir == 'R' ? 1 : -1)) % 4);
    }
    return cur;
}

void print(grid g, const pos& cur) {
    char c;
    switch (cur.second) {
        case facing::down: c = 'v'; break;
        case facing::up: c = '^'; break;
        case facing::left: c = '<'; break;
        case facing::right: c = '>'; break;
    }
    g[cur.first.first][cur.first.second] = c;
    print(g);
}

int compute_password(const grid& g, const path& p, nbrs_strategy find_nbrs) {
    maze m(g, build_adjmat(g, find_nbrs));
    pos cur(find_start(g), facing::right);
    for (step s : p) cur = move(m, cur, s);
    return 1000 * (cur.first.first + 1) + 4 * (cur.first.second + 1) +
           int(cur.second);
}

int main(int argc, char* argv[]) {
    if (argc != 2) die("usage: day22 <file>");
    auto [g, p] = parse(std::ifstream(argv[1]));
    std::cout << compute_password(g, p, find_nbrs) << std::endl;
    std::cout << cube_width(g) << std::endl;
    // std::cout << compute_password(g, p, find_nbrs_cube) << std::endl;
}
