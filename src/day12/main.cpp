#include <aux.hpp>

constexpr size_t MAX_BITS = 2500;

using grid_bitset_t = std::bitset<MAX_BITS>;
using presents_t = std::vector<size_t>;

class present_shape_t {
   public:
    explicit present_shape_t() : shape(std::array<std::array<int, 3>, 3>{}) {}

    explicit present_shape_t(std::array<std::array<int, 3>, 3> shape)
        : shape(shape) {}

    const std::array<std::array<int, 3>, 3>& get_shape() const {
        return shape;
    };

    std::array<std::array<int, 3>, 3>& get_shape_mut() { return shape; };

    void rotate() {
        for (size_t i = 0; i < 3; ++i) {
            for (size_t j = i + 1; j < 3; ++j) {
                std::swap(shape[i][j], shape[j][i]);
            }
        }
        for (size_t i = 0; i < 3; ++i) {
            std::reverse(shape[i].begin(), shape[i].end());
        }
    }

    bool operator<(const present_shape_t& other) const {
        return shape < other.shape;
    }

    bool operator==(const present_shape_t& other) const {
        return shape == other.shape;
    }

   private:
    std::array<std::array<int, 3>, 3> shape;
};

class tree_shape_t {
   public:
    tree_shape_t(size_t h, size_t w)
        : h(h),
          w(w),
          shape(std::vector<std::vector<int>>(h, std::vector<int>(w, 0))) {}

    size_t get_h() const { return h; }

    size_t get_w() const { return w; }

    bool place(const size_t r, const size_t c, const present_shape_t& present) {
        const auto& p_shape = present.get_shape();
        if (r + 3 > h || c + 3 > w)
            return false;

        for (size_t i = 0; i < 3; ++i) {
            for (size_t j = 0; j < 3; ++j) {
                if (p_shape[i][j] && shape[r + i][c + j])
                    return false;
            }
        }
        for (size_t i = 0; i < 3; ++i) {
            for (size_t j = 0; j < 3; ++j) {
                shape[r + i][c + j] |= p_shape[i][j];
            }
        }

        return true;
    }

    friend bool solve(tree_shape_t&,
                      presents_t&,
                      const std::vector<present_shape_t>&);

   private:
    size_t h;
    size_t w;
    std::vector<std::vector<int>> shape;
};

inline void hash_combine(std::size_t& seed, std::size_t value) {
    seed ^= value + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

struct memo_t {
    grid_bitset_t mask;
    std::vector<size_t> remaining;

    bool operator==(const memo_t& other) const {
        return mask == other.mask && remaining == other.remaining;
    }
};

struct memo_hash_t {
    std::size_t operator()(const memo_t& k) const {
        std::size_t seed = std::hash<grid_bitset_t>{}(k.mask);
        for (size_t count : k.remaining) {
            hash_combine(seed, count);
        }
        return seed;
    }
};

struct placement_t {
    grid_bitset_t mask;
    int r, c;
    present_shape_t shape_obj;
};

struct solver_t {
    size_t H, W;
    size_t region_area;
    std::vector<size_t> shape_areas;
    std::vector<std::vector<placement_t>> placements_by_type;
    std::vector<int> shape_order;

    std::unordered_map<memo_t, bool, memo_hash_t> memo;

    struct Step {
        int type_idx;
        int placement_idx;
    };
    std::vector<Step> result_path;

    bool search(grid_bitset_t occupied, std::vector<size_t>& remaining) {
        size_t rem_area_needed = 0;
        for (int i : shape_order) {
            if (remaining[i] > 0) {
                rem_area_needed += remaining[i] * shape_areas[i];
            }
        }

        if (rem_area_needed == 0) {
            return true;
        }

        size_t free_area = region_area - occupied.count();
        if (rem_area_needed > free_area) {
            return false;
        }

        memo_t key{occupied, remaining};
        if (memo.count(key)) {
            return memo[key];
        }

        int t = -1;
        for (int idx : shape_order) {
            if (remaining[idx] > 0) {
                t = idx;
                break;
            }
        }

        if (t == -1) {
            return true;
        }

        const auto& candidates = placements_by_type[t];
        for (size_t p_idx = 0; p_idx < candidates.size(); ++p_idx) {
            const auto& p = candidates[p_idx];

            if ((occupied & p.mask).any()) {
                continue;
            }

            --remaining[t];

            if (search(occupied | p.mask, remaining)) {
                result_path.push_back({t, (int)p_idx});

                return true;
            }

            ++remaining[t];
        }

        return memo[key] = false;
    }
};

bool solve(tree_shape_t& tree,
           presents_t& presents,
           const std::vector<present_shape_t>& present_shapes) {
    assert(tree.get_h() * tree.get_w() <= MAX_BITS);

    solver_t ctx;
    ctx.H = tree.get_h();
    ctx.W = tree.get_w();
    ctx.region_area = ctx.H * ctx.W;
    size_t total_types = presents.size();

    ctx.shape_areas.resize(total_types, 0);
    ctx.placements_by_type.resize(total_types);

    grid_bitset_t initial_mask;
    for (size_t r = 0; r < ctx.H; ++r) {
        for (size_t c = 0; c < ctx.W; ++c) {
            if (tree.shape[r][c]) {
                initial_mask.set(r * ctx.W + c);
            }
        }
    }

    for (size_t i = 0; i < total_types; ++i) {
        if (presents[i] == 0) {
            continue;
        }

        auto s = present_shapes[i].get_shape();
        int area = 0;
        for (auto& r : s) {
            for (int v : r) {
                if (v) {
                    ++area;
                }
            }
        }

        ctx.shape_areas[i] = area;

        std::vector<present_shape_t> uniques;
        present_shape_t temp = present_shapes[i];
        for (int k = 0; k < 4; ++k) {
            bool found = false;
            for (const auto& u : uniques) {
                if (u == temp) {
                    found = true;
                }
            }

            if (!found) {
                uniques.push_back(temp);
            }
            temp.rotate();
        }

        for (const auto& rot : uniques) {
            const auto& grid_shape = rot.get_shape();

            for (size_t r = 0; r <= ctx.H - 3; ++r) {
                for (size_t c = 0; c <= ctx.W - 3; ++c) {
                    grid_bitset_t mask;
                    bool in_bounds = true;

                    for (int pr = 0; pr < 3; ++pr) {
                        for (int pc = 0; pc < 3; ++pc) {
                            if (grid_shape[pr][pc]) {
                                size_t idx = (r + pr) * ctx.W + (c + pc);
                                if (idx >= MAX_BITS || idx >= ctx.H * ctx.W) {
                                    in_bounds = false;
                                    break;
                                }
                                mask.set(idx);
                            }
                        }
                        if (!in_bounds) {
                            break;
                        }
                    }

                    if (in_bounds) {
                        if (!(mask & initial_mask).any()) {
                            ctx.placements_by_type[i].push_back(
                                {mask, (int)r, (int)c, rot});
                        }
                    }
                }
            }
        }
    }

    for (size_t i = 0; i < total_types; ++i) {
        if (presents[i] > 0) {
            ctx.shape_order.push_back(i);
        }
    }

    for (int idx : ctx.shape_order) {
        if (ctx.placements_by_type[idx].empty()) {
            return false;
        }
    }

    std::sort(ctx.shape_order.begin(), ctx.shape_order.end(),
              [&](int a, int b) {
                  size_t len_a = ctx.placements_by_type[a].size();
                  size_t len_b = ctx.placements_by_type[b].size();
                  if (len_a != len_b)
                      return len_a < len_b;
                  return ctx.shape_areas[a] > ctx.shape_areas[b];
              });

    if (ctx.search(initial_mask, presents)) {
        std::fill(presents.begin(), presents.end(), 0);
        for (const auto& step : ctx.result_path) {
            const auto& pl =
                ctx.placements_by_type[step.type_idx][step.placement_idx];
            tree.place(pl.r, pl.c, pl.shape_obj);
        }
        return true;
    }

    return false;
}

std::string solve_1(const std::string& input) {
    const auto parts = split_view(input, "\n\n");
    const auto n = parts.size();

    std::vector<present_shape_t> present_shapes;
    for (size_t i = 0; i < n - 1; ++i) {
        present_shape_t present_shape;
        const auto x = split_view(parts[i], ":\n")[1];
        const auto shape_str = split_lines_view(x);

        for (size_t r = 0; r < 3; ++r) {
            for (size_t c = 0; c < 3; ++c) {
                if (r < shape_str.size() && c < shape_str[r].size()) {
                    present_shape.get_shape_mut()[r][c] =
                        (shape_str[r][c] == '#');
                }
            }
        }
        present_shapes.push_back(present_shape);
    }

    auto ans = 0;

    const auto trees_str = split_lines_view(parts[n - 1]);
    for (size_t i = 0; i < trees_str.size(); ++i) {
        if (trees_str[i].empty())
            continue;

        const auto tree_str = split_view(trees_str[i], ": ");
        const auto size = split_view(tree_str[0], "x");

        const auto h = parse_from_substr<size_t>(size[0]);
        const auto w = parse_from_substr<size_t>(size[1]);

        tree_shape_t tree(h, w);

        presents_t presents;
        const auto cnt_str = split_view(tree_str[1], " ");
        for (size_t j = 0; j < cnt_str.size(); ++j) {
            presents.push_back(parse_from_substr<size_t>(cnt_str[j]));
        }

        ans += solve(tree, presents, present_shapes);
    }

    return std::to_string(ans);
}

std::string solve_2(const std::string& input) {
    (void)input;

    return std::to_string(42);
}

int run_tests() {
    using namespace std;

    assert(solve_1(readFile(day_path("day12", "in_small.txt"))) == "2");
    assert(solve_1(readFile(day_path("day12", "in.txt"))) == "510");

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

    const auto input = readFile(day_path("day12", argv[2]));
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