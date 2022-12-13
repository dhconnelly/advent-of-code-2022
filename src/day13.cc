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
};

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

int compare(item& left, item& right);
int compare(packet& left, packet& right) {
    int i;
    for (i = 0; i < left.size() && i < right.size(); i++) {
        if (int d = compare(left[i], right[i]); d < 0) return -1;
        else if (d > 0) return 1;
    }
    if (i == left.size()) return -1;
    if (i == right.size()) return 1;
    return 0;
}

void print(const item& i) {
    switch (i.typ) {
        case item::type::num: std::cout << i.num << ' '; break;
        case item::type::packet:
            std::cout << '(';
            for (int j = 0; j < i.p.size(); j++) {
                print(i.p[j]);
            }
            std::cout << ')';
            break;
    }
}

int compare(item& left, item& right) {
    std::cout << "comparing: ";
    print(left);
    std::cout << " with ";
    print(right);
    std::cout << std::endl;
    if (left.typ == item::type::num && right.typ == item::type::num) {
        return left.num - right.num;
    } else if (left.typ == item::type::packet &&
               right.typ == item::type::packet) {
        return compare(left.p, right.p);
    } else if (left.typ == item::type::num) {
        packet p;
        p.push_back(left);
        return compare(p, right.p);
    } else {
        packet p;
        p.push_back(right);
        return compare(left.p, p);
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) die("usage: day13 <file>");
    auto packet_pairs = parse(std::ifstream(argv[1]));
    int sum = 0;
    for (int i = 0; i < packet_pairs.size(); i++) {
        std::cout << "pair " << i + 1 << std::endl;
        if (compare(packet_pairs[i].first, packet_pairs[i].second) < 0) {
            std::cout << "right order!\n";
            sum += i + 1;
        }
    }
    std::cout << sum << std::endl;
}
