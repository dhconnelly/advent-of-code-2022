#include <cstring>
#include <fstream>
#include <functional>
#include <iostream>
#include <istream>
#include <numeric>
#include <string>
#include <string_view>
#include <vector>

#include "util.h"

using Round = std::pair<char, char>;

std::vector<Round> parse_rounds(std::istream& is) {
    if (!is.good()) die(strerror(errno));
    std::vector<Round> rounds;
    std::string line;
    while (std::getline(is, line)) rounds.emplace_back(line[0], line[2]);
    if (is.bad()) die(strerror(errno));
    return rounds;
}

int score_hand(char hand) {
    switch (hand) {
        case 'A': return 1;
        case 'B': return 2;
        case 'C': return 3;
    }
    die("invalid hand");
}

char select_hand1(const Round& round) {
    switch (round.second) {
        case 'X': return 'A';
        case 'Y': return 'B';
        case 'Z': return 'C';
    }
    die("invalid play");
}

char select_hand2(const Round& round) {
    char outcome = round.second;
    if (outcome == 'Y') return round.first;
    switch (round.first) {
        case 'A': return outcome == 'X' ? 'C' : 'B';
        case 'B': return outcome == 'X' ? 'A' : 'C';
        case 'C': return outcome == 'X' ? 'B' : 'A';
    }
    die("invalid outcome");
}

int outcome(char enemy_hand, char hand) {
    if (enemy_hand == hand) return 3;
    switch (hand) {
        case 'A': return enemy_hand == 'C' ? 6 : 0;
        case 'B': return enemy_hand == 'A' ? 6 : 0;
        case 'C': return enemy_hand == 'B' ? 6 : 0;
    }
    die("invalid hands");
}

int score_game(const std::vector<Round>& rounds,
               std::function<char(const Round&)> select_hand) {
    auto score_round = [&](const Round& round) {
        int hand1 = round.first;
        int hand2 = select_hand(round);
        return score_hand(hand2) + outcome(hand1, hand2);
    };
    return std::accumulate(
        rounds.begin(), rounds.end(), 0,
        [&](int sum, const Round& round) { return sum + score_round(round); });
}

int main(int argc, char* argv[]) {
    if (argc != 2) die("usage: day2 <input>");
    std::ifstream ifs(argv[1]);
    auto rounds = parse_rounds(ifs);
    std::cout << score_game(rounds, select_hand1) << std::endl;
    std::cout << score_game(rounds, select_hand2) << std::endl;
}
