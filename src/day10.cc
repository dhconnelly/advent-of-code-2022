#include <fstream>
#include <iostream>
#include <iterator>

#include "util.h"

enum class opcode { addx, noop };
constexpr int duration(opcode op) { return op == opcode::addx ? 2 : 1; }

struct instr {
    opcode op;
    int arg;
};

std::istream& operator>>(std::istream& is, instr& i) {
    char ch;
    is >> ch;
    i.op = ch == 'a' ? opcode::addx : opcode::noop;
    for (int i = 0; i < 3; i++) is >> ch;
    if (i.op == opcode::addx) is >> i.arg;
    return is;
}

template <typename ProgramIt>
class vm {
public:
    enum class state { executing, halted };
    vm(ProgramIt it, ProgramIt end) : it_(it), end_(end) {
        state_ = it == end ? state::halted : state::executing;
        if (state_ != state::halted) remaining_ = duration(it_->op);
    }
    int cycle() const { return cycle_; }
    int x() const { return x_; }
    bool halted() const { return state_ == state::halted; }
    void tick() {
        if (state_ == state::halted) die("halted");
        if (--remaining_ == 0) {
            if (it_->op == opcode::addx) x_ += it_->arg;
            if (++it_ == end_) state_ = state::halted;
            else remaining_ = duration(it_->op);
        }
        cycle_++;
    }

private:
    state state_;
    ProgramIt it_, end_;
    int cycle_ = 1, remaining_;
    int x_ = 1;
};

template <typename T>
bool sprite_active(int pixel, const vm<T>& vm) {
    int x = vm.x();
    return x - 1 == pixel || x == pixel || x + 1 == pixel;
}

void run(std::istream&& is) {
    size_t sum = 0;
    std::istream_iterator<instr> begin(is), end;
    for (vm vm(begin, end); !vm.halted(); vm.tick()) {
        int pixel = (vm.cycle() - 1) % 40;
        if (vm.cycle() > 1 && pixel == 0) std::cout << std::endl;
        std::cout << (sprite_active(pixel, vm) ? '#' : ' ');
        if (auto c = vm.cycle(); c > 1 && (c - 20) % 40 == 0) sum += c * vm.x();
    }
    std::cout << std::endl << sum << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc != 2) die("usage: day10 <file>");
    run(std::ifstream(argv[1]));
}
