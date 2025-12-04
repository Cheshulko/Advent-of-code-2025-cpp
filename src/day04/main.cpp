#include <aux.hpp>

std::string solve_1(const std::string& input) {
    const auto grid = split_lines_view(input);
    const int n = grid.size();
    const int m = grid[0].size();

    int ans = 0;
    for (auto i = 0; i < n; ++i) {
        for (auto j = 0; j < m; ++j) {
            if (grid[i][j] != '@') {
                continue;
            }

            auto cnt = 0;
            for (int di = -1; di <= 1; ++di) {
                for (int dj = -1; dj <= 1; ++dj) {
                    const auto ti = i + di;
                    const auto tj = j + dj;

                    if (ti >= 0 && ti < n && tj >= 0 && tj < m) {
                        cnt += grid[ti][tj] == '@';
                    }
                }
            }
            if (cnt < 4 + 1 /*itself*/) {
                ans += 1;
            }
        }
    }

    return std::to_string(ans);
}

template <typename T>
using grid_t = std::vector<std::vector<T>>;
using point_t = std::pair<int, int>;

std::string solve_2(const std::string& input) {
    const auto grid = split_lines_view(input);
    const int n = grid.size();
    const int m = grid[0].size();

    std::deque<point_t> q;
    grid_t<bool> seen(n, std::vector<bool>(m, false));
    grid_t<int> counts(n, std::vector<int>(m, 0));

    for (auto i = 0; i < n; ++i) {
        for (auto j = 0; j < m; ++j) {
            if (grid[i][j] != '@') {
                counts[i][j] = -1;

                continue;
            }

            auto cnt = 0;
            for (int di = -1; di <= 1; ++di) {
                for (int dj = -1; dj <= 1; ++dj) {
                    const auto ti = i + di;
                    const auto tj = j + dj;

                    if (ti >= 0 && ti < n && tj >= 0 && tj < m) {
                        cnt += grid[ti][tj] == '@';
                    }
                }
            }

            counts[i][j] = cnt - 1;

            if (counts[i][j] < 4) {
                seen[i][j] = true;
                q.push_back({i, j});
            }
        }
    }

    int ans = 0;
    while (!q.empty()) {
        const auto [i, j] = q.front();
        q.pop_front();

        assert(grid[i][j] == '@');
        assert(counts[i][j] < 4);

        ans += 1;

        for (int di = -1; di <= 1; ++di) {
            for (int dj = -1; dj <= 1; ++dj) {
                const auto ti = i + di;
                const auto tj = j + dj;

                if (ti >= 0 && ti < n && tj >= 0 && tj < m) {
                    if (grid[ti][tj] == '@') {
                        counts[ti][tj] -= 1;

                        if (counts[ti][tj] < 4 && !seen[ti][tj]) {
                            seen[ti][tj] = true;
                            q.push_back({ti, tj});
                        }
                    }
                }
            }
        }
    }

    return std::to_string(ans);
}

int run_tests() {
    using namespace std;

    assert(solve_1(readFile(day_path("day04", "in_small.txt"))) == "13");
    assert(solve_1(readFile(day_path("day04", "in.txt"))) == "1367");

    assert(solve_2(readFile(day_path("day04", "in_small.txt"))) == "43");
    assert(solve_2(readFile(day_path("day04", "in.txt"))) == "9144");

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

    const auto input = readFile(day_path("day04", argv[2]));
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