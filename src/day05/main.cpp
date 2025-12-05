#include <aux.hpp>

using range = std::pair<long long, long long>;

std::string solve_1(const std::string& input) {
    const auto input_split = split_view(input, "\n\n");

    const auto id_ranges_str = split_lines_view(input_split[0]);
    std::vector<range> id_ranges(id_ranges_str.size());
    std::transform(id_ranges_str.cbegin(), id_ranges_str.cend(),
                   id_ranges.begin(), [](auto id_range_str) {
                       const auto id_range = split_view(id_range_str, "-");

                       return range{
                           parse_from_substr<long long>(id_range[0]),
                           parse_from_substr<long long>(id_range[1]),
                       };
                   });

    const auto ids_str = split_lines_view(input_split[1]);

    auto ans = 0;
    for (size_t i = 0; i < ids_str.size(); ++i) {
        const auto id = parse_from_substr<long long>(ids_str[i]);
        for (size_t j = 0; j < id_ranges.size(); ++j) {
            if (id_ranges[j].first <= id && id <= id_ranges[j].second) {
                ans += 1;

                break;
            }
        }
    }

    return std::to_string(ans);
}

std::string solve_2(const std::string& input) {
    const auto input_split = split_view(input, "\n\n");

    const auto id_ranges_str = split_lines_view(input_split[0]);
    std::vector<range> id_ranges(id_ranges_str.size());
    std::transform(id_ranges_str.cbegin(), id_ranges_str.cend(),
                   id_ranges.begin(), [](auto id_range_str) {
                       const auto id_range = split_view(id_range_str, "-");

                       return range{
                           parse_from_substr<long long>(id_range[0]),
                           parse_from_substr<long long>(id_range[1]),
                       };
                   });

    std::sort(id_ranges.begin(), id_ranges.end());

    long long start = 0;
    long long ans = 0;
    for (size_t i = 0; i < id_ranges.size(); ++i) {
        start = std::max(start, id_ranges[i].first);
        ans += std::max((long long)0, id_ranges[i].second - start + 1);
        start = std::max(start, id_ranges[i].second + 1);
    }

    return std::to_string(ans);
}

int run_tests() {
    using namespace std;

    assert(solve_1(readFile(day_path("day05", "in_small.txt"))) == "3");
    assert(solve_1(readFile(day_path("day05", "in.txt"))) == "735");

    assert(solve_2(readFile(day_path("day05", "in_small.txt"))) == "14");
    assert(solve_2(readFile(day_path("day05", "in.txt"))) == "344306344403172");

    cerr << "All tests passed\n";

    return 0;
}

int main(int argc, char** argv) {
#ifdef TESTING
    (void)argc;
    (void)argv;

    return run_tests();
#else
    using namespace std;

    if (argc < 3) {
        cerr << "Usage: ./dayXX <part(1|2)> <input_file>\n";

        return 1;
    }

    const auto input = readFile(day_path("day05", argv[2]));
    switch (stoi(argv[1])) {
        case 1:
            cout << solve_1(input) << endl;

            break;
        case 2:
            cout << solve_2(input) << endl;

            break;
        default:
            cerr << "Invalid part. Expected 1 or 2.\n";

            return 1;
    }

    return 0;
#endif
}