#include <cctype>
#include <fstream>
#include <iostream>
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

using maze = std::vector<std::string>;
using path = std::vector<step>;

std::pair<maze, path> parse(std::istream&& is) {
    maze m;
    for (std::string line; std::getline(is, line) && !line.empty();) {
        m.push_back(line);
    }
    path p;
    for (step s; is >> s;) p.push_back(s);
    return {m, p};
}

void print(const maze& m) {
    for (auto row : m) std::cout << row << std::endl;
    std::cout << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc != 2) die("usage: day22 <file>");
    auto [m, path] = parse(std::ifstream(argv[1]));
    print(m);
    for (step s : path) std::cout << s << std::endl;
}
