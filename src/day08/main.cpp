#include <aux.hpp>

class DSU {
   private:
    std::vector<size_t> parent;
    std::vector<size_t> rank;

   public:
    explicit DSU(size_t n) {
        parent.resize(n + 1);
        rank.resize(n + 1, 0);
        std::iota(parent.begin(), parent.end(), 0);
    }

    ~DSU() = default;

    size_t find_set(size_t v) {
        if (v == parent[v]) {
            return v;
        }

        return parent[v] = find_set(parent[v]);
    }

    bool union_sets(size_t a, size_t b) {
        a = find_set(a);
        b = find_set(b);

        if (a != b) {
            if (rank[a] < rank[b]) {
                std::swap(a, b);
            }

            parent[b] = a;

            if (rank[a] == rank[b]) {
                rank[a]++;
            }

            return true;
        }
        return false;
    }
};

class Solver {
   public:
    struct Edge {
        size_t u, v;
        long long weight;

        Edge(size_t u, size_t v, long long weight)
            : u(u), v(v), weight(weight) {}

        bool operator<(Edge const& other) const {
            return weight < other.weight;
        }
    };

   public:
    explicit Solver(size_t nodes) : n(nodes), dsu(nodes) {}

    ~Solver() = default;

    void add_edge(size_t u, size_t v, long long weight) {
        edges.push_back({u, v, weight});
    }

    std::pair<size_t, size_t> solve(int connections) {
        dsu = DSU(n);

        std::sort(edges.begin(), edges.end());

        std::pair<size_t, size_t> last_connected;
        for (const Edge& e : edges) {
            if (connections == 0) {
                break;
            }
            --connections;

            if (dsu.union_sets(e.u, e.v)) {
                last_connected = {e.u, e.v};
            }
        }

        return last_connected;
    }

    size_t get_component(size_t node) { return dsu.find_set(node); }

   private:
    size_t n;
    DSU dsu;
    std::vector<Edge> edges;
};

struct Point {
    int x, y, z;

    Point(int x, int y, int z) : x(x), y(y), z(z) {}

    long long dist_sqr(const Point& other) const {
        const long long dx = x - other.x;
        const long long dy = y - other.y;
        const long long dz = z - other.z;

        return dx * dx + dy * dy + dz * dz;
    }
};

std::string solve_1(const std::string& input, size_t connections) {
    const auto rows = split_lines_view(input);
    const auto n = rows.size();

    std::vector<Point> points;

    for (const auto& line : rows) {
        const auto xyz_str = split_view(line, ",");

        points.push_back(Point{
            parse_from_substr<int>(xyz_str[0]),
            parse_from_substr<int>(xyz_str[1]),
            parse_from_substr<int>(xyz_str[2]),
        });
    }

    Solver mst(n);
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = i + 1; j < n; ++j) {
            mst.add_edge(i, j, points[i].dist_sqr(points[j]));
        }
    }

    mst.solve((int)connections);

    std::map<int, long long> sizes;
    for (size_t i = 0; i < n; ++i) {
        const auto comp = mst.get_component(i);
        ++sizes[comp];
    }

    std::vector<long long> sizes_sorted(sizes.size());
    std::transform(sizes.cbegin(), sizes.cend(), sizes_sorted.begin(),
                   [](const auto& e) { return e.second; });

    std::sort(sizes_sorted.begin(), sizes_sorted.end(), std::greater<>());

    long long ans = 1;
    for (auto i = 0; i < 3; ++i) {
        ans *= sizes_sorted[i];
    }

    return std::to_string(ans);
}

std::string solve_2(const std::string& input) {
    const auto rows = split_lines_view(input);
    const auto n = rows.size();

    std::vector<Point> points;

    for (const auto& line : rows) {
        const auto xyz_str = split_view(line, ",");

        points.push_back(Point{
            parse_from_substr<int>(xyz_str[0]),
            parse_from_substr<int>(xyz_str[1]),
            parse_from_substr<int>(xyz_str[2]),
        });
    }

    Solver mst(n);
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = i + 1; j < n; ++j) {
            mst.add_edge(i, j, points[i].dist_sqr(points[j]));
        }
    }

    const auto last_connected = mst.solve((int)-1);

    const auto ans = (long long)points[last_connected.first].x *
                     (long long)points[last_connected.second].x;

    return std::to_string(ans);
}

int run_tests() {
    using namespace std;

    assert(solve_1(readFile(day_path("day08", "in_small.txt")), 10) == "40");
    assert(solve_1(readFile(day_path("day08", "in.txt")), 1000) == "175440");

    assert(solve_2(readFile(day_path("day08", "in_small.txt"))) == "25272");
    assert(solve_2(readFile(day_path("day08", "in.txt"))) == "3200955921");

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

    const auto input = readFile(day_path("day08", argv[2]));
    switch (stoi(argv[1])) {
        case 1:
            cout << solve_1(input, 1000) << endl;

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