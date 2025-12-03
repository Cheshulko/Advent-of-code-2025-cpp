#include <aux.hpp>

std::string solve_1(const std::string& input) {
    const auto banks = split_lines_view(input);

    int ans = 0;
    for (const auto& bank : banks) {
        std::vector<std::vector<int>> indxs(10, std::vector<int>());

        for (size_t i = 0; i < bank.size(); ++i) {
            indxs[bank[i] - '0'].push_back(i);
        }

        int best = 0;
        for (int d1 = 9; d1 >= 1; --d1) {
            for (int d2 = 9; d2 >= 0; --d2) {
                if (indxs[d1].empty() || indxs[d2].empty()) {
                    continue;
                }

                const int i = indxs[d1].front();
                const int j = indxs[d2].back();
                if (i >= j) {
                    continue;
                }

                best = std::max(best, d1 * 10 + d2);
            }
        }

        ans += best;
    }

    return std::to_string(ans);
}

std::string solve_2(const std::string& input) {
    using mem_map_t = std::map<std::pair<int, int>, long long>;

    auto count = [](auto self, mem_map_t& dp, size_t ind, size_t len,
                    std::string_view s) -> std::optional<long long> {
        if (len == 0) {
            return 0;
        }
        if (dp.cend() != dp.find({ind, len})) {
            return dp[{ind, len}];
        }

        long long ans = -1;
        for (int i = ind; i >= 0; --i) {
            if (const auto r = self(self, dp, i - 1, len - 1, s); r) {
                ans = std::max(ans, (*r) * 10 + (s[i] - '0'));
            }
        }
        if (ans == -1) {
            return std::nullopt;
        } else {
            return dp[{ind, len}] = ans;
        }
    };

    const auto banks = split_lines_view(input);

    long long ans = 0;
    for (const auto& bank : banks) {
        mem_map_t dp;
        const auto best = count(count, dp, bank.size() - 1, 12, bank);
        assert(best);

        ans += *best;
    }

    return std::to_string(ans);
}

int run_tests() {
    using namespace std;

    assert(solve_1(readFile(day_path("day03", "in_small.txt"))) == "357");
    assert(solve_1(readFile(day_path("day03", "in.txt"))) == "16946");

    assert(solve_2(readFile(day_path("day03", "in_small.txt"))) ==
           "3121910778619");
    assert(solve_2(readFile(day_path("day03", "in.txt"))) == "168627047606506");

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

    const auto input = readFile(day_path("day03", argv[2]));
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