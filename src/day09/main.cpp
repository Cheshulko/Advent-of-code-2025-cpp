#include <aux.hpp>

struct Point {
    int x, y;

    Point(int x, int y) : x(x), y(y) {}

    long long rect_sqr(const Point& other) const {
        const long long dx = abs(x - other.x) + 1;
        const long long dy = abs(y - other.y) + 1;

        return dx * dy;
    }
};

class Compressed2D {
   public:
    explicit Compressed2D(const std::vector<Point>& points) {
        xs.reserve(points.size() * 3);
        ys.reserve(points.size() * 3);

        for (const auto& p : points) {
            xs.push_back(p.x);
            ys.push_back(p.y);

            xs.push_back(p.x - 1);
            xs.push_back(p.x + 1);

            ys.push_back(p.y - 1);
            ys.push_back(p.y + 1);
        }

        std::sort(xs.begin(), xs.end());
        std::sort(ys.begin(), ys.end());

        xs.erase(std::unique(xs.begin(), xs.end()), xs.end());
        ys.erase(std::unique(ys.begin(), ys.end()), ys.end());

        compressed_points.reserve(points.size());
        for (const auto& p : points) {
            compressed_points.push_back({compressX(p.x), compressY(p.y)});
        }
    }

    Compressed2D(const Compressed2D&) = delete;
    Compressed2D& operator=(const Compressed2D&) = delete;

    Compressed2D(Compressed2D&&) = default;
    Compressed2D& operator=(Compressed2D&&) = default;

    Point originalByIndex(size_t i) const {
        return Point{xs[compressed_points[i].x], ys[compressed_points[i].y]};
    };

    int restoreX(int cy) const { return xs[cy]; }
    int restoreY(int cy) const { return ys[cy]; }

    size_t size() const { return compressed_points.size(); }

    Point operator[](size_t index) const { return compressed_points[index]; }

   private:
    int compressX(int v) const {
        return std::lower_bound(xs.begin(), xs.end(), v) - xs.begin();
    }

    int compressY(int v) const {
        return std::lower_bound(ys.begin(), ys.end(), v) - ys.begin();
    }

   private:
    std::vector<int> xs;
    std::vector<int> ys;
    std::vector<Point> compressed_points;
};

std::string solve_1(const std::string& input) {
    const auto rows = split_lines_view(input);
    const auto n = rows.size();

    std::vector<Point> points;
    for (const auto& line : rows) {
        const auto xy_str = split_view(line, ",");

        points.push_back(Point{
            parse_from_substr<int>(xy_str[0]),
            parse_from_substr<int>(xy_str[1]),
        });
    }

    long long ans = 0;
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = i + 1; j < n; ++j) {
            ans = std::max(ans, points[i].rect_sqr(points[j]));
        }
    }

    return std::to_string(ans);
}

bool on_segment(const Point& p, const Point& q, const Point& r) {
    if (q.x < std::min(p.x, r.x) || q.x > std::max(p.x, r.x)) {
        return false;
    }
    if (q.y < std::min(p.y, r.y) || q.y > std::max(p.y, r.y)) {
        return false;
    }

    return ((r.x - p.x) * (q.y - p.y) == (q.x - p.x) * (r.y - p.y));
}

bool point_in_polygon(const std::vector<Point>& poly, const Point& p) {
    bool inside = false;
    int n = poly.size();

    for (int i = 0, j = n - 1; i < n; j = i++) {
        const Point& a = poly[j];
        const Point& b = poly[i];

        if (on_segment(a, p, b)) {
            return true;
        }

        long long ay = a.y, by = b.y;
        long long ax = a.x, bx = b.x;
        if (ay > by) {
            std::swap(ay, by);
            std::swap(ax, bx);
        }

        if (p.y > ay && p.y <= by) {
            long long dx = bx - ax;
            long long dy = by - ay;
            long long qy = p.y - ay;

            long long intersectX = ax + dx * qy / dy;

            if (p.x <= intersectX) {
                inside = !inside;
            }
        }
    }
    return inside;
}

std::string solve_2(const std::string& input) {
    const auto rows = split_lines_view(input);

    std::vector<Point> points;
    for (const auto& line : rows) {
        const auto xy_str = split_view(line, ",");

        points.push_back(Point{
            parse_from_substr<int>(xy_str[0]),
            parse_from_substr<int>(xy_str[1]),
        });
    }

    Compressed2D compressed(points);

    long long ans = 0;
    for (size_t i = 0; i < compressed.size(); ++i) {
        for (size_t j = i + 1; j < compressed.size(); ++j) {
            const auto pi = compressed.originalByIndex(i);
            const auto pj = compressed.originalByIndex(j);

            const auto mi_x = std::min(compressed[i].x, compressed[j].x);
            const auto ma_x = std::max(compressed[i].x, compressed[j].x);
            const auto mi_y = std::min(compressed[i].y, compressed[j].y);
            const auto ma_y = std::max(compressed[i].y, compressed[j].y);

            bool all = true;

            for (int x = mi_x; x <= ma_x && all; ++x) {
                all &= point_in_polygon(points,
                                        Point{
                                            compressed.restoreX(x),
                                            pi.y,
                                        }) &
                       point_in_polygon(points, Point{
                                                    compressed.restoreX(x),
                                                    pj.y,
                                                });
            }

            for (int y = mi_y; y <= ma_y && all; ++y) {
                all &= point_in_polygon(points,
                                        Point{pi.x, compressed.restoreY(y)}) &
                       point_in_polygon(points,
                                        Point{pj.x, compressed.restoreY(y)});
            }

            if (all) {
                ans = std::max(ans, pi.rect_sqr(pj));
            }
        }
    }
    return std::to_string(ans);
}

int run_tests() {
    using namespace std;

    assert(solve_1(readFile(day_path("day09", "in_small.txt"))) == "50");
    assert(solve_1(readFile(day_path("day09", "in.txt"))) == "4764078684");

    assert(solve_2(readFile(day_path("day09", "in_small.txt"))) == "24");
    assert(solve_2(readFile(day_path("day09", "in.txt"))) == "1652344888");

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

    const auto input = readFile(day_path("day09", argv[2]));
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