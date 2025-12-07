#include <aux.hpp>

std::string solve_1(const std::string& input) {
    const auto rows = split_lines_view(input);
    const auto n = rows.size();
    const auto m = rows[0].size();

    std::array<std::vector<bool>, 2> beam;
    beam[0].assign(m, false);
    beam[1].assign(m, false);

    for (size_t j = 0; j < m; ++j) {
        if (rows[0][j] == 'S') {
            beam[0][j] = true;

            break;
        }
    }

    int ans = 0;
    for (size_t i = 1; i < n; ++i) {
        beam[1].assign(m, false);

        for (size_t j = 0; j < m; ++j) {
            if (!beam[0][j]) {
                continue;
            }
            if (rows[i][j] == '^') {
                ans += 1;
                if (j > 0) {
                    beam[1][j - 1] = true;
                }
                if (j + 1 < m) {
                    beam[1][j + 1] = true;
                }

            } else {
                beam[1][j] = true;
            }
        }

        std::swap(beam[0], beam[1]);
    }

    return std::to_string(ans);
}

std::string solve_2(const std::string& input) {
    const auto rows = split_lines_view(input);
    const auto n = rows.size();
    const auto m = rows[0].size();

    std::array<std::vector<long long>, 2> beam;
    beam[0].assign(m, 0);
    beam[1].assign(m, 0);

    for (size_t j = 0; j < m; ++j) {
        if (rows[0][j] == 'S') {
            beam[0][j] = 1;

            break;
        }
    }

    for (size_t i = 1; i < n; ++i) {
        beam[1].assign(m, 0);

        for (size_t j = 0; j < m; ++j) {
            if (rows[i][j] == '^') {
                if (j > 0) {
                    beam[1][j - 1] += beam[0][j];
                }
                if (j + 1 < m) {
                    beam[1][j + 1] += beam[0][j];
                }
            } else {
                beam[1][j] += beam[0][j];
            }
        }

        std::swap(beam[0], beam[1]);
    }

    const auto ans =
        std::accumulate(beam[0].cbegin(), beam[0].cend(), (long long)0);

    return std::to_string(ans);
}

int run_tests() {
    using namespace std;

    assert(solve_1(readFile(day_path("day07", "in_small.txt"))) == "21");
    assert(solve_1(readFile(day_path("day07", "in.txt"))) == "1541");

    assert(solve_2(readFile(day_path("day07", "in_small.txt"))) == "40");
    assert(solve_2(readFile(day_path("day07", "in.txt"))) == "80158285728929");

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

    const auto input = readFile(day_path("day07", argv[2]));
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