#include "aoc.h"
#include "util.h"

int main(int argc, char* argv[]) {
    for (int i = 1; i < argc; i++) {
        auto test = kTests.find(argv[i]);
        if (test == kTests.end()) die("not found: " + std::string(argv[i]));
        test->second();
    }
    if (argc == 1) {
        std::cout << "running all tests\n";
        for (const auto& [_, test] : kTests) test();
    }
    return 0;
}
