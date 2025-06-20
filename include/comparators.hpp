#pragma once

#include "book.hpp"

namespace bookdb::comp {

struct LessByAuthor {
    using is_transparent = std::true_type;

    constexpr bool operator()(const Book& lhs, std::string_view rhs) const {
        return lhs.author < rhs;
    }

    constexpr bool operator()(const Book& lhs, const Book& rhs) const {
        return lhs.author < rhs.author;
    }
};

struct GreaterByRating {
    using is_transparent = std::true_type;

    constexpr bool operator()(const Book& lhs, const Book& rhs) const {
        return lhs.rating < rhs.rating;
    }

    constexpr bool operator()(const Book& lhs, double rhs) const {
        return lhs.rating < rhs;
    }
};

struct LessByRating {
    using is_transparent = std::true_type;

    constexpr bool operator()(const Book& lhs, const Book& rhs) const {
        return lhs.rating < rhs.rating;
    }

    constexpr bool operator()(const Book& lhs, double rhs) const {
        return lhs.rating < rhs;
    }
};

struct LessByPopularity {
    using is_transparent = std::true_type;

    constexpr bool operator()(const Book& lhs, const Book& rhs) const {
        return lhs.read_count < rhs.read_count;
    }

    constexpr bool operator()(const Book& lhs, int rhs) const {
        return lhs.read_count < rhs;
    }
};

}  // namespace bookdb::comp