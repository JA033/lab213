#pragma once

#include <limits>
#include <string>
#include <string_view>
#include <vector>

#include "typedefs.hpp"

namespace wildcard {

struct WildcardGap {
    uindex min, max;

    WildcardGap(uindex min = 0, uindex max = std::numeric_limits<uindex>::max())
        : min(std::min(min, max)), 
          max(std::max(min, max)) { }
    
    [[nodiscard]] bool is_bounded() const { 
        return max != std::numeric_limits<uindex>::max();
    }
};

class WildcardPattern {
public:
    WildcardPattern() = default;
    WildcardPattern(const WildcardPattern &) = default;
    WildcardPattern(WildcardPattern &&) = default;

    WildcardPattern(std::string_view raw_pattern);
    WildcardPattern(const std::string &raw_pattern);
    WildcardPattern(const char *raw_pattern);

    WildcardPattern(const std::vector<ConstByteSpan> &subpatterns,
                    const std::vector<WildcardGap> &gaps);

    WildcardPattern &operator=(const WildcardPattern &) = default;
    WildcardPattern &operator=(WildcardPattern &&) = default;

    [[nodiscard]] const std::string &raw() const { return raw_; }
    [[nodiscard]] const std::vector<ConstByteSpan> &subpatterns() const { return subpatterns_; }
    [[nodiscard]] const std::vector<WildcardGap> &gaps() const { return gaps_; }
    [[nodiscard]] usize subpattern_count() const { return subpatterns().size(); }
    [[nodiscard]] usize gap_count() const { return gaps().size(); }

    [[nodiscard]] bool has_leading_wildcards() const;
    [[nodiscard]] bool has_trailing_wildcards() const;
    [[nodiscard]] bool is_exact() const;
    [[nodiscard]] bool is_wildcard_only() const;

private:
    std::string                raw_;
    std::vector<ByteSequence>  unescaped_subpatterns_;
    std::vector<ConstByteSpan> subpatterns_;
    std::vector<WildcardGap>   gaps_;
};

namespace literals {

inline WildcardPattern operator"" _wp(const char *raw_pattern, usize n) {
    return WildcardPattern(std::string_view{raw_pattern, n});
}

}

}