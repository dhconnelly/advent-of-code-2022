#include <cinttypes>
#include <fstream>
#include <iostream>
#include <iterator>
#include <limits>
#include <map>
#include <memory>
#include <numeric>
#include <vector>

#include "util.h"

struct dir_entry {
    std::map<std::string, int64_t> files;
    std::map<std::string, std::unique_ptr<dir_entry>> dirs;
    int64_t size;
};

std::unique_ptr<dir_entry> parse_dir(std::istream& is) {
    // TODO: figure out the std::regex api
    auto dir = std::make_unique<dir_entry>();
    std::string line;
    if (!std::getline(is, line) || line != "$ ls") die("expected ls: " + line);
    while (is.peek() != '$' && !is.eof()) {
        if (!std::getline(is, line)) die("expected file");
        if (line[0] == 'd') {
            dir->dirs.insert({line.substr(4), std::make_unique<dir_entry>()});
        } else {
            int sep = line.find(' ');
            int64_t size = std::stoll(line.substr(0, sep));
            auto name = line.substr(sep + 1);
            dir->files.insert({name, size});
        }
    }
    while (std::getline(is, line)) {
        if (line.find("$ cd") != 0) die("expected cd: " + line);
        if (line[5] == '.') {
            if (line[6] != '.') die("bad cd");
            break;
        } else {
            auto name = line.substr(5);
            dir->dirs.at(name) = parse_dir(is);
        }
    }
    return dir;
}

std::unique_ptr<dir_entry> parse_filesystem(std::istream& is) {
    if (std::string root; !std::getline(is, root) || root != "$ cd /")
        die("bad filesystem");
    return parse_dir(is);
}

int64_t find_sum_smaller(dir_entry& dir, int64_t max) {
    int64_t files_size = std::accumulate(
        dir.files.begin(), dir.files.end(), 0,
        [](int64_t sum, const auto& file) { return sum + file.second; });
    auto [dirs_size, sizes_smaller] = std::accumulate(
        dir.dirs.begin(), dir.dirs.end(), std::pair{0, 0},
        [&](auto sums, const auto& dir) {
            auto subdir_smaller = find_sum_smaller(*dir.second, max);
            return std::pair{sums.first + dir.second->size,
                             sums.second + subdir_smaller};
        });
    dir.size = files_size + dirs_size;
    if (dir.size <= max) sizes_smaller += dir.size;
    return sizes_smaller;
}

int64_t min_del(const dir_entry& dir, int64_t free, int64_t needed) {
    static const int64_t kMaxInt = std::numeric_limits<int64_t>::max();
    int64_t deletion = ((free + dir.size) >= needed) ? dir.size : kMaxInt;
    for (const auto& [_, subdir] : dir.dirs) {
        deletion = std::min(deletion, min_del(*subdir, free, needed));
    }
    return deletion;
}

int main(int argc, char* argv[]) {
    if (argc != 2) die("usage: day7 <file>");
    std::ifstream ifs(argv[1]);
    auto root = parse_filesystem(ifs);
    std::cout << find_sum_smaller(*root, 100000) << std::endl;
    std::cout << min_del(*root, 70000000 - root->size, 30000000) << std::endl;
}
