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
    packet packet;
};

packet parse_packet(std::istream& is);
item parse_item(std::istream& is) {
    if (is.peek() == '[') {
        return item{.typ = item::type::packet, .packet = parse_packet(is)};
    } else {
        item i{.typ = item::type::num};
        is >> i.num;
        return i;
    }
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

int compare(const packet& left, const packet& right) { return 1; }

int main(int argc, char* argv[]) {
    if (argc != 2) die("usage: day13 <file>");
    auto packet_pairs = parse(std::ifstream(argv[1]));
    int sum = 0;
    for (int i = 1; i <= packet_pairs.size(); i++) {
        if (compare(packet_pairs[i].first, packet_pairs[i].second) == 0) {
            sum += i;
        }
    }
    std::cout << sum << std::endl;
}
