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
    arg_type arg_types[2];
    int64_t lit_vals[2];
};
int64_t evaluate(const operation& op, int64_t old) {
    int64_t left = op.arg_types[0] == arg_type::lit ? op.lit_vals[0] : old;
    int64_t right = op.arg_types[1] == arg_type::lit ? op.lit_vals[1] : old;
    return op.op == binary_op::add ? left + right : left * right;
}

struct item {
    int64_t worry_level;
};

struct monkey {
    int id;
    std::list<item> items;
    operation worry_operation;
    int64_t test_modulus;
    int true_dest, false_dest;
    int inspections = 0;
};

void turn(std::vector<monkey>& monkeys, monkey& m, int modulus,
          bool reduce_stress) {
    for (auto it = m.items.begin(); it != m.items.end();) {
        m.inspections++;
        item i = *it;
        i.worry_level = evaluate(m.worry_operation, i.worry_level);
        if (reduce_stress) i.worry_level /= 3;
        else i.worry_level %= modulus;
        it = m.items.erase(it);
        int dest = i.worry_level % m.test_modulus ? m.false_dest : m.true_dest;
        monkeys[dest].items.push_back(i);
    }
}

void round(std::vector<monkey>& monkeys, int modulus, bool reduce_stress) {
    for (auto& monkey : monkeys) turn(monkeys, monkey, modulus, reduce_stress);
}

int64_t monkey_business(const std::vector<monkey>& monkeys) {
    std::vector monkeys2(monkeys);
    std::sort(monkeys2.begin(), monkeys2.end(), [](auto& m1, auto& m2) {
        return m1.inspections > m2.inspections;
    });
    return monkeys2[0].inspections * monkeys2[1].inspections;
}

int modulus(const std::vector<monkey>& monkeys) {
    return std::accumulate(
        monkeys.begin(), monkeys.end(), 1,
        [](int mod, auto& m) { return mod * m.test_modulus; });
}

std::list<item> parse_items(const std::string& s) {
    std::list<item> items;
    static const std::regex item_pat(R"((, )?(\d+))");
    std::sregex_iterator begin(s.begin(), s.end(), item_pat), end;
    for (auto it = begin; it != end; ++it) {
        items.push_back(item{std::stoll((*it)[2].str())});
    }
    return items;
}

operation parse_operation(const std::string& s) {
    // todo: regex
    operation op;
    int lbegin = s.find('=') + 2;
    int lend = s.find(' ', lbegin);
    op.arg_types[0] = s[lbegin] == 'o' ? arg_type::old : arg_type::lit;
    if (op.arg_types[0] == arg_type::lit) {
        op.lit_vals[0] = std::stoll(s.substr(lbegin, lend));
    }
    char opc = s[lend + 1];
    op.op = opc == '*' ? binary_op::mul : binary_op::add;
    int rbegin = lend + 3;
    int rend = s.size();
    op.arg_types[1] = s[rbegin] == 'o' ? arg_type::old : arg_type::lit;
    if (op.arg_types[1] == arg_type::lit) {
        op.lit_vals[1] = std::stoll(s.substr(rbegin, rend));
    }
    return op;
}

void eatline(std::istream& is, std::string& buf) {
    if (!std::getline(is, buf)) die(std::string("getline: ") + strerror(errno));
}

std::vector<monkey> parse(std::istream&& is) {
    std::vector<monkey> monkeys;
    std::string line;
    for (int i = 0; !is.eof(); i++) {
        // header
        eatline(is, line);
        if (is.eof()) break;
        // items
        eatline(is, line);
        auto items = parse_items(line.substr(line.find(':') + 2));
        // operation
        eatline(is, line);
        auto op = parse_operation(line.substr(line.find(':') + 2));
        // test
        eatline(is, line);
        int64_t test_modulus = std::stoll(line.substr(line.find("by") + 3));
        // destinations
        eatline(is, line);
        int true_dest = std::stoi(line.substr(line.find("monkey") + 7));
        eatline(is, line);
        int false_dest = std::stoi(line.substr(line.find("monkey") + 7));
        // delimiter can be empty
        std::getline(is, line);
        monkeys.push_back({i, items, op, test_modulus, true_dest, false_dest});
    }
    return monkeys;
}

int main(int argc, char* argv[]) {
    if (argc != 2) die("usage: day11 <file>");
    auto monkeys1 = parse(std::ifstream(argv[1]));
    auto monkeys2 = monkeys1;
    int mod = modulus(monkeys1);

    for (int i = 0; i < 20; i++) round(monkeys1, mod, true);
    std::cout << monkey_business(monkeys1) << std::endl;

    for (int i = 0; i < 10000; i++) round(monkeys2, mod, false);
    std::cout << monkey_business(monkeys2) << std::endl;
}
