#include <algorithm>
#include <cassert>
#include <charconv>
#include <fstream>
#include <iostream>
#include <map>
#include <queue>
#include <regex>
#include <set>
#include <string>
#include <string_view>

std::string day_path(const std::string& day, const std::string& input) {
    using std::filesystem::path;

    const auto p = path("src") / day / path("input") / input;

    return p.string();
}

std::string readFile(const std::string& path) {
    std::ifstream in(path, std::ios::in | std::ios::binary);
    if (!in) {
        throw std::runtime_error("Failed to open file: " + path);
    }

    std::string contents((std::istreambuf_iterator<char>(in)),
                         std::istreambuf_iterator<char>());
    if (in.bad()) {
        throw std::runtime_error("I/O error while reading file: " + path);
    }

    return contents;
}

std::vector<std::string_view> split_view(std::string_view s,
                                         std::string_view delimiter) {
    std::vector<std::string_view> result;
    std::size_t start = 0;
    while (true) {
        std::size_t pos = s.find(delimiter, start);
        if (pos == std::string_view::npos) {
            result.emplace_back(s.substr(start));

            break;
        }
        result.emplace_back(s.substr(start, pos - start));
        start = pos + delimiter.size();
    }

    return result;
}

std::vector<std::string_view> split_lines_view(std::string_view s) {
    return split_view(s, "\n");
}

template <typename T>
T parse_from_substr(std::string_view s,
                    size_t pos = 0,
                    size_t len = std::numeric_limits<size_t>::max()) {
    assert(pos < s.size());
    s = s.substr(pos, std::min(len, s.size() - pos));

    T value = 0;
    auto r = std::from_chars(s.data(), s.data() + s.size(), value);
    assert(r.ec == std::errc());

    return value;
}