#pragma once

#include <algorithm>
#include <ranges>
#include <string>
#include <string_view>

#include "typedefs.hpp"

namespace wildcard {

template<typename T, bool Referrable>
class Enumerator {
public:
    using reference = std::conditional_t<Referrable, const T &, T>;

    [[nodiscard]] virtual bool has_next() const = 0;
    [[nodiscard]] virtual reference current() const = 0;
    virtual bool next() = 0;
};

template<typename T, typename HintT, bool Referrable>
class SearchableEnumerator : public Enumerator<T, Referrable> {
public:
    virtual bool next_lower_bound(const HintT &hint) = 0;
    virtual bool next_upper_bound(const HintT &hint) = 0;
};

[[nodiscard]] inline constexpr const ubyte8 *bytes_cast(const char *s) {
    return static_cast<const ubyte8 *>(static_cast<const void *>(s));
}

[[nodiscard]] inline constexpr ubyte8 *bytes_cast(char *s) {
    return static_cast<ubyte8 *>(static_cast<void *>(s));
}

[[nodiscard]] inline constexpr ByteSpan bytes_cast(std::span<char> s) {
    return ByteSpan(bytes_cast(s.data()), s.size());
}

[[nodiscard]] inline constexpr ConstByteSpan bytes_cast(std::span<const char> s) {
    return ConstByteSpan(bytes_cast(s.data()), s.size());
}

template<std::integral T, std::integral U>
[[nodiscard]] inline constexpr bool in_range(const T x, const U min, const U max) {
    using UnsignedT = std::make_unsigned_t<T>;
    using UnsignedU = std::make_unsigned_t<T>;
    return (static_cast<UnsignedT>(x) - static_cast<UnsignedU>(min)) <
           (static_cast<UnsignedU>(max) - static_cast<UnsignedU>(min));
}

template<typename T>
[[nodiscard]] inline constexpr std::span<T> remove_span_first(std::span<T> span, usize n) {
    return span.subspan(n);
}

template<typename T>
[[nodiscard]] inline constexpr std::span<T> remove_span_last(std::span<T> span, usize n) {
    return span.first(span.size() - n);
}

template<std::integral T, typename U = std::make_unsigned_t<T>>
[[nodiscard]] inline constexpr std::span<U>
span_make_unsigned(std::span<T> span) {
    return std::span<U>{static_cast<U *>(static_cast<void *>(span.data())), span.size()};
}

template<std::integral T, typename U = std::make_unsigned_t<T>>
[[nodiscard]] inline constexpr std::span<const U>
span_make_unsigned(std::span<const T> span) {
    return std::span<const U>{static_cast<const U *>(static_cast<const void *>(span.data())), span.size()};
}

template<std::integral T, typename U = std::make_signed_t<T>>
[[nodiscard]] inline constexpr std::span<U>
span_make_signed(std::span<T> span) {
    return std::span<U>{static_cast<U *>(static_cast<void *>(span.data())), span.size()};
}

template<std::integral T, typename U = std::make_signed_t<T>>
[[nodiscard]] inline constexpr std::span<const U>
span_make_signed(std::span<const T> span) {
    return std::span<const U>{static_cast<const U *>(static_cast<const void *>(span.data())), span.size()};
}

template<std::ranges::forward_range RangeT, typename T, 
         typename CmpT = std::less<>, typename ProjT = std::identity,
         usize Threshold = 32>
[[nodiscard]] inline constexpr auto 
lower_bound(RangeT &&range, const T &value, CmpT &&cmp = {}, ProjT &&proj = {}, std::integral_constant<usize, Threshold> = {}) {
    usize size = std::ranges::size(range);
    if (size <= Threshold) {
        return std::ranges::find_if_not(range, [&](const auto &x) -> bool { return cmp(proj(x), value); });
    } else {
        return std::ranges::lower_bound(range, value, cmp, proj);
    }
}

template<std::input_iterator InputIterT, typename T, 
         typename CmpT = std::less<>, typename ProjT = std::identity,
         usize Threshold = 32>
[[nodiscard]] inline constexpr auto
lower_bound(InputIterT first, InputIterT last, const T &value, CmpT &&cmp = {}, ProjT &&proj = {}) {
    return lower_bound(std::ranges::subrange(first, last), value, cmp, proj, std::integral_constant<usize, Threshold>{});
}

template<std::ranges::forward_range RangeT, typename T, 
         typename CmpT = std::less<>, typename ProjT = std::identity,
         usize Threshold = 32>
[[nodiscard]] inline constexpr auto
upper_bound(RangeT &&range, const T &value, CmpT &&cmp = {}, ProjT &&proj = {}, std::integral_constant<usize, Threshold> = {}) {
    usize size = std::ranges::size(range);
    if (size <= Threshold) {
        return std::ranges::find_if(range, [&](const auto &x) -> bool { return cmp(value, proj(x)); });
    } else {
        return std::ranges::upper_bound(range, value, cmp, proj);
    }
}

template<std::input_iterator InputIterT, typename T, 
         typename CmpT = std::less<>, typename ProjT = std::identity,
         usize Threshold = 32>
[[nodiscard]] inline constexpr auto
upper_bound(InputIterT first, InputIterT last, const T &value, CmpT &&cmp = {}, ProjT &&proj = {}) {
    return upper_bound(std::ranges::subrange(first, last), value, cmp, proj, std::integral_constant<usize, Threshold>{});
}

template<std::ranges::forward_range RangeT, typename T, typename CmpT = std::less<>, typename ProjT = std::identity>
[[nodiscard]] inline constexpr auto
find_bounds(RangeT &&range, const T &min, const T &max, CmpT &&cmp = {}, ProjT &&proj = {}) {
    return std::pair{lower_bound(range, min, cmp, proj),
                     upper_bound(range, max, cmp, proj)};
}

template<typename InputIterT, typename T, typename CmpT = std::less<>, typename ProjT = std::identity>
[[nodiscard]] inline constexpr auto
find_bounds(InputIterT first, InputIterT last, const T &min, const T &max, CmpT &&cmp = {}, ProjT &&proj = {}) {
    return std::pair{lower_bound(first, last, min, cmp, proj),
                     upper_bound(first, last, max, cmp, proj)};
}

template<typename T>
[[nodiscard]] inline constexpr std::pair<T, T> idiv(T a, T b) {
    return {a / b, a % b};
}

template<typename T> 
[[nodiscard]] inline constexpr T div_ceil(T a, T b) { 
    return a == 0 ? 0 : (a -  1) / b + 1;
}

[[nodiscard]] ByteSequence 
join_strs(std::vector<ConstByteSpan> strs, ubyte8 delimiter, bool prepend = true, bool append = true,
          std::vector<uindex> *delimiter_pos = nullptr);

void mem_prefetch(const void *ptr);

[[nodiscard]] u32 select1(u32 block, u32 k);
[[nodiscard]] u32 select1(u64 block, u32 k);

}