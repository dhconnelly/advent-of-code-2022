#include <fstream>
#include <iostream>
#include <iterator>
#include <list>
#include <map>
#include <vector>

#include "util.h"

template <typename T>
class windowed_set {
public:
    windowed_set(int max) : max_(max) {}
    int size() const { return window_.size(); }
    int distinct() const { return ts_.size(); }
    void push(T t) {
        window_.push_back(t);
        ts_[t]++;
        if (window_.size() > max_) {
            ts_[window_.front()]--;
            if (ts_[window_.front()] == 0) ts_.erase(window_.front());
            window_.pop_front();
        }
    }

private:
    std::map<T, int> ts_;
    std::list<T> window_;
    const int max_;
};

int find_marker(const std::vector<char>& chars, int max) {
    windowed_set<char> window(max);
    int i = 0;
    for (auto it = chars.begin(); it != chars.end() && window.distinct() < max;
         it++, i++) {
        window.push(*it);
    }
    return i;
}

int main(int argc, char* argv[]) {
    if (argc != 2) die("usage: day6 <file>");
    std::ifstream ifs(argv[1]);
    std::vector<char> chars((std::istream_iterator<char>(ifs)),
                            std::istream_iterator<char>());
    std::cout << find_marker(chars, 4) << std::endl;
    std::cout << find_marker(chars, 14) << std::endl;
}
