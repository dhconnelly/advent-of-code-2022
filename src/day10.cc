#include <algorithm>
#include <cinttypes>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <iterator>
#include <optional>
#include <string>
#include <unordered_set>
#include <vector>

#include "util.h"

enum class opcode : int64_t {
    addx = 0,
    noop = 1,
};

constexpr int64_t duration(opcode op) {
    switch (op) {
        case opcode::addx: return 2;
        case opcode::noop: return 1;
    }
}

struct instr {
    opcode op;
    int64_t arg;
};

std::istream& operator>>(std::istream& is, instr& i) {
    std::string op;
    if (!(is >> op)) return is;
    if (op == "addx") {
        i.op = opcode::addx;
        return is >> i.arg;
    } else {
        i.op = opcode::noop;
        return is;
    }
}

struct instr_state {
    instr i;
    size_t ticks_remaining;
};

enum class vm_state {
    executing,
    halted,
};

class vm {
public:
    explicit vm(std::vector<instr> program) : program_(program) {}
    int64_t cycle() const { return cycle_; }
    int64_t signal() const { return cycle_ * x_; }
    bool start();
    void tick();

private:
    void load();
    void log();

    size_t pc_;
    std::vector<instr> program_;
    int64_t x_;
    int64_t cycle_;
    vm_state state_ = vm_state::halted;
    instr_state istate_;
};

void vm::load() {
    if (state_ == vm_state::halted) return;
    pc_++;
    if (pc_ == program_.size()) {
        state_ = vm_state::halted;
    } else {
        istate_.i = program_[pc_];
        istate_.ticks_remaining = duration(program_[pc_].op);
    }
}

void vm::log() {
    std::cout << "[" << cycle_ << " x = " << x_ << "] ";
    switch (istate_.i.op) {
        case opcode::addx: std::cout << "addx " << istate_.i.arg; break;
        case opcode::noop: std::cout << "noop";
    }
    std::cout << std::endl;
}

bool vm::start() {
    if (program_.empty()) return false;
    pc_ = 0;
    x_ = 1;
    cycle_ = 1;
    state_ = vm_state::executing;
    istate_.i = program_[pc_];
    istate_.ticks_remaining = duration(program_[pc_].op);
    return true;
}

void vm::tick() {
    if (state_ == vm_state::halted) die("halted");
    if (getenv("LOG")) log();
    istate_.ticks_remaining--;
    if (istate_.ticks_remaining != 0) {
        // do nothing
    } else {
        if (istate_.i.op == opcode::addx) x_ += istate_.i.arg;
        load();
    }
    cycle_++;
}

int64_t signal_sum(std::istream& is,
                   const std::unordered_set<int64_t>& cycles) {
    size_t sum = 0;
    std::vector<instr> program((std::istream_iterator<instr>(is)),
                               (std::istream_iterator<instr>()));
    vm vm(program);
    if (!vm.start()) die("bad program");
    while (vm.cycle() <= *std::max_element(cycles.begin(), cycles.end())) {
        if (cycles.count(vm.cycle())) sum += vm.signal();
        vm.tick();
    }
    return sum;
}

int main(int argc, char* argv[]) {
    if (argc != 2) die("usage: day10 <file>");
    std::ifstream ifs(argv[1]);
    std::cout << signal_sum(ifs, {20, 60, 100, 140, 180, 220}) << std::endl;
}
