#pragma once

#include <format>
#include <magic_enum/magic_enum.hpp>
#include <string_view>

namespace bookdb {

enum class Genre { Fiction, NonFiction, SciFi, Biography, Mystery, Unknown };

constexpr Genre GenreFromString(std::string_view s) { return magic_enum::enum_cast<Genre>(s).value_or(Genre::Unknown); }

constexpr std::string_view GenreToString(Genre genre) { return magic_enum::enum_name<Genre>(genre); }

struct Book {
    std::string title;
    std::string_view author;

    int year;
    Genre genre;
    double rating;
    int read_count;

    constexpr Book(std::string title, std::string_view author, int year, Genre genre, double rating, int read_count)
        : title(std::move(title)), author(author), year(year), genre(genre), rating(rating), read_count(read_count) {}

    constexpr Book(std::string title, std::string_view author, int year, std::string_view genre, double rating,
                   int read_count)
        : Book(std::move(title), author, year, GenreFromString(genre), rating, read_count) {}
};
}  // namespace bookdb

namespace std {
template <>
struct formatter<bookdb::Genre, char> {
    template <typename FormatContext>
    auto format(const bookdb::Genre g, FormatContext &fc) const {
        return format_to(fc.out(), "{}", magic_enum::enum_name(g));
    }

    constexpr auto parse(format_parse_context &ctx) {
        return ctx.begin();  // Просто игнорируем пользовательский формат
    }
};

template <>
struct formatter<bookdb::Book, char> {
    template <typename FormatContext>
    constexpr auto format(const bookdb::Book b, FormatContext &fc) const {
        return std::format_to(fc.out(), R"({{"{}", "{}", {}, Genre::{}, {}, {}}})", b.author, b.title, b.year,
                              bookdb::GenreToString(b.genre), b.rating, b.read_count);
    }

    constexpr auto parse(format_parse_context &ctx) const { return ctx.begin(); }
};

}  // namespace std
