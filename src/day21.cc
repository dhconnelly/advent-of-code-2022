#include <fstream>
#include <iostream>
#include <map>
#include <regex>
#include <string>
#include <utility>

#include "util.h"

enum class type { val, binary };
struct expr {
    type typ;
    int64_t val;
    char op;
    std::pair<std::string, std::string> args;
};

std::pair<std::string, expr> parse(const std::string& s) {
    std::smatch m;
    static const std::regex lit(R"(^(\w+): (\d+)$)");
    if (std::regex_match(s, m, lit)) {
        return {m[1].str(), {.typ = type::val, .val = int_match(m, 2)}};
    }
    static const std::regex binary(R"(^(\w+): (\w+) ([\+\-\*\/]) (\w+)$)");
    if (std::regex_match(s, m, binary)) {
        std::pair args(m[2].str(), m[4].str());
        return {m[1].str(),
                {.typ = type::binary, .op = *m[3].first, .args = args}};
    }
    die("bad rule: " + s);
}

std::ostream& operator<<(std::ostream& os, const expr& e) {
    if (e.typ == type::val) return os << e.val;
    else return os << e.args.first << ' ' << e.op << ' ' << e.args.second;
}

std::map<std::string, expr> parse(std::istream&& is) {
    std::map<std::string, expr> rules;
    for (std::string s; std::getline(is, s);) rules.insert(parse(s));
    return rules;
}

int main(int argc, char* argv[]) {
    if (argc != 2) die("usage: day21 <file>");
    std::map rules = parse(std::ifstream(argv[1]));
    for (auto [name, rule] : rules)
        std::cout << name << ": " << rule << std::endl;
}
