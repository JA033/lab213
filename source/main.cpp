#include <algorithm>
#include <chrono>
#include <deque>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <regex>

#include "gecsa/gecsa.hpp"
#include "gecsa/sparse_bitmap.hpp"

#include "wildcard_executor.hpp"
#include "document_searcher.hpp"
#include "wildcard_pattern.hpp"
#include "utils.hpp"
#include "timer.hpp"

#include <execution>

using namespace gecsa;
using namespace wildcard;
using namespace std::string_view_literals;
namespace fs = std::filesystem;

i32 main(i32 argc, const char *argv[]) {
    std::vector<int> vec(10000);
    std::sort(std::execution::par, vec.begin(), vec.end());


    const fs::path input_path = fs::current_path() / "test.dat";
    std::vector<std::string> texts;
    std::ifstream file(input_path);
    std::string line;
    while (std::getline(file, line)) {
        texts.push_back(line);
    }

    std::vector<ConstByteSpan> bytes(texts.size());
    std::transform(texts.begin(), texts.end(), bytes.begin(), [&](const auto &text) {
        return bytes_cast(std::string_view{text});
    });

    usize total_size = texts.size() + 1;
    std::ranges::for_each(texts, [&](const auto &s) {
        total_size += s.size();
    });

    std::string buf;
    for (uindex i = 0; i < texts.size(); ++i) {
        buf += texts[i] + '\n';
    }

    wildcard::CSADocumentSearcher searcher(bytes, 16);
    std::cout << "csa compression ratio: " << 
        (f32) searcher.csa().size_in_bytes(GeCSA::kPhi | GeCSA::kSampleMarks | GeCSA::kSaSamples) / (f32) total_size * 100.0F << "%\n";
    std::cout << "doc_sep compression ratio: " 
        << (f32) searcher.document_separators().size_in_bytes() / (f32) total_size * 100.0F << "%\n";
    std::cout << "total compression ratio: " 
        << (f32) (searcher.document_separators().size_in_bytes() + searcher.csa().size_in_bytes()) / (f32) total_size * 100.0F << "%\n";


    CSAWildcardExecutor executor;

    f64 time;
    std::vector<uindex> matched_ids;
    
    time = measure_time(TimeUnit::kSeconds, [&] {
        matched_ids = executor.match(searcher, "hdfs:*8200/??po1/data?/*w");
    });

    std::cout << "time: " << time << "s\n";


    std::cout << matched_ids.size() << "\n";

    std::cout << "final check start\n";
    for (uindex i = 0; i < matched_ids.size(); ++i) {
        const auto &str = texts[matched_ids[i]];
    }

    bool sorted = std::is_sorted(matched_ids.begin(), matched_ids.end());
    if (!sorted) {
        std::cout << "result is not sorted\n";
    }

    for (uindex i = 1; i < matched_ids.size(); ++i) {
        if (matched_ids[i] == matched_ids[i - 1]) {
            std::cout << "result is not unique\n";
            break;
        }
    }

    std::cout << "final check end\n";

    return 0;

}