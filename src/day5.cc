#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "util.h"

std::vector<std::string> read_stacks_section(std::istream& is) {
    std::vector<std::string> lines;
    for (std::string line; std::getline(is, line) && !line.empty();) {
        lines.push_back(line);
    }
    return lines;
}

using stack = std::vector<char>;

stack parse_stack(const std::vector<std::string>& lines, int col) {
    stack s;
    for (int row = lines.size() - 2; row >= 0; row--) {
        if (lines[row][col + 1] == ' ') break;
        s.push_back(lines[row][col + 1]);
    }
    return s;
}

std::vector<stack> parse_stacks(const std::vector<std::string>& lines) {
    std::vector<stack> stacks;
    for (int col = 0; col < lines[0].size(); col += 4) {
        stacks.push_back(parse_stack(lines, col));
    }
    return stacks;
}

struct instr {
    int amt;
    int src;
    int dst;
};

std::istream& operator>>(std::istream& is, instr& i) {
    std::string buf;  // throw away
    is >> buf;
    is >> i.amt;
    is >> buf;
    is >> i.src;
    is >> buf;
    is >> i.dst;
    return is;
}

void apply1(std::vector<stack>& stacks, const instr& instr) {
    for (int i = 0; i < instr.amt; i++) {
        stacks[instr.dst - 1].push_back(stacks[instr.src - 1].back());
        stacks[instr.src - 1].pop_back();
    }
}

void apply2(std::vector<stack>& stacks, const instr& instr) {
    auto& src = stacks[instr.src - 1];
    for (int i = instr.amt; i > 0; i--) {
        stacks[instr.dst - 1].push_back(src[src.size() - i]);
    }
    src.resize(src.size() - instr.amt);
}

void print_tops(const std::vector<stack>& stacks) {
    for (const auto& stack : stacks) std::cout << stack.back();
    std::cout << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc != 2) die("usage: day5 <file>");
    std::ifstream ifs(argv[1]);
    auto stacks1 = parse_stacks(read_stacks_section(ifs));
    auto stacks2 = stacks1;
    for (auto it = std::istream_iterator<instr>(ifs);
         it != std::istream_iterator<instr>(); ++it) {
        apply1(stacks1, *it);
        apply2(stacks2, *it);
    }
    print_tops(stacks1);
    print_tops(stacks2);
}
