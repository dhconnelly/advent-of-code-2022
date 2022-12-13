#include <fstream>
#include <iostream>
#include <utility>
#include <vector>

#include "util.h"

// TODO: try again to use std::variant here :(
struct item;
using packet = std::vector<item>;
struct item {
    enum class type { num, packet };

    type typ;
    int num;
    packet p;

    static item from(int num) { return {.typ = type::num, .num = num}; }
    static item from(packet p) { return {.typ = type::packet, .p = p}; }
    packet to_packet() const {
        packet p;
        p.push_back(*this);
        return p;
    }
};

void print(const item& i) {
    switch (i.typ) {
        case item::type::num: std::cout << i.num << ' '; break;
        case item::type::packet:
            std::cout << '(';
            for (int j = 0; j < i.p.size(); j++) print(i.p[j]);
            std::cout << ')';
            break;
    }
}

void print(const packet& p) { print(item::from(p)); }

packet parse_packet(std::istream& is);
item parse_item(std::istream& is) {
    if (is.peek() == '[') return item::from(parse_packet(is));
    else if (int num; is >> num) return item::from(num);
    else die("bad item");
}

packet parse_packet(std::istream& is) {
    if (is.get() != '[') die("want [");
    packet p;
    while (is && is.peek() != ']') {
        p.push_back(parse_item(is));
        if (is.peek() == ',') is.get();
    }
    if (is.get() != ']') die("want ]");
    if (is.peek() == '\n') is.get();
    return p;
}

std::vector<std::pair<packet, packet>> parse(std::istream&& is) {
    std::vector<std::pair<packet, packet>> packet_pairs;
    for (std::string line; is;) {
        packet left = parse_packet(is);
        packet right = parse_packet(is);
        packet_pairs.push_back({left, right});
        std::getline(is, line);
    }
    return packet_pairs;
}

int compare(const item& left, const item& right);
int compare(const packet& left, const packet& right) {
    std::cout << "comparing: ";
    print(left);
    std::cout << " and ";
    print(right);
    std::cout << std::endl;
    int i;
    for (i = 0; i < left.size() && i < right.size(); i++) {
        if (int d = compare(left[i], right[i]); d < 0) {
            std::cout << "left side smaller\n";
            return -1;
        } else if (d > 0) {
            std::cout << "right side smaller\n";
            return 1;
        }
    }
    if (i == left.size() && i < right.size()) {
        std::cout << "left ran out\n";
        return -1;
    }
    if (i == right.size() && i < left.size()) {
        std::cout << "right ran out\n";
        return 1;
    }
    return 0;
}

int compare(const item& left, const item& right) {
    std::cout << "comparing: ";
    print(left);
    std::cout << " and ";
    print(right);
    std::cout << std::endl;
    if (left.typ == right.typ && right.typ == item::type::num) {
        return left.num - right.num;
    } else if (left.typ == right.typ && left.typ == item::type::packet) {
        return compare(left.p, right.p);
    } else if (left.typ == item::type::num) {
        return compare(left.to_packet(), right.p);
    } else {
        return compare(left.p, right.to_packet());
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) die("usage: day13 <file>");
    auto packet_pairs = parse(std::ifstream(argv[1]));
    int sum = 0;
    for (int i = 0; i < packet_pairs.size(); i++) {
        const auto& [left, right] = packet_pairs[i];
        if (compare(left, right) < 0) {
            std::cout << "right order: " << i + 1 << std::endl;
            sum += i + 1;
        }
    }
    std::cout << sum << std::endl;
}
