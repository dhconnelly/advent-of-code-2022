#include <algorithm>
#include <fstream>
#include <iostream>
#include <numeric>
#include <sstream>

#include "util.h"

int main(int argc, char* argv[]) {
    if (argc != 2) die("usage: day1 <input>");
    std::ifstream ifs(argv[1]);
    auto es = parse_entries(ifs, [](const auto& s) {
        int sum = 0, val;
        for (std::stringstream ss(s); ss >> val;) sum += val;
        return sum;
    });
    std::sort(es.rbegin(), es.rend());
    std::cout << es[0] << std::endl;
    std::cout << std::accumulate(es.begin(), es.begin() + 3, 0) << std::endl;
}
