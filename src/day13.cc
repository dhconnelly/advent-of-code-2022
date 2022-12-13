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

    friend bool operator==(const item& left, const item& right) {
        switch (left.typ) {
            case item::type::num:
                return right.typ == item::type::num && left.num == right.num;
            case item::type::packet:
                if (right.typ != item::type::packet) return false;
                return left.p == right.p;
        }
    }
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

int compare(const item& left, const item& right);
int compare(const packet& left, const packet& right) {
    int i;
    for (i = 0; i < left.size() && i < right.size(); i++) {
        if (int d = compare(left[i], right[i]); d < 0) return -1;
        else if (d > 0) return 1;
    }
    if (i == left.size() && i < right.size()) return -1;
    if (i == right.size() && i < left.size()) return 1;
    return 0;
}

int compare(const item& left, const item& right) {
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

std::vector<packet> combine_packets(
    const std::vector<std::pair<packet, packet>>& pairs) {
    std::vector<packet> ps;
    for (const auto& [left, right] : pairs) {
        ps.push_back(left);
        ps.push_back(right);
    }
    return ps;
}

int main(int argc, char* argv[]) {
    if (argc != 2) die("usage: day13 <file>");
    auto packet_pairs = parse(std::ifstream(argv[1]));

    // part 1
    int sum = 0;
    for (int i = 0; i < packet_pairs.size(); i++) {
        const auto& [left, right] = packet_pairs[i];
        if (compare(left, right) < 0) sum += i + 1;
    }
    std::cout << sum << std::endl;

    // part 2
    auto all = combine_packets(packet_pairs);
    auto div1 = item::from(item::from(2).to_packet()).to_packet();
    auto div2 = item::from(item::from(6).to_packet()).to_packet();
    all.push_back(div1);
    all.push_back(div2);
    std::sort(all.begin(), all.end(),
              [](auto& l, auto& r) { return compare(l, r) < 0; });
    auto i = std::find(all.begin(), all.end(), div1) - all.begin() + 1;
    auto j = std::find(all.begin(), all.end(), div2) - all.begin() + 1;
    std::cout << (i * j) << std::endl;
}
