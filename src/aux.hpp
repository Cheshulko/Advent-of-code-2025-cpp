#include <cassert>
#include <charconv>
#include <fstream>
#include <iostream>
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

std::vector<std::string_view> split_lines_view(std::string_view s) {
    std::vector<std::string_view> lines;
    size_t start = 0;
    while (start < s.size()) {
        auto pos = s.find_first_of("\n", start);
        if (pos == std::string_view::npos) {
            lines.emplace_back(s.substr(start));

            break;
        }
        lines.emplace_back(s.substr(start, pos - start));
        start = pos + 1;
    }

    return lines;
}

int parse_int_from_substr(std::string_view s, size_t pos, size_t len) {
    assert(pos < s.size());
    s = s.substr(pos, std::min(len, s.size() - pos));

    auto value = 0;
    auto r = std::from_chars(s.data(), s.data() + s.size(), value);
    assert(r.ec == std::errc());

    return value;
}