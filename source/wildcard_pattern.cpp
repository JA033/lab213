#include "wildcard_pattern.hpp"

#include <cassert>

#include "utils.hpp"

namespace wildcard {

static inline bool IsWildcardSymbol(char c) {
    return c == '?' || c == '*';
}

static std::string NormalizePattern(std::string_view raw_pattern) {
    std::string buf;
    buf.reserve(raw_pattern.size());
    for (uindex i = 0; i < raw_pattern.size(); ++i) {
        char c = raw_pattern[i];
        if (!buf.empty()) {
            if (c == '*') {
                while (buf.back() == '?' || buf.back() == '*')
                    buf.pop_back();
            } else if (c == '?') {
                if (buf.back() == '*')
                    continue;
            }
        }
        buf.push_back(c);
    }
    return buf;
}

static inline ByteSequence Unescape(ConstByteSpan str) {
    ByteSequence result;
    auto iter = str.begin();
    while (iter != str.end()) {
        if (*iter == '\\')
            ++iter;
        result.push_back(*iter);
        ++iter;
    }
    return result;
}

static inline WildcardGap ParseWildcardGap(std::string_view wildcards) {
    usize gap_min = 0, gap_max = 0;
    for (auto wildcard : wildcards) {
        assert(IsWildcardSymbol(wildcard));
        if (wildcard == '?')
            ++gap_min;
        else if (wildcard == '*')
            gap_max = std::numeric_limits<usize>::max();
    }
    if (gap_max != std::numeric_limits<usize>::max())
        gap_max = gap_min + 1;
    return WildcardGap{gap_min, gap_max};
}

WildcardPattern::WildcardPattern(std::string_view raw_pattern)
    : raw_(raw_pattern) {
    auto iter = raw_.begin();

    while (iter != raw_.end()) {
        auto subpattern_beg = iter;
        bool escaped = false;
        while (!IsWildcardSymbol(*iter) && iter != raw_.end()) {
            if (*iter == '\\') {
                ++iter;
                escaped = true;
            }
            ++iter;
        }
        if (iter != subpattern_beg) {
            auto subpattern = bytes_cast(std::string_view{subpattern_beg, iter});
            if (escaped) {
                unescaped_subpatterns_.push_back(Unescape(subpattern));
                subpatterns_.push_back(unescaped_subpatterns_.back());
            } else {
                subpatterns_.push_back(subpattern);
            }
        }

        auto wildcards_beg = iter;
        while (IsWildcardSymbol(*iter) && iter != raw_.end()) {
            ++iter;
        }
        if (iter != wildcards_beg) {
            gaps_.push_back(ParseWildcardGap(std::string_view{wildcards_beg, iter}));
        }
    }
}

WildcardPattern::WildcardPattern(const std::string &raw_pattern)
    : WildcardPattern(std::string_view{raw_pattern}) { }

WildcardPattern::WildcardPattern(const char *raw_pattern)
    : WildcardPattern(std::string_view{raw_pattern}) { }

bool WildcardPattern::has_leading_wildcards() const {
    return IsWildcardSymbol(raw().front());
}

bool WildcardPattern::has_trailing_wildcards() const {
    return IsWildcardSymbol(raw().back());
}

bool WildcardPattern::is_exact() const {
    return subpattern_count() == 1 && gap_count() == 0;
}

bool WildcardPattern::is_wildcard_only() const {
    return subpattern_count() == 0 && gap_count() == 1;
}


}
