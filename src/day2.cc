#include <cstring>
#include <fstream>
#include <iostream>
#include <istream>
#include <numeric>
#include <string>
#include <string_view>
#include <vector>

[[noreturn]] void die(const std::string_view msg) {
    std::cerr << msg << std::endl;
    std::exit(1);
}

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

char play_to_hand(char play) {
    switch (play) {
        case 'X': return 'A';
        case 'Y': return 'B';
        case 'Z': return 'C';
    }
    die("invalid play");
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

int score_round(const Round& round) {
    int hand1 = round.first;
    int hand2 = play_to_hand(round.second);
    return score_hand(hand2) + outcome(hand1, hand2);
}

int main(int argc, char* argv[]) {
    if (argc != 2) die("usage: day2 <input>");
    std::ifstream ifs(argv[1]);
    auto rounds = parse_rounds(ifs);
    std::vector<int> scores;
    std::transform(rounds.begin(), rounds.end(), std::back_inserter(scores),
                   score_round);
    std::cout << std::accumulate(scores.begin(), scores.end(), 0) << std::endl;
}
