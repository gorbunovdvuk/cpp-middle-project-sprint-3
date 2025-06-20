#pragma once

#include <concepts>
#include <iterator>

#include "book.hpp"

namespace bookdb {

template <typename BookContainer>
concept BookContainerLike = std::ranges::sized_range<BookContainer> &&
                            std::default_initializable<BookContainer> &&
                            std::same_as<typename BookContainer::value_type, Book> && requires(BookContainer container, const BookContainer const_container) {
    typename BookContainer::size_type;
    typename BookContainer::difference_type;
    typename BookContainer::reference;
    typename BookContainer::const_reference;
    typename BookContainer::iterator;
    typename BookContainer::const_iterator;
    { BookContainer(std::declval<std::initializer_list<Book>>()) };
    { container.push_back(std::declval<Book>()) };
    { container.emplace_back(std::declval<Book>()) };
    { container.begin() } -> std::same_as<typename BookContainer::iterator>;
    { const_container.begin() } -> std::same_as<typename BookContainer::const_iterator>;
    { container.end() } -> std::same_as<typename BookContainer::iterator>;
    { const_container.end() } -> std::same_as<typename BookContainer::const_iterator>;
    { container.size() } -> std::same_as<typename BookContainer::size_type>;
};


template <typename T>
concept BookIterator = std::input_or_output_iterator<T> && std::is_same_v<std::iter_value_t<T>, Book>;

template <typename P>
concept BookPredicate = std::predicate<P, const Book&>;

template <typename C>
concept BookComparator = std::strict_weak_order<C, const Book&, const Book&>;

}  // namespace bookdb
