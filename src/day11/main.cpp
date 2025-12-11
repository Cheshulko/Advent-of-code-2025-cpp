#include <aux.hpp>

template <size_t const N>
std::array<long long, 1 << N> dfs(
    int p,
    size_t cur,
    size_t target,
    const std::array<size_t, N>& required,
    const std::vector<std::vector<size_t>>& connections,
    std::vector<std::optional<std::array<long long, 1 << N>>>& mem) {
    using state = std::array<long long, 1 << N>;

    if (cur == target) {
        auto ans = state{0};
        ans[0] = 1;

        return ans;
    }
    if (const auto cur_mem = mem[cur]; cur_mem) {
        return *cur_mem;
    }

    size_t cur_required = 0;
    for (size_t i = 0; i < N; ++i) {
        if (cur == required[i]) {
            cur_required |= (1 << i);
        }
    }

    auto children_ways = state{0};
    for (const auto to : connections[cur]) {
        if ((int)to == p) {
            continue;
        }

        const auto ways =
            dfs<N>((int)cur, to, target, required, connections, mem);

        for (size_t i = 0; i < (1 << N); ++i) {
            children_ways[i] += ways[i];
        }
    }

    auto cur_ways = state{0};
    for (size_t i = 0; i < (1 << N); ++i) {
        cur_ways[i | cur_required] += children_ways[i];
    }

    mem[cur] = cur_ways;

    return cur_ways;
}

template <size_t const N>
std::array<long long, 1 << N> solve(std::array<std::string, N> required_str,
                                    const std::string& start,
                                    const std::string& end,
                                    const std::string& input) {
    const auto rows = split_lines_view(input);

    std::map<std::string_view, size_t> indx;
    auto insert = [&](std::string_view s) {
        if (indx.cend() == indx.find(s)) {
            return indx[s] = indx.size();
        } else {
            return indx[s];
        }
    };

    std::vector<std::vector<size_t>> connections;

    for (const auto line : rows) {
        const auto split = split_view(line, ": ");

        const auto from = insert(split[0]);

        while (connections.size() <= from) {
            connections.push_back({});
        }

        for (const auto to : split_view(split[1], " ")) {
            const auto i = insert(to);

            connections[from].push_back(i);
        };
    }

    std::vector<std::optional<std::array<long long, 1 << N>>> mem(
        connections.size(), std::nullopt);

    std::array<size_t, N> required;
    for (size_t i = 0; i < N; ++i) {
        required[i] = indx[required_str[i]];
    }

    return dfs<N>(-1, indx[start], indx[end], required, connections, mem);
}

std::string solve_1(const std::string& input) {
    const size_t N = 0;

    const auto ans = solve<N>({}, "you", "out", input);

    return std::to_string(ans[(1 << N) - 1]);
}

std::string solve_2(const std::string& input) {
    const size_t N = 2;

    const auto ans = solve<N>({"dac", "fft"}, "svr", "out", input);

    return std::to_string(ans[(1 << N) - 1]);
}

int run_tests() {
    using namespace std;

    assert(solve_1(readFile(day_path("day11", "in_small.txt"))) == "5");
    assert(solve_1(readFile(day_path("day11", "in.txt"))) == "724");

    assert(solve_2(readFile(day_path("day11", "in_small_2.txt"))) == "2");
    assert(solve_2(readFile(day_path("day11", "in.txt"))) == "473930047491888");

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

    const auto input = readFile(day_path("day11", argv[2]));
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