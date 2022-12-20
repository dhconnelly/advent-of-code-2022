#include <fstream>
#include <iostream>
#include <iterator>
#include <list>
#include <vector>

#include "util.h"

using file = std::list<int64_t>;
using file_it = file::const_iterator;

void print(const file& f) {
    for (int64_t x : f) std::cout << x << ' ';
    std::cout << std::endl;
}

file_it shift(const file& f, file_it it, int64_t n) {
    if (n < 0) {
        for (int64_t i = 0; i < -n; i++) {
            if (it == f.begin()) it = f.end();
            --it;
        }
        if (it == f.begin()) it = f.end();
    } else if (n > 0) {
        for (int64_t i = 0; i < n; i++) {
            if (it == f.end()) it = f.begin();
            ++it;
        }
        if (it == f.end()) it = f.begin();
    }
    return it;
}

void mix(file& f) {
    std::vector<file_it> iterators;
    for (auto it = f.begin(); it != f.end(); ++it) iterators.push_back(it);
    for (auto it : iterators) {
        int val = *it;
        it = f.erase(it);
        file_it dest = shift(f, it, val);
        f.insert(dest, val);
    }
}

int64_t after(const file& f, file_it base, int64_t n) {
    for (int64_t k = 0; k < (n % f.size()); k++) {
        if (++base == f.end()) base = f.begin();
    }
    return *base;
}

int main(int argc, char* argv[]) {
    if (argc != 2) die("usage: day20 <file>");
    std::ifstream ifs(argv[1]);
    std::istream_iterator<int64_t> begin(ifs), end;
    file f(begin, end);
    mix(f);

    auto it = std::find(f.begin(), f.end(), 0);
    if (it == f.end()) die("no zero");
    auto x = after(f, it, 1000);
    auto y = after(f, it, 2000);
    auto z = after(f, it, 3000);
    std::cout << (x + y + z) << std::endl;
}
