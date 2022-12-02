#include <algorithm>
#include <cstring>
#include <fstream>
#include <iostream>
#include <istream>
#include <numeric>
#include <vector>

[[noreturn]] void die(const std::string_view msg) {
    std::cerr << msg << std::endl;
    std::exit(1);
}

std::vector<int> parse_calories(std::istream& is) {
    if (!is.good()) die(strerror(errno));
    std::vector<int> calories(1);
    std::string buf;
    while (std::getline(is, buf)) {
        if (buf.empty()) calories.push_back(0);
        else calories.back() += std::stoi(buf);
    }
    if (is.bad()) die(strerror(errno));
    return calories;
}

int main(int argc, char* argv[]) {
    if (argc != 2) die("usage: day1 <input>");
    std::ifstream ifs(argv[1]);
    auto es = parse_calories(ifs);
    std::sort(es.begin(), es.end());
    std::cout << es.back() << std::endl;
    std::cout << std::accumulate(es.rbegin(), es.rbegin() + 3, 0) << std::endl;
}
