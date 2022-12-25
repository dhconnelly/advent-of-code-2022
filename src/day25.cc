#include <cinttypes>
#include <fstream>
#include <iostream>
#include <string>

#include "util.h"

int64_t value(char c) {
    switch (c) {
        case '-': return -1;
        case '=': return -2;
        default: return c - '0';
    }
}

std::pair<int, char> snafu(int val) {
    switch (val) {
        case -5: return {-1, '0'};
        case -4: return {-1, '1'};
        case -3: return {-1, '2'};
        case -2: return {0, '='};
        case -1: return {0, '-'};
        case 0: return {0, '0'};
        case 1: return {0, '1'};
        case 2: return {0, '2'};
        case 3: return {1, '='};
        case 4: return {1, '-'};
        case 5: return {1, '0'};
        default: die("unreachable");
    }
}

std::string add(const std::string& x, const std::string& y) {
    int carry = 0;
    std::string result_rev;
    for (int i = 0; carry != 0 || i < x.size() || i < y.size(); i++) {
        int val = carry;
        carry = 0;
        if (i < x.size()) val += value(x[x.size() - 1 - i]);
        if (i < y.size()) val += value(y[y.size() - 1 - i]);
        auto [next_carry, cur] = snafu(val);
        carry = next_carry;
        result_rev.push_back(cur);
    }
    std::reverse(result_rev.begin(), result_rev.end());
    return result_rev;
}

std::string sum(std::istream&& is) {
    std::string sum;
    eatline(is, sum);
    for (std::string line; std::getline(is, line);) sum = add(sum, line);
    return sum;
}

int main(int argc, char* argv[]) {
    if (argc != 2) die("usage: day25 <file>");
    std::cout << sum(std::ifstream(argv[1])) << std::endl;
}
