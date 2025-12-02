#include <aux.hpp>

constexpr auto MAX = 100;

std::string solve_1(const std::string& input) {
    const auto lines = split_lines_view(input);

    auto pos = 50;
    auto answer = 0;
    for (const auto& l : lines) {
        const auto type = l[0];
        assert(type == 'L' || type == 'R');

        const auto value = parse_from_substr<int>(l, 1) % MAX;
        if (type == 'L') {
            pos = (pos + MAX - value) % MAX;
        } else {
            pos = (pos + value) % MAX;
        }

        answer += (pos == 0);
    }

    return std::to_string(answer);
}

std::string solve_2(const std::string& input) {
    const auto lines = split_lines_view(input);

    auto pos = 50;
    auto answer = 0;
    for (const auto& l : lines) {
        const auto type = l[0];
        assert(type == 'L' || type == 'R');

        const auto value_rep = parse_from_substr<int>(l, 1);
        answer += value_rep / MAX;

        const auto value = value_rep % MAX;
        if (type == 'L') {
            if (value >= pos && pos > 0) {
                answer += 1;
            }
            pos = (pos + MAX - value) % MAX;
        } else {
            if (pos + value >= MAX) {
                answer += 1;
            }
            pos = (pos + value) % MAX;
        }
    }

    return std::to_string(answer);
}

int run_tests() {
    using namespace std;

    assert(solve_1(readFile(day_path("day01", "in_small.txt"))) == "3");
    assert(solve_1(readFile(day_path("day01", "in.txt"))) == "1195");

    assert(solve_2(readFile(day_path("day01", "in_small.txt"))) == "6");
    assert(solve_2(readFile(day_path("day01", "in.txt"))) == "6770");

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

    const auto input = readFile(day_path("day01", argv[2]));
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