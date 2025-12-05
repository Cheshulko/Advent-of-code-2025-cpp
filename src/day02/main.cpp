#include <aux.hpp>

std::string solve_1(const std::string& input) {
    const auto ranges = split_view(input, ",");

    long long ans = 0;
    for (const auto& range : ranges) {
        const auto ids = split_view(range, "-");
        const auto start = parse_from_substr<long long>(ids[0]);
        const auto end = parse_from_substr<long long>(ids[1]);

        const auto end_digits = static_cast<int>(std::log10(end)) + 1;
        for (long long n = 1; n <= pow(10, (end_digits + 1) / 2); ++n) {
            const auto digits = static_cast<int>(std::log10(n)) + 1;
            const auto nn = n * pow(10, digits) + n;
            if (nn >= start && nn <= end) {
                ans += nn;
            }
        }
    }

    return std::to_string(ans);
}

std::string solve_2(const std::string& input) {
    const auto ranges = split_view(input, ",");

    long long ans = 0;
    for (const auto& range : ranges) {
        const auto ids = split_view(range, "-");
        const auto start = parse_from_substr<long long>(ids[0]);
        const auto end = parse_from_substr<long long>(ids[1]);

        std::set<long long> seen;
        const auto end_digits = static_cast<int>(std::log10(end)) + 1;
        for (long long n = 1; n <= pow(10, (end_digits + 1) / 2); ++n) {
            const auto digits = static_cast<int>(std::log10(n)) + 1;

            for (auto nn = n; nn <= end;) {
                nn = nn * pow(10, digits) + n;
                if (nn >= start && nn <= end) {
                    if (seen.cend() == seen.find(nn)) {
                        ans += nn;
                    }

                    seen.insert(nn);
                }
            }
        }
    }

    return std::to_string(ans);
}

int run_tests() {
    using namespace std;

    assert(solve_1(readFile(day_path("day02", "in_small.txt"))) ==
           "1227775554");
    assert(solve_1(readFile(day_path("day02", "in.txt"))) == "30323879646");

    assert(solve_2(readFile(day_path("day02", "in_small.txt"))) ==
           "4174379265");
    assert(solve_2(readFile(day_path("day02", "in.txt"))) == "43872163557");

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

    const auto input = readFile(day_path("day02", argv[2]));
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