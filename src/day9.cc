#include <iostream>
#include <iterator>
#include <fstream>
#include <numeric>
#include <utility>
#include <set>
#include <vector>

#include "util.h"

using pt2 = std::pair<int, int>;

bool touching(pt2 a, pt2 b) {
  return std::abs(a.first - b.first) <= 1 && std::abs(a.second - b.second) <= 1;
}

bool in_line(pt2 a, pt2 b) {
  return a.first == b.first || a.second == b.second;
}

int step_dist(pt2 a, pt2 b) {
  return std::abs(a.first - b.first) + std::abs(a.second - b.second);
}

pt2 unit_vec(pt2 from, pt2 to) {
  pt2 vec{to.first - from.first, to.second - from.second};
  if (vec.first != 0) vec.first = vec.first / std::abs(vec.first);
  if (vec.second != 0) vec.second = vec.second / std::abs(vec.second);
  if (vec.first != 0 && vec.second != 0) die("unreachable");
  return vec;
}

pt2 add(pt2 a, pt2 b) {
  return {a.first + b.first, a.second + b.second};
}


pt2 move_tail(pt2 cur_tail, pt2 head) {
  static constexpr pt2 kNbrDirs[] = {{0, 1}, {0, -1}, {1, 0}, {-1, 0}};
  int d = step_dist(cur_tail, head);
  if (touching(cur_tail, head)) {
    return cur_tail;
  } else if (in_line(cur_tail, head) && d == 2) {
    return add(cur_tail, unit_vec(cur_tail, head));
  } else {
    // assume the rest is right and step diagonally
    for (int i = 0; i < 4; i++) {
      pt2 nbr = add(head, kNbrDirs[i]);
      if (touching(nbr, cur_tail)) return nbr;
    }
  }
  die("unreachable");
}

pt2 move_head(pt2 cur_head, char dir) {
  switch (dir) {
    case 'U': cur_head.first++; break;
    case 'R': cur_head.second++; break;
    case 'D': cur_head.first--; break;
    case 'L': cur_head.second--; break;
    default: die("bad direction");
  }
  return cur_head;
}

struct instr : std::pair<char, int> {
  using std::pair<char, int>::pair;
};

std::istream& operator>>(std::istream& is, instr& i) {
  if (!(is >> i.first)) return is;
  if (!(is >> i.second)) return is;
  return is;
}

std::set<pt2> move_rope(const std::vector<instr>& instrs) {
  pt2 head, tail;
  std::set<pt2> v;
  v.insert(tail);
  for (const auto& [dir, dist] : instrs) {
    for (int i = 0; i < dist; i++) {
      head = move_head(head, dir);
      tail = move_tail(tail, head);
      v.insert(tail);
    }
  }
  return v;
}

int main(int argc, char* argv[]) {
  if (argc != 2) die("usage: day9 <file>");
  std::ifstream ifs(argv[1]);
  std::vector<instr> instrs(
      (std::istream_iterator<instr>(ifs)),
      (std::istream_iterator<instr>()));
  auto v = move_rope(instrs);
  std::cout << v.size() << std::endl;
}

