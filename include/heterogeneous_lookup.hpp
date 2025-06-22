#pragma once

#include <string>
#include <string_view>

namespace bookdb {

struct TransparentStringLess {
    using is_transparent = std::true_type;

    bool operator()(const std::unique_ptr<std::string>& lhs, std::string_view rhs) const { return *lhs < rhs; }
    bool operator()(const std::unique_ptr<std::string>& lhs, const std::unique_ptr<std::string>& rhs) const { return *lhs < *rhs; }
    bool operator()(std::string_view lhs, std::string_view rhs) const { return lhs < rhs; }
    bool operator()(std::string_view lhs, const std::unique_ptr<std::string>& rhs) const { return lhs < *rhs; }
};

struct TransparentStringEqual {
    using is_transparent = std::true_type;

    bool operator()(const std::unique_ptr<std::string>& lhs, std::string_view rhs) const { return *lhs == rhs; }
    bool operator()(std::string_view lhs, const std::unique_ptr<std::string>& rhs) const { return lhs == *rhs; }
    bool operator()(std::string_view lhs, std::string_view rhs) const { return lhs == rhs; }
    bool operator()(const std::unique_ptr<std::string>& lhs, const std::unique_ptr<std::string>& rhs) const { return *lhs == *rhs; }
};

struct TransparentStringHash {
    using is_transparent = std::true_type;

    std::size_t operator()(std::string_view str) const { return std::hash<std::string_view>{}(str); }
    std::size_t operator()(const std::string& str) const { return std::hash<std::string_view>{}(str); }
    std::size_t operator()(const std::unique_ptr<std::string>& str) const { return std::hash<std::string_view>{}(*str); }
};

}  // namespace bookdb
