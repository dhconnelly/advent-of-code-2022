#include <cinttypes>
#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
#include <memory>
#include <numeric>
#include <vector>

#include "util.h"

struct dir_entry {
    std::map<std::string, int64_t> files;
    std::map<std::string, std::unique_ptr<dir_entry>> dirs;
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
    std::string root;
    if (!std::getline(is, root) || root != "$ cd /") die("bad filesystem");
    return parse_dir(is);
}

std::pair<int64_t, int64_t> sum_total_sizes(const dir_entry& dir, int64_t max) {
    int64_t files_size = std::accumulate(
        dir.files.begin(), dir.files.end(), 0,
        [](int64_t sum, const auto& file) { return sum + file.second; });
    int64_t sizes_smaller = 0;
    int64_t dirs_size = std::accumulate(
        dir.dirs.begin(), dir.dirs.end(), 0, [&](int64_t sum, const auto& dir) {
            auto [subdir, subdir_smaller] = sum_total_sizes(*dir.second, max);
            sizes_smaller += subdir_smaller;
            return sum + subdir;
        });
    int64_t total_size = files_size + dirs_size;
    if (total_size <= max) sizes_smaller += total_size;
    return {total_size, sizes_smaller};
}

int main(int argc, char* argv[]) {
    if (argc != 2) die("usage: day7 <file>");
    std::ifstream ifs(argv[1]);
    auto root = parse_filesystem(ifs);
    std::cout << sum_total_sizes(*root, 100000).second << std::endl;
}
