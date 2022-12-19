#include <array>
#include <fstream>
#include <iostream>
#include <iterator>
#include <regex>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "util.h"

enum class robot : int { ore = 0, clay = 1, obsidian = 2, geode = 3 };
// TODO: are constexpr maps supported?
constexpr std::string_view kRobots[] = {"ore", "clay", "obsidian", "geode"};
int lookup(const std::string_view name) {
    return std::find(kRobots, kRobots + 4, name) - kRobots;
}

using cost = std::array<int, 4>;
struct blueprint {
    int id;
    std::array<cost, 4> costs;
};

std::ostream& operator<<(std::ostream& os, const blueprint& bp) {
    os << "[ id=" << bp.id << " |";
    for (int i = 0; i < 4; i++) {
        os << ' ' << kRobots[i] << "=(";
        for (int j = 0; j < 4; j++) {
            os << " " << kRobots[j] << ":" << bp.costs[i][j] << " ";
        }
        os << ") ";
    }
    return os << " ]";
}

std::istream& operator>>(std::istream& is, blueprint& bp) {
    std::string line;
    if (!std::getline(is, line)) return is;
    std::smatch m;

    // id
    static const std::regex id_pat(R"(Blueprint (\d+))");
    if (!std::regex_search(line, m, id_pat)) die("no id");
    bp.id = int_match(m, 1);

    // robots
    static const std::regex robot_pat(R"(Each (\w+) robot costs ([ \w\d]+)\.)");
    std::sregex_iterator it(line.begin(), line.end(), robot_pat), end;
    for (int i = 0; i < 4; i++) {
        if (it == end) die("bad robots: " + line);
        auto m = *it++;
        if (m[1].str() != kRobots[i]) die("bad robot: " + m[1].str());

        // costs
        bp.costs[i] = {0, 0, 0, 0};
        static const std::regex costs_pat(R"((\d+) (\w+))");
        std::sregex_iterator it2(m[2].first, m[2].second, costs_pat), end2;
        for (; it2 != end2; it2++) {
            bp.costs[i][lookup((*it2)[2].str())] = int_match(*it2, 1);
        }
    }
    return is;
}

int main(int argc, char* argv[]) {
    if (argc != 2) die("usage: day19 <file>");
    std::ifstream ifs(argv[1]);
    std::istream_iterator<blueprint> begin(ifs), end;
    std::vector<blueprint> blueprints(begin, end);
    for (const auto& bp : blueprints) std::cout << bp << std::endl;
}
