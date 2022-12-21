#include <fstream>
#include <iostream>
#include <iterator>
#include <list>
#include <vector>

#include "util.h"

using file = std::list<int64_t>;
using file_it = file::const_iterator;

file_it shift(const file& f, file_it it, int64_t n) {
    int64_t k = std::abs(n) % static_cast<int64_t>(f.size());
    if (n < 0) {
        for (int64_t i = 0; i < k; i++) {
            if (it == f.begin()) it = f.end();
            --it;
        }
        if (it == f.begin()) it = f.end();
    } else if (n > 0) {
        for (int64_t i = 0; i < k; i++) {
            if (it == f.end()) it = f.begin();
            ++it;
        }
        if (it == f.end()) it = f.begin();
    }
    return it;
}

void mix(file& f, int n = 1) {
    std::vector<file_it> iterators;
    for (auto it = f.begin(); it != f.end(); ++it) iterators.push_back(it);
    while (n--) {
        for (auto& it : iterators) {
            int64_t val = *it;
            file_it dest = shift(f, f.erase(it), val);
            it = f.insert(dest, val);
        }
    }
}

int64_t after(const file& f, file_it base, int64_t n) {
    for (int64_t k = 0; k < (n % f.size()); k++) {
        if (++base == f.end()) base = f.begin();
    }
    return *base;
}

int64_t grove_sum(file f, int mixes, int factor) {
    for (auto& x : f) x *= factor;
    mix(f, mixes);
    auto it = std::find(f.begin(), f.end(), 0);
    if (it == f.end()) die("no zero");
    auto x = after(f, it, 1000);
    auto y = after(f, it, 2000);
    auto z = after(f, it, 3000);
    return (x + y + z);
}

int main(int argc, char* argv[]) {
    if (argc != 2) die("usage: day20 <file>");
    std::ifstream ifs(argv[1]);
    std::istream_iterator<int64_t> begin(ifs), end;
    file f(begin, end);
    std::cout << grove_sum(f, 1, 1) << std::endl;
    std::cout << grove_sum(f, 10, 811589153) << std::endl;
}
