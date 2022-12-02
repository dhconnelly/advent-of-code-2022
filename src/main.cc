#include <fstream>
#include <iostream>

#include "aoc.h"
#include "util.h"

int main(int argc, char* argv[]) {
    // TODO: static dispatch
    if (argc != 3) die("usage: aoc2022 <day> <input>");
    auto day = argv[1];
    auto solution = kSolutions.find(day);
    if (solution == kSolutions.end()) die("solution not found");
    std::ifstream ifs(argv[2]);
    if (!ifs.good()) die(strerror(errno));
    solution->second(ifs);
}
