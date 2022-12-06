#include <fstream>
#include <iostream>
#include <iterator>
#include <list>
#include <map>

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

    friend std::ostream& operator<<(std::ostream& os,
                                    const windowed_set& window) {
        std::cout << "window";
        os << '[';
        for (const auto& t : window.window_) std::cout << ' ' << t;
        os << " ]";
        std::cout << "set";
        os << '[';
        for (const auto& t : window.ts_)
            std::cout << ' ' << t.first << '(' << t.second << ')';
        os << " ]";
        return os;
    }

private:
    std::map<T, int> ts_;
    std::list<T> window_;
    const int max_;
};

int main(int argc, char* argv[]) {
    if (argc != 2) die("usage: day6 <file>");
    std::ifstream ifs(argv[1]);
    windowed_set<char> window(4);
    int i = 0;
    for (auto it = std::istream_iterator<char>(ifs);
         it != std::istream_iterator<char>() && window.distinct() < 4;
         it++, i++) {
        window.push(*it);
    }
    std::cout << i << std::endl;
}
