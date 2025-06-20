//
// Created by Dmitry Gorbunov on 20.06.2025.
//

#include "book_database.hpp"
#include "statistics.hpp"

#include <benchmark/benchmark.h>
#include <random>

namespace {

bookdb::BookDatabase<> generate_database(size_t size) {
    static constexpr size_t kTitleSize = 20;
    static constexpr size_t kAuthorSize = 1;

    auto random_string = [](size_t size, char a = 'A', char b = 'Z') {
        return [=](auto& rng) {
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
        database.emplace_back(
            random_string(kAuthorSize)(rng),
            random_string(kTitleSize)(rng),
            std::uniform_int_distribution<int>(0, 2025)(rng),
            bookdb::Genre::Unknown,
            std::uniform_real_distribution<double>(0.0, 100.0)(rng),
            std::uniform_int_distribution<int>(0, 100)(rng)
        );
    }
    return database;
}

void BM_AuthorHistogramFlatMap(benchmark::State& state) {
    auto database = generate_database(state.range(0));
    for (auto _ : state) {
        auto result = bookdb::buildAuthorHistogramFlat(database);
        benchmark::DoNotOptimize(result);
    }
}

void BM_AuthorHistogram(benchmark::State& state) {
    auto database = generate_database(state.range(0));
    for (auto _ : state) {
        auto result = bookdb::buildAuthorHistogram(database);
        benchmark::DoNotOptimize(result);
    }
}

}  // namespace

BENCHMARK(BM_AuthorHistogram)->Range(4, 1 << 20);
BENCHMARK(BM_AuthorHistogramFlatMap)->Range(4, 1 << 20);

BENCHMARK_MAIN();