#include <aux.hpp>

std::string solve_1(const std::string& input) {
    const auto lines = split_lines_view(input);
    const auto n = lines.size();
    const std::regex number_re(R"(\d+)");
    const std::regex op_re(R"(\*|\+)");

    std::vector<std::vector<long long>> grid(n - 1, std::vector<long long>());

    for (size_t i = 0; i < n - 1; ++i) {
        const auto s = lines[i];
        std::cregex_iterator it(s.data(), s.data() + s.size(), number_re);
        std::cregex_iterator end;

        for (; it != end; ++it) {
            grid[i].push_back(parse_from_substr<long long>(it->str()));
        }
    }

    long long ans = 0;
    {
        const auto s = lines[n - 1];
        std::cregex_iterator it(s.data(), s.data() + s.size(), op_re);
        std::cregex_iterator end;

        for (size_t i = 0; it != end; ++it, ++i) {
            const auto op = it->str();
            assert(op == "*" || op == "+");

            long long cur;
            if (op == "*") {
                cur = 1;
                for (const auto& row : grid) {
                    cur *= row[i];
                }
            } else {
                cur = 0;
                for (const auto& row : grid) {
                    cur += row[i];
                }
            }

            ans += cur;
        }
    }

    return std::to_string(ans);
}

std::string solve_2(const std::string& input) {
    const auto lines = split_lines_view(input);
    const auto n = lines.size();
    const auto m = lines[0].size();
    const std::regex op_re(R"(\*|\+)");

    enum class op_t { mul, add };
    std::vector<op_t> ops;
    {
        const auto s = lines[n - 1];
        std::cregex_iterator it(s.data(), s.data() + s.size(), op_re);
        std::cregex_iterator end;

        for (size_t i = 0; it != end; ++it, ++i) {
            const auto op = it->str();
            assert(op == "*" || op == "+");

            if (op == "*") {
                ops.push_back(op_t::mul);
            } else {
                ops.push_back(op_t::add);
            }
        }
    }

    long long ans = 0;
    size_t col = 0;
    size_t j = 0;
    size_t j_cur = j;
    while (col < ops.size()) {
        const auto any =
            std::any_of(lines.cbegin(), lines.cend(),
                        [=](const auto& line) { return line[j_cur] != ' '; });

        if (any && j_cur < m) {
            j_cur += 1;

            continue;
        }

        long long res = ops[col] == op_t::mul;
        for (int jj = j_cur - 1; jj >= (int)j; --jj) {
            long long num = 0;
            for (size_t ii = 0; ii < n - 1; ++ii) {
                if (lines[ii][(size_t)jj] != ' ') {
                    num = num * 10 + (lines[ii][(size_t)jj] - '0');
                } else {
                    if (num > 0) {
                        break;
                    }
                    num *= 10;
                }
            }

            if (ops[col] == op_t::mul) {
                res *= num;
            } else {
                res += num;
            }
        }
        j_cur = j = j_cur + 1;
        col += 1;
        ans += res;
    }

    return std::to_string(ans);
}

int run_tests() {
    using namespace std;

    assert(solve_1(readFile(day_path("day06", "in_small.txt"))) == "4277556");
    assert(solve_1(readFile(day_path("day06", "in.txt"))) == "4309240495780");

    assert(solve_2(readFile(day_path("day06", "in_small.txt"))) == "3263827");
    assert(solve_2(readFile(day_path("day06", "in.txt"))) == "9170286552289");

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

    const auto input = readFile(day_path("day06", argv[2]));
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