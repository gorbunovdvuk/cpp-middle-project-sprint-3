#pragma once

#include "book.hpp"
#include "concepts.hpp"
#include "heterogeneous_lookup.hpp"

#include <print>
#include <span>
#include <string_view>
#include <unordered_set>
#include <vector>

namespace bookdb {

template <BookContainerLike BookContainer = std::vector<Book>>
class BookDatabase {
public:
    using value_type = typename BookContainer::value_type;
    using size_type = typename BookContainer::size_type;
    using difference_type = typename BookContainer::difference_type;
    using reference = typename BookContainer::reference;
    using const_reference = typename BookContainer::const_reference;
    using iterator = typename BookContainer::iterator;
    using const_iterator = typename BookContainer::const_iterator;

    using AuthorContainer = std::unordered_set<std::string, TransparentStringHash, TransparentStringEqual>;

    BookDatabase() = default;

    constexpr BookDatabase(std::initializer_list<Book> books) {
        for (auto book : books) {
            emplace_back(std::move(book));
        }
    }

    void clear() {
        books_.clear();
        authors_.clear();
    }

    const BookContainer &GetBooks() const { return books_; }
    const AuthorContainer &GetAuthors() const { return authors_; }

    void push_back(const Book &book) {
        books_.push_back(book);
        authors_.emplace(book.author);
    }

    template <typename... Args>
        requires std::constructible_from<Book, Args...>
    void emplace_back(Args &&...args) {
        Book &book = books_.emplace_back(std::forward<Args>(args)...);
        book.author = *authors_.emplace(book.author).first;
    }

    std::size_t size() const { return books_.size(); }
    auto begin(this auto &self) { return self.books_.begin(); }
    auto end(this auto &self) { return self.books_.end(); }

private:
    BookContainer books_;
    AuthorContainer authors_;
};

}  // namespace bookdb

namespace std {
template <bookdb::BookContainerLike BookContainer>
struct formatter<bookdb::BookDatabase<BookContainer>> {
    template <typename FormatContext>
    auto format(const bookdb::BookDatabase<BookContainer> &db, FormatContext &fc) const {
        format_to(fc.out(), "BookDatabase (size = {}): ", db.size());

        format_to(fc.out(), "Books:\n");
        for (const auto &book : db.GetBooks()) {
            format_to(fc.out(), "- {}\n", book);
        }

        format_to(fc.out(), "Authors:\n");
        for (const auto &author : db.GetAuthors()) {
            format_to(fc.out(), "- {}\n", author);
        }

        return fc.out();
    }

    constexpr auto parse(format_parse_context &ctx) {
        return ctx.begin();  // Просто игнорируем пользовательский формат
    }
};

}  // namespace std
