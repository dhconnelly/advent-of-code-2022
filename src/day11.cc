#include <fstream>
#include <iostream>
#include <iterator>
#include <list>
#include <numeric>
#include <string>
#include <vector>

#include "util.h"

enum class arg_type { lit, old };
enum class binary_op { add, mul };
struct operation {
    binary_op op;
    arg_type arg_types[2];
    int lit_vals[2];
};

int evaluate(const operation& op, int old) {
    int left = op.arg_types[0] == arg_type::lit ? op.lit_vals[0] : old;
    int right = op.arg_types[1] == arg_type::lit ? op.lit_vals[1] : old;
    return op.op == binary_op::add ? left + right : left * right;
}

struct item {
    int worry_level;
};

struct monkey {
    int id;
    std::list<item> items;
    operation worry_operation;
    int test_modulus;
    int true_dest, false_dest;
    int inspections = 0;
};

void turn(std::vector<monkey>& monkeys, monkey& m) {
    std::cout << "Monkey " << m.id << std::endl;
    for (auto it = m.items.begin(); it != m.items.end();) {
        m.inspections++;
        item i = *it;
        std::cout << "  inspects " << i.worry_level << std::endl;
        i.worry_level = evaluate(m.worry_operation, i.worry_level);
        std::cout << "    -> " << i.worry_level << std::endl;
        i.worry_level /= 3;
        std::cout << "    -> " << i.worry_level << std::endl;
        if (i.worry_level % m.test_modulus == 0) {
            std::cout << "    true, thrown to " << m.true_dest << std::endl;
            it = m.items.erase(it);
            monkeys[m.true_dest].items.push_back(i);
        } else {
            std::cout << "    false, thrown to " << m.false_dest << std::endl;
            it = m.items.erase(it);
            monkeys[m.false_dest].items.push_back(i);
        }
    }
    std::cout << std::endl;
}

void print(const std::vector<monkey>& monkeys) {
    for (const auto& monkey : monkeys) {
        std::cout << "Monkey " << monkey.id << ": ";
        for (const auto& item : monkey.items)
            std::cout << item.worry_level << ", ";
        std::cout << std::endl;
    }
}

void round(std::vector<monkey>& monkeys) {
    for (auto& monkey : monkeys) turn(monkeys, monkey);
    print(monkeys);
}

std::list<item> parse_items(const std::string& s) {
    // todo: regex
    std::list<item> items;
    int from = 0;
    int to = s.find(", ");
    if (to == std::string::npos) to = s.size();
    items.push_back(item{std::stoi(s.substr(from, to))});
    while (to != s.size()) {
        from = to + 2;
        to = s.find(", ", from);
        if (to == std::string::npos) to = s.size();
        items.push_back(item{std::stoi(s.substr(from, to))});
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
        op.lit_vals[0] = std::stoi(s.substr(lbegin, lend));
    }
    char opc = s[lend + 1];
    op.op = opc == '*' ? binary_op::mul : binary_op::add;
    int rbegin = lend + 3;
    int rend = s.size();
    op.arg_types[1] = s[rbegin] == 'o' ? arg_type::old : arg_type::lit;
    if (op.arg_types[1] == arg_type::lit) {
        op.lit_vals[1] = std::stoi(s.substr(rbegin, rend));
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
        int test_modulus = std::stoi(line.substr(line.find("by") + 3));
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

std::ostream& operator<<(std::ostream& os, const monkey& m) {
    std::cout << "monkey " << m.id << std::endl;
    std::cout << "items: ";
    for (const auto& item : m.items) std::cout << item.worry_level << ", ";
    std::cout << std::endl;
    std::cout << "operation: ";
    if (m.worry_operation.arg_types[0] == arg_type::old) std::cout << "old";
    else std::cout << m.worry_operation.lit_vals[0];
    std::cout << ' ';
    if (m.worry_operation.op == binary_op::add) std::cout << '+';
    else std::cout << '*';
    std::cout << ' ';
    if (m.worry_operation.arg_types[1] == arg_type::old) std::cout << "old";
    else std::cout << m.worry_operation.lit_vals[1];
    std::cout << std::endl;
    std::cout << "test: " << m.test_modulus << std::endl;
    std::cout << "true: " << m.true_dest << std::endl;
    std::cout << "false: " << m.false_dest << std::endl;
    return os;
}

int monkey_business(const std::vector<monkey>& monkeys) {
    std::vector monkeys2(monkeys);
    std::sort(monkeys2.begin(), monkeys2.end(), [](auto& m1, auto& m2) {
        return m1.inspections > m2.inspections;
    });
    return monkeys2[0].inspections * monkeys2[1].inspections;
}

int main(int argc, char* argv[]) {
    if (argc != 2) die("usage: day11 <file>");
    auto monkeys = parse(std::ifstream(argv[1]));
    for (const auto& m : monkeys) std::cout << m << std::endl;
    print(monkeys);
    for (int i = 0; i < 20; i++) round(monkeys);
    std::cout << monkey_business(monkeys) << std::endl;
}
