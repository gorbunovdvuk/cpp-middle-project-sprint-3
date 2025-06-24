//
// Created by Dmitry Gorbunov on 20.06.2025.
//

#include "book_database.hpp"
#include "statistics.hpp"

#include <benchmark/benchmark.h>
#include <random>

namespace {

constexpr size_t kTitleSize = 10;
constexpr size_t kAuthorSize = 4;

bookdb::BookDatabase<> generate_database(size_t size) {
    auto random_string = [](size_t size, char a = 'A', char b = 'Z') {
        return [=](auto &rng) {
            std::string result;
            result.reserve(size);
            for (size_t i = 0; i < size; i++) {
                result.push_back(std::uniform_int_distribution<int>(static_cast<int>(a), static_cast<int>(b))(rng));
            }
            return result;
        };
    };

    std::mt19937 rng;
    bookdb::BookDatabase<> database;
    for (size_t i = 0; i < size; i++) {
        database.emplace_back(random_string(kTitleSize)(rng), random_string(kAuthorSize)(rng),
                              std::uniform_int_distribution<int>(0, 2025)(rng), bookdb::Genre::Unknown,
                              std::uniform_real_distribution<double>(0.0, 100.0)(rng),
                              std::uniform_int_distribution<int>(0, 100)(rng));
    }
    return database;
}

void BM_AuthorHistogramFlatMap(benchmark::State &state) {
    const size_t kSize = state.range(0);
    auto database = generate_database(kSize * kAuthorSize);
    state.SetComplexityN(kSize);
    state.counters["authors"] = kSize;
    state.counters["author_size"] = kAuthorSize;
    state.counters["total_size"] = kSize * kAuthorSize;
    for (auto _ : state) {
        auto result = bookdb::buildAuthorHistogramFlat(database);
        benchmark::DoNotOptimize(result);
    }
}

void BM_AuthorHistogram(benchmark::State &state) {
    const size_t kSize = state.range(0);
    auto database = generate_database(kSize * kAuthorSize);
    state.SetComplexityN(kSize);
    state.counters["authors"] = kSize;
    state.counters["author_size"] = kAuthorSize;
    state.counters["total_size"] = kSize * kAuthorSize;
    for (auto _ : state) {
        auto result = bookdb::buildAuthorHistogram(database);
        benchmark::DoNotOptimize(result);
    }
}

}  // namespace

BENCHMARK(BM_AuthorHistogram)->Range(1024, 1 << 16)->Complexity();
BENCHMARK(BM_AuthorHistogramFlatMap)->Range(1024, 1 << 16)->Complexity();

BENCHMARK_MAIN();
