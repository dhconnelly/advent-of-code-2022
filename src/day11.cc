#include <cinttypes>
#include <fstream>
#include <iostream>
#include <iterator>
#include <list>
#include <numeric>
#include <regex>
#include <string>
#include <vector>

#include "util.h"

enum class arg_type { lit, old };
enum class binary_op { add, mul };
struct operation {
    binary_op op;
    arg_type args[2];
    int64_t lits[2];
};
int64_t evaluate(const operation& op, int64_t old) {
    int64_t left = op.args[0] == arg_type::lit ? op.lits[0] : old;
    int64_t right = op.args[1] == arg_type::lit ? op.lits[1] : old;
    return op.op == binary_op::add ? left + right : left * right;
}

struct monkey {
    std::list<int64_t> items;
    operation worry_operation;
    int64_t test_modulus;
    int true_dest, false_dest;
    int inspections = 0;
};

void turn(std::vector<monkey>& monkeys, monkey& m, int modulus, bool reduce) {
    for (auto it = m.items.begin(); it != m.items.end();) {
        m.inspections++;
        *it = evaluate(m.worry_operation, *it);
        *it = reduce ? *it / 3 : *it % modulus;
        int dest = *it % m.test_modulus ? m.false_dest : m.true_dest;
        monkeys[dest].items.push_back(*it);
        it = m.items.erase(it);
    }
}

void round(std::vector<monkey>& monkeys, int modulus, bool reduce) {
    for (auto& monkey : monkeys) turn(monkeys, monkey, modulus, reduce);
}

int64_t monkey_business(std::vector<monkey>& monkeys) {
    std::sort(monkeys.begin(), monkeys.end(), [](auto& m1, auto& m2) {
        return m1.inspections > m2.inspections;
    });
    return monkeys[0].inspections * monkeys[1].inspections;
}

int modulus(const std::vector<monkey>& monkeys) {
    return std::accumulate(
        monkeys.begin(), monkeys.end(), 1,
        [](int mod, auto& m) { return mod * m.test_modulus; });
}

std::list<int64_t> parse_items(const std::string& s) {
    std::list<int64_t> items;
    static const std::regex item_pat(R"((, )?(\d+))");
    std::sregex_iterator begin(s.begin(), s.end(), item_pat), end;
    for (auto it = begin; it != end; ++it) {
        items.push_back(std::stoll((*it)[2].str()));
    }
    return items;
}

operation parse_operation(const std::string& s) {
    static const std::regex expr_pat(R"(((\d+)|old) (\+|\*) ((\d+)|old))");
    std::smatch m;
    if (!std::regex_search(s, m, expr_pat)) die("bad operation: " + s);
    operation op;
    op.args[0] = *m[1].first == 'o' ? arg_type::old : arg_type::lit;
    if (op.args[0] == arg_type::lit) op.lits[0] = std::stoll(m[1].str());
    op.op = *m[3].first == '*' ? binary_op::mul : binary_op::add;
    op.args[1] = *m[4].first == 'o' ? arg_type::old : arg_type::lit;
    if (op.args[1] == arg_type::lit) op.lits[1] = std::stoll(m[4].str());
    return op;
}

int parse_num(const std::string& s) {
    static const std::regex pat(R"((\d+))");
    std::smatch m;
    if (!std::regex_search(s, m, pat)) die("bad num");
    return std::stoi(m[1].str());
}

std::string& eatline(std::istream& is, std::string& buf) {
    if (!std::getline(is, buf)) die(std::string("getline: ") + strerror(errno));
    return buf;
}

std::vector<monkey> parse(std::istream&& is) {
    std::vector<monkey> monkeys;
    std::string line;
    while (!is.eof()) {
        eatline(is, line);
        auto items = parse_items(eatline(is, line));
        auto op = parse_operation(eatline(is, line));
        int64_t test_modulus = parse_num(eatline(is, line));
        int true_dest = parse_num(eatline(is, line));
        int false_dest = parse_num(eatline(is, line));
        std::getline(is, line);  // delimiter can be empty
        monkeys.push_back({items, op, test_modulus, true_dest, false_dest});
    }
    return monkeys;
}

int main(int argc, char* argv[]) {
    if (argc != 2) die("usage: day11 <file>");
    auto monkeys = parse(std::ifstream(argv[1]));
    int mod = modulus(monkeys);

    auto monkeys1 = monkeys;
    for (int i = 0; i < 20; i++) round(monkeys1, mod, true);
    std::cout << monkey_business(monkeys1) << std::endl;

    auto monkeys2 = monkeys;
    for (int i = 0; i < 10000; i++) round(monkeys2, mod, false);
    std::cout << monkey_business(monkeys2) << std::endl;
}
