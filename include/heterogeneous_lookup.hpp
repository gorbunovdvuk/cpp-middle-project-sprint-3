#pragma once

#include <string>
#include <string_view>

namespace bookdb {

struct TransparentStringLess {
    using is_transparent = std::true_type;

    bool operator()(std::string_view lhs, std::string_view rhs) const { return lhs.compare(rhs) < 0; }
};

struct TransparentStringEqual {
    using is_transparent = std::true_type;

    bool operator()(std::string_view lhs, std::string_view rhs) const { return lhs == rhs; }
};

struct TransparentStringHash {
    using is_transparent = std::true_type;

    std::size_t operator()(std::string_view str) const { return std::hash<std::string_view>{}(str); }
};

}  // namespace bookdb
