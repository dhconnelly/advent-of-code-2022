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
static constexpr std::string_view kRobotNames[] = {"ore", "clay", "obsidian",
                                                   "geode"};

robot lookup(const std::string& name) {
    for (int i = 0; i < 4; i++)
        if (kRobotNames[i] == name) return static_cast<robot>(i);
    die("bad robot: " + name);
}

int eat_id(std::string::const_iterator& begin,
           std::string::const_iterator end) {
    std::smatch m;
    static const std::regex id_pat(R"(Blueprint (\d+):)");
    if (!std::regex_search(begin, end, m, id_pat)) die("no id");
    begin = m[0].second;
    return std::stoi(m[1].str());
}

std::string eat_robot(std::string::const_iterator& begin,
                      std::string::const_iterator& end) {
    std::smatch m;
    static const std::regex robot_pat(R"(Each (\w+) robot costs ([^\.]+)\.)");
    if (!std::regex_search(begin, end, m, robot_pat)) die("no robot");
    begin = m[2].first;
    end = m[2].second;
    return m[1].str();
}

std::pair<std::string, int> eat_cost(std::string::const_iterator& begin,
                                     std::string::const_iterator end) {
    std::smatch m;
    static const std::regex cost_pat(R"((and )?(\d+) (\w+))");
    if (!std::regex_search(begin, end, m, cost_pat)) die("no cost");
    begin = m[0].second;
    return {m[3].str(), std::stoi(m[2].str())};
}

using cost = std::array<int, 4>;
struct blueprint {
    int id;
    std::array<cost, 4> costs;
};
std::ostream& operator<<(std::ostream& os, const blueprint& bp) {
    os << "[ id=" << bp.id << " |";
    for (int i = 0; i < 4; i++) {
        os << ' ' << kRobotNames[i] << "=(";
        for (int j = 0; j < 4; j++) {
            os << " " << kRobotNames[j] << ":" << bp.costs[i][j] << " ";
        }
        os << ") ";
    }
    return os << " ]";
}
std::istream& operator>>(std::istream& is, blueprint& bp) {
    std::string line;
    if (!std::getline(is, line)) return is;
    auto it = line.cbegin(), end = line.cend();
    bp.id = eat_id(it, end);
    for (int i = 0; i < 4; i++) {
        bp.costs[i] = {0, 0, 0, 0};
        auto robot_end = end;
        std::string robot = eat_robot(it, robot_end);
        if (robot != kRobotNames[i]) die("wrong robot");
        while (it != robot_end) {
            auto [target, cost] = eat_cost(it, robot_end);
            bp.costs[i][static_cast<int>(lookup(target))] = cost;
        }
        it = robot_end;
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
