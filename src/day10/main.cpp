#include <aux.hpp>

typedef long double f64;

constexpr f64 EPS = 1e-9;

class Machine {
   public:
    static Machine parse_machine(const std::string_view line) {
        Machine result;

        // Extract the [...] part
        {
            const std::regex r_brackets(R"br(\[([^\]]*)\])br");
            std::match_results<std::string_view::const_iterator> m;

            const auto found =
                std::regex_search(line.begin(), line.end(), m, r_brackets);
            assert(found);

            int state = 0;
            const auto& s = m[1].str();
            const auto n = m[1].length();
            result.lights = n;
            for (ptrdiff_t i = 0; i < n; ++i) {
                if (s[i] == '#') {
                    state |= (1 << i);
                }
            }

            result.state = state;
        }

        // Extract all (...) parts
        {
            const std::regex r_paren(R"par(\(([^)]*)\))par");
            auto begin = std::regex_iterator(line.begin(), line.end(), r_paren);
            auto end = std::regex_iterator<std::string_view::const_iterator>();

            for (auto it = begin; it != end; ++it) {
                auto sub = (*it)[1];

                std::vector<int> nums;
                if (sub.first != sub.second) {
                    std::string inside(sub.first, sub.second);
                    std::stringstream ss(inside);
                    std::string token;

                    while (std::getline(ss, token, ',')) {
                        nums.push_back(std::stoi(token));
                    }
                }
                int button = 0;
                const auto n = nums.size();
                for (size_t i = 0; i < n; ++i) {
                    button |= (1 << nums[i]);
                };
                result.buttons.push_back(button);
            }
        }

        // Extract the {...} part
        {
            const std::regex r_brace(R"brace(\{([^}]*)\})brace");
            std::match_results<std::string_view::const_iterator> m;

            const auto found =
                std::regex_search(line.begin(), line.end(), m, r_brace);
            assert(found);

            auto sub = m[1];
            std::string inside(sub.first, sub.second);

            std::stringstream ss(inside);
            std::string token;
            while (std::getline(ss, token, ',')) {
                result.joltage.push_back(std::stoi(token));
            }
        }

        return result;
    }

    int solve() const {
        std::vector<int> seen(1 << lights, std::numeric_limits<int>::max());

        seen[0] = 0;
        std::deque<int> q;
        q.push_back(0);

        while (!q.empty()) {
            const auto cur_state = q.front();
            q.pop_front();

            if (cur_state == state) {
                break;
            }

            for (const auto& button : buttons) {
                const auto to_state = cur_state ^ button;

                if (std::numeric_limits<int>::max() == seen[to_state]) {
                    seen[to_state] = seen[cur_state] + 1;
                    q.push_back(to_state);
                }
            }
        }

        assert(seen[state] != std::numeric_limits<int>::max());

        return seen[state];
    }

    friend class Solver;

   private:
    int lights;
    int state;
    std::vector<int> buttons;
    std::vector<int> joltage;
};

std::string solve_1(const std::string& input) {
    const auto rows = split_lines_view(input);

    int ans = 0;
    for (const auto& line : rows) {
        const auto machine = Machine::parse_machine(line);

        ans += machine.solve();
    }

    return std::to_string(ans);
}

/// Gaussian Elimination
/// Solve a system of equations
/// (0 or 1) * x1 + (0 or 1) * x2 + (0 or 1) * x3 .. (0 or 1) * xn = a1;
/// (0 or 1) * x1 + (0 or 1) * x2 + (0 or 1) * x3 .. (0 or 1) * xn = a2;
/// (0 or 1) * x1 + (0 or 1) * x2 + (0 or 1) * x3 .. (0 or 1) * xn = a3;
/// ..
/// (0 or 1) * x1 + (0 or 1) * x2 + (0 or 1) * x3 .. (0 or 1) * xn = am;
/// x1 + x2 + x3 + .. + x4 -> min
class Solver {
   public:
    int solve(const Machine& machine) {
        const auto m = machine.joltage.size();

        std::vector<std::vector<int>> A(m, std::vector<int>());

        for (size_t b = 0; b < machine.buttons.size(); ++b) {
            const auto& button = machine.buttons[b];

            for (size_t light = 0; light < m; ++light) {
                A[light].push_back((button & (1 << light)) > 0);
            }
        }

        return run(A, machine.joltage);
    }

   private:
    int n, m;
    std::vector<std::vector<f64>> M;
    std::vector<int> pivot_col_for_row;
    std::vector<int> free_vars;
    std::vector<int> var_to_pivot_row;

    long long best_sum = -1;
    std::vector<int> best_sol;

    int run(const std::vector<std::vector<int>>& A, const std::vector<int>& b) {
        m = A.size();
        n = A[0].size();

        // 1. Build Augmented Matrix [A | b]
        M.assign(m, std::vector<f64>(n + 1));
        for (int i = 0; i < m; i++) {
            for (int j = 0; j < n; j++)
                M[i][j] = (f64)A[i][j];
            M[i][n] = (f64)b[i];
        }

        // 2. Gaussian Elimination to Row Echelon Form
        int pivot_row = 0;
        pivot_col_for_row.assign(m, -1);
        var_to_pivot_row.assign(n, -1);

        for (int col = 0; col < n && pivot_row < m; col++) {
            // Find pivot in this column
            int sel = -1;
            for (int row = pivot_row; row < m; row++) {
                if (abs(M[row][col]) > EPS) {
                    sel = row;
                    break;
                }
            }

            if (sel == -1) {
                // No pivot in this column, it's a free variable
                continue;
            }

            // Swap rows
            swap(M[pivot_row], M[sel]);

            // Normalize pivot row
            f64 div = M[pivot_row][col];
            for (int j = col; j <= n; j++)
                M[pivot_row][j] /= div;

            // Eliminate other rows
            for (int i = 0; i < m; i++) {
                if (i != pivot_row && abs(M[i][col]) > EPS) {
                    f64 fac = M[i][col];
                    for (int j = col; j <= n; j++)
                        M[i][j] -= fac * M[pivot_row][j];
                }
            }

            // Record pivot info
            pivot_col_for_row[pivot_row] = col;
            var_to_pivot_row[col] = pivot_row;
            pivot_row++;
        }

        // 3. Check for Inconsistency (0 = 1)
        for (int i = pivot_row; i < m; i++) {
            if (abs(M[i][n]) > EPS) {
                assert(false);
            }
        }

        // 4. Identify Free Variables
        for (int j = 0; j < n; j++) {
            if (var_to_pivot_row[j] == -1) {
                free_vars.push_back(j);
            }
        }

        // 5. Search on Free Variables
        // If system is fully determined, free_vars is empty, loop runs once.
        std::vector<int> current_x(n, 0);
        solve_free(0, current_x);

        // 6. Output
        if (best_sum == -1) {
            assert(false);
        }

        // Verification
        bool all_ok = true;
        for (int i = 0; i < m; i++) {
            long long calc = 0;
            for (int j = 0; j < n; j++) {
                calc += (long long)A[i][j] * best_sol[j];
            }
            if (calc != b[i]) {
                all_ok = false;
            }
        }
        if (!all_ok) {
            assert(false);
        }

        return best_sum;
    }

    void solve_free(size_t idx, std::vector<int>& x) {
        // Base Case: All free variables assigned
        if (idx == free_vars.size()) {
            // Compute Pivot Variables based on Free Variables
            // Row equation: x_pivot + sum(coeff * x_free) = RHS
            // x_pivot = RHS - sum(coeff * x_free)

            long long current_total = 0;
            std::vector<int> candidate_x = x;

            // Calculate Pivots in reverse order (bottom-up) usually helps,
            // but since we diagonalized (Jordan), we can do any order.
            for (int j = 0; j < n; j++) {
                int r = var_to_pivot_row[j];
                if (r != -1) {
                    f64 val = M[r][n];
                    for (int k = 0; k < n; k++) {
                        if (k != j && abs(M[r][k]) > EPS) {
                            val -= M[r][k] * candidate_x[k];
                        }
                    }

                    // Check Integer Integrality
                    if (val < -EPS || abs(val - round(val)) > EPS) {
                        // Negative or Non-Integer
                        return;
                    }
                    candidate_x[j] = (int)round(val);
                }
                current_total += candidate_x[j];
            }

            // Update Best Solution
            if (best_sum == -1 || current_total < best_sum) {
                best_sum = current_total;
                best_sol = candidate_x;
            }
            return;
        }

        // Recursive Step for Free Variables
        // We need to bound the free variable.
        // Heuristic limit: free vars are usually small in minimal sums.
        // For strictly determined systems, this loop doesn't matter.
        // For under-determined, we try a reasonable range.
        // Since input b_i <= 228, variables won't likely exceed ~230.
        int f_var = free_vars[idx];

        // Optimize: Check if this free var is bounded by any row directly?
        // Simple 0..230 search is safe given the constraints and speed of
        // modern CPU if number of free vars is small.
        for (int val = 0; val <= 250; val++) {
            x[f_var] = val;
            solve_free(idx + 1, x);
            // Optimization: If we found a valid solution, and since we iterate
            // upwards, later values usually increase sum. (Not strictly true
            // with negative relationships, but often true). For rigorous
            // correctness, we don't break early.
        }
    }
};

std::string solve_2(const std::string& input) {
    const auto rows = split_lines_view(input);

    int ans = 0;
    for (const auto& line : rows) {
        const auto machine = Machine::parse_machine(line);

        Solver solver;
        ans += solver.solve(machine);
    }

    return std::to_string(ans);
}

int run_tests() {
    using namespace std;

    assert(solve_1(readFile(day_path("day10", "in_small.txt"))) == "7");
    assert(solve_1(readFile(day_path("day10", "in.txt"))) == "411");

    assert(solve_2(readFile(day_path("day10", "in_small.txt"))) == "33");
    assert(solve_2(readFile(day_path("day10", "in.txt"))) == "16063");

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

    const auto input = readFile(day_path("day10", argv[2]));
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