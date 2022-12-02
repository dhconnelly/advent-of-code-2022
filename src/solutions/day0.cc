#include <sstream>

#include "../aoc.h"
#include "../util.h"

namespace day0 {

std::vector<std::string> parse(std::istream& is) {
    return parse_lines(is, [](const auto& s) { return s; });
}

std::string solve1(const std::vector<std::string>& entries) {
    return entries.back();
}

int solve2(const std::vector<std::string>& entries) { return entries.size(); }

void test() {
    constexpr auto s = "foo\nbar\nbaz";
    std::stringstream ss(s);
    auto lines = parse(ss);
    assert(solve1(lines) == "baz");
    assert(solve2(lines) == 3);
}

}  // namespace day0

REGISTER_SOLUTION(day0, day0::parse, day0::solve1, day0::solve2, day0::test);
