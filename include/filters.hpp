#pragma once

#include <algorithm>
#include <functional>
#include <ranges>

#include "book.hpp"
#include "concepts.hpp"

namespace bookdb {

inline auto YearBetween(int year_left, int year_right) {
    return [year_left, year_right](const Book &book) { return year_left <= book.year && book.year <= year_right; };
}

inline auto RatingAbove(double rating) {
    return [rating](const Book &book) { return book.rating > rating; };
}

inline auto GenreIs(Genre genre) {
    return [genre](const Book &book) { return book.genre == genre; };
}

inline auto AuthorIs(std::string_view author) {
    return [author](const Book &book) { return book.author == author; };
}

template <BookPredicate... Predicate>
auto all_of(Predicate &&...predicate) {
    return [... predicate = std::forward<Predicate>(predicate)](const Book &book) { return (... && predicate(book)); };
}

template <BookPredicate... Predicate>
auto any_of(Predicate &&...predicate) {
    return [... predicate = std::forward<Predicate>(predicate)](const Book &book) { return (... || predicate(book)); };
}

template <BookIterator Iterator, std::sentinel_for<Iterator> Sentinel, BookPredicate Predicate>
auto filterBooks(Iterator begin, Sentinel end, Predicate predicate) {
    std::vector<std::reference_wrapper<const Book>> result;
    std::ranges::copy(std::ranges::subrange(begin, end) | std::views::filter(predicate), std::back_inserter(result));
    return result;
}

}  // namespace bookdb
