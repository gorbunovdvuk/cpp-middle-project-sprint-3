#pragma once

#include "book_database.hpp"
#include "comparators.hpp"

#include <algorithm>
#include <flat_map>
#include <iostream>
#include <iterator>
#include <random>
#include <ranges>
#include <stdexcept>
#include <string_view>

namespace bookdb {

template <typename MapType, BookContainerLike T, typename... Args>
    requires std::is_constructible_v<MapType, Args...>
auto buildAuthorHistogramWithType(const BookDatabase<T> &cont, Args &&...args) {
    MapType histogram(std::forward<Args>(args)...);
    for (const auto &book : cont) {
        ++histogram[book.author];
    }
    return histogram;
}

template <BookContainerLike T, typename Comparator = TransparentStringLess>
auto buildAuthorHistogramFlat(const BookDatabase<T> &cont, Comparator comp = {}) {
    return buildAuthorHistogramWithType<std::flat_map<std::string_view, size_t, Comparator>>(cont, comp);
}

template <BookContainerLike T>
auto buildAuthorHistogram(const BookDatabase<T> &cont) {
    return buildAuthorHistogramWithType<std::unordered_map<std::string_view, size_t>>(cont);
}

template <BookContainerLike T>
auto calculateGenreRatings(const BookDatabase<T> &cont) {
    std::flat_map<Genre, double> total_rating;
    std::flat_map<Genre, size_t> books_count;
    for (const auto &book : cont) {
        total_rating[book.genre] += book.rating;
        ++books_count[book.genre];
    }
    std::flat_map<Genre, double> result;
    for (auto genre : total_rating | std::views::keys) {
        result[genre] += total_rating[genre] / static_cast<double>(books_count[genre]);
    }
    return result;
}

template <BookContainerLike T>
double calculateAverageRating(const BookDatabase<T> &cont) {
    return std::transform_reduce(cont.begin(), cont.end(), 0.0, std::plus<>{},
                                 [](const Book &book) { return book.rating; }) /
           std::max(1.0, static_cast<double>(cont.size()));
}

template <BookContainerLike T, BookComparator Comp = comp::GreaterByRating>
auto getTopNBy(const BookDatabase<T> &cont, std::size_t n, Comp comp = Comp{}) {
    n = std::min(n, cont.size());

    std::vector<std::reference_wrapper<const Book>> references;
    references.reserve(n);
    std::ranges::transform(cont, std::back_inserter(references), [](const Book &book) { return std::ref(book); });
    std::nth_element(references.begin(), references.begin() + n, references.end(), comp);
    references.erase(references.begin() + n, references.end());
    return references;
}

template <BookContainerLike T, typename Random>
    requires std::random_access_iterator<std::ranges::iterator_t<BookDatabase<T>>>
auto sampleRandomBooks(const BookDatabase<T> &cont, std::size_t n, Random &&rng) {
    if (n > cont.size()) {
        throw std::invalid_argument("Requested number exceeds the size of the book db");
    }
    std::vector<std::reference_wrapper<const Book>> references;
    references.reserve(n);
    std::ranges::sample(cont, std::back_inserter(references), n, rng);
    return references;
}

}  // namespace bookdb
