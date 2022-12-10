#include <cinttypes>
#include <cstring>
#include <fstream>
#include <iostream>
#include <iterator>
#include <limits>
#include <map>
#include <memory>
#include <numeric>
#include <regex>
#include <string>
#include <vector>

#include "util.h"

struct dir_entry {
    std::map<std::string, int64_t> files;
    std::map<std::string, std::unique_ptr<dir_entry>> dirs;
    int64_t size;
};

void eat_line(std::istream& is, const std::regex& pat, std::smatch& m,
              std::string& buf) {
    if (!std::getline(is, buf)) die(strerror(errno));
    if (!std::regex_match(buf, m, pat)) die("failed match: " + buf);
}

std::string get_match(const std::smatch& m, size_t n) {
    return std::string(m[n].first, m[n].second);
}

std::unique_ptr<dir_entry> parse_dir(std::istream& is) {
    auto dir = std::make_unique<dir_entry>();
    std::string line;
    std::smatch m;

    // always expect ls when entering a new directory
    static const std::regex ls_pat(R"(\$ ls)");
    eat_line(is, ls_pat, m, line);

    // read ls output
    while (is.peek() != '$' && !is.eof()) {
        if (!std::getline(is, line)) die("can't read file");
        static const std::regex file_pat(R"((\d+) ((\.|\w)+))");
        static const std::regex dir_pat(R"(dir (\w+))");
        if (std::regex_match(line, m, dir_pat)) {
            dir->dirs.insert({get_match(m, 1), std::make_unique<dir_entry>()});
        } else if (std::regex_match(line, m, file_pat)) {
            dir->files.insert({get_match(m, 2), std::stoll(get_match(m, 1))});
        } else {
            die("bad file: " + line);
        }
    }

    // process directories recursively
    while (!is.eof()) {
        static const std::regex cd_pat(R"(\$ cd ((\w+)|(\.\.)))");
        eat_line(is, cd_pat, m, line);
        if (m[2].matched) dir->dirs.at(get_match(m, 2)) = parse_dir(is);
        else if (m[3].matched) break;
        else die("unreachable");
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
