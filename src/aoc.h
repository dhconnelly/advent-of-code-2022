#ifndef SRC_AOC_H_
#define SRC_AOC_H_

#include <istream>
#include <map>

using TestFn = std::function<void()>;
using SolutionFn = std::function<void(std::istream&)>;

extern std::map<std::string, TestFn> kTests;
extern std::map<std::string, SolutionFn> kSolutions;

class SolutionRegistration {
public:
    SolutionRegistration(const std::string& day, SolutionFn solution,
                         TestFn test) {
        kTests[day] = test;
        kSolutions[day] = solution;
    }
};

// register a solution with the framework.
//
// day: the name of the solution
// parse: a function with the signature T parse(std::istream&)
// solve1: takes the parsed input and returns an output that can be written
//     to stdout. signature: O solve(T&)
// solve2: also takes the parsed input and returns an output.
//     signature: O solve(T&)
// test: a function that runs test cases. signature: void test()
//
#define REGISTER_SOLUTION(day, parse, solve1, solve2, test) \
    void solve_##day(std::istream& is) {                    \
        auto t = parse(is);                                 \
        std::cout << solve1(t) << std::endl;                \
        std::cout << solve2(t) << std::endl;                \
    }                                                       \
                                                            \
    void test_##day() {                                     \
        std::cout << "running: " << #day << std::endl;      \
        test();                                             \
    }                                                       \
                                                            \
    static SolutionRegistration reg(#day, solve_##day, test_##day);

#endif  // SRC_AOC_H_
