#include "wildcard_executor.hpp"

#include <cassert>
#include <iostream>

#include "utils.hpp"
#include "timer.hpp"

namespace wildcard {

static const std::array<u8, 1> kEofSeq {0};

[[nodiscard]] std::pair<uindex, uindex>
SuffixRangeWithFrontSep(const gecsa::GeCSA &csa, ConstByteSpan p) {
    auto suf_range = csa.suffix_range(p);
    suf_range = csa.suffix_range(suf_range.first, suf_range.second, kEofSeq);
    return suf_range;
}

[[nodiscard]] std::pair<uindex, uindex>
SuffixRangeWithBackSep(const gecsa::GeCSA &csa, ConstByteSpan p) {
    auto suf_range = csa.alphabet_range(0);
    suf_range = csa.suffix_range(suf_range.first, suf_range.second, p);
    return suf_range;
}

struct Interval {
    uindex min, max;

    [[nodiscard]] bool Empty() const {
        return max <= min;
    }

    [[nodiscard]] bool Contains(uindex index) const {
        return (index - min) < (max - min);
    }

    Interval &Merge(const Interval &other) {
        min = std::min(min, other.min);
        max = std::max(max, other.max);
        return *this;
    }
};

template<bool Forward>
class AcceptableIntervalList {
public:
    [[nodiscard]] usize Size() const { return intervals_.size(); }
    [[nodiscard]] bool Empty() const { return Size() == 0; }
    [[nodiscard]] const Interval &At(uindex index) const { return intervals_[index]; }
    [[nodiscard]] const Interval &Front() const { return intervals_.front(); }
    [[nodiscard]] const Interval &Back() const { return intervals_.back(); }
    [[nodiscard]] const std::vector<Interval> &Intervals() const { return intervals_; }

    // For forward matching, the pos must in 
    bool ValidateSubpattern(ConstByteSpan subpattern, uindex pos) const {
        return !intervals_.empty() && intervals_.front().Contains(pos);
    }

    // Remove all intervals whose maximum value is less than the hint.
    void RemoveDeadIntervals(uindex hint) {
        auto iter = intervals_.begin();
        if constexpr (Forward) {
            iter = lower_bound(intervals_, hint, std::less<>{}, [](const Interval &v) { return v.max; });
        } else {
            iter = upper_bound(intervals_, hint, std::greater<>{}, [](const Interval &v) { return v.min; });
        }
        intervals_.erase(intervals_.begin(), iter);
    }

    void Add(ConstByteSpan subpattern, WildcardGap gap, const Document &doc, uindex pos) {
        Interval interval;
        if constexpr (Forward) {
            // `pos` is the position of the first character of the subpattern.
            interval.min = pos + subpattern.size() + gap.min;
            interval.max = gap.is_bounded() ? pos + subpattern.size() + gap.max : gap.max;
            interval.max = std::min(interval.max, doc.end);

            if (interval.Empty())
                return;

            if (!intervals_.empty() && interval.min <= intervals_.back().max) {
                intervals_.back().Merge(interval);
            } else {
                intervals_.push_back(interval);
            }
        } else {
            // `pos` is the position of the last character of the subpattern.
            sindex min = gap.is_bounded() ? pos - subpattern.size() - gap.max : 0;
            min = std::max(min, static_cast<ssize>(doc.start + 1));
            sindex max = pos - subpattern.size() - gap.min;
            max = std::max(max, static_cast<sindex>(0));

            interval.min = min;
            interval.max = max;
            
            if (interval.Empty())
                return;
            
            if (!intervals_.empty() && interval.max >= intervals_.back().min) {
                intervals_.back().Merge(interval);
            } else {
                intervals_.push_back(interval);
            }
        }
    }

    void Clear() {
        intervals_.clear();
    }

    void Swap(AcceptableIntervalList &other) {
        intervals_.swap(other.intervals_);
    }

private:
    std::vector<Interval> intervals_;
};

template<bool Forward>
void FindAllValidLocationsV1(ConstByteSpan subpattern, WildcardGap gap,
          DocumentLocationEnumerator *enumerator,
          const AcceptableIntervalList<Forward> &in,
          AcceptableIntervalList<Forward> &out) {
    for (uindex i = 0; i < in.Size(); ++i) {
        const Interval &interval = in.At(i);
        // 1. find leftmost location whose position is greater than or equal to
        //    interval.min
        
    }
}

template<bool Forward>
void FindAllValidLocationsV2(ConstByteSpan subpattern,
                           WildcardGap gap,
                           DocumentLocationEnumerator *enumerator, 
                           const AcceptableIntervalList<Forward> &in,
                           AcceptableIntervalList<Forward> &out) {
    auto iter = in.Intervals().begin();
    auto iter_end = in.Intervals().end();
    enumerator->next();
    while (enumerator->has_next() && iter != iter_end) {
        DocumentLocation loc = enumerator->current();
        uindex pos = loc.position();
        if (pos < iter->min) {
            enumerator->next(); // Discard position.
        } else if (pos >= iter->max) {
            ++iter; // Discard interval.
        } else {
            out.Add(subpattern, gap, loc.document(), pos);
            enumerator->next();
        }
    }
}

template<bool Forward>
void FindAllValidLocationsV2(ConstByteSpan subpattern,
                           DocumentLocationEnumerator *enumerator, 
                           const AcceptableIntervalList<Forward> &in,
                           std::vector<uindex> &out) {
    auto iter = in.Intervals().begin();
    enumerator->next();
    while (enumerator->has_next() && iter != in.Intervals().end()) {
        DocumentLocation loc = enumerator->current();
        uindex pos = loc.position();
        if (pos < iter->min) {
            enumerator->next(); // Discard position.
        } else if (pos >= iter->max) {
            ++iter; // Discard interval.
        } else {
            out.push_back(loc.document().id);
            enumerator->next();
        }
    }
}

template<bool Forward>
static void FindAllValidLocations() {

}

[[nodiscard]] static usize 
ExactCount(const CSADocumentSearcher &searcher, const WildcardPattern &pattern) {
    assert(pattern.is_exact());
    const auto &csa = searcher.csa();

    auto suf_range = csa.alphabet_range(0);
    suf_range = csa.suffix_range(pattern.subpatterns().front());
    suf_range = csa.suffix_range(suf_range.first, suf_range.second, kEofSeq);
    return suf_range.second - suf_range.first;
}

[[nodiscard]] static std::vector<uindex> 
ExactMatch(const CSADocumentSearcher &searcher, const WildcardPattern &pattern) {
    assert(pattern.is_exact());
    const auto &csa = searcher.csa();

    auto suf_range = csa.alphabet_range(0);
    suf_range = csa.suffix_range(pattern.subpatterns().front());
    suf_range = csa.suffix_range(suf_range.first, suf_range.second, kEofSeq);
    DocumentLocateResult result = searcher.locate_documents(suf_range.first, suf_range.second);

    std::vector<uindex> final_result(result.documents().size());
    for (uindex i = 0; i < final_result.size(); ++i)
        final_result[i] = result.documents()[i].id;
    return final_result;
}

[[nodiscard]] static usize
WildcardOnlyCount(const CSADocumentSearcher &searcher, const WildcardPattern &pattern) {
    assert(pattern.is_wildcard_only());
    WildcardGap gap = pattern.gaps().front();
    auto doc_enum = searcher.get_documents();

    usize result = 0;
    while (doc_enum->next()) {
        Document doc = doc_enum->current();
        if (in_range(doc.content_length(), gap.min, gap.max))
            ++result;
    }
    return result;
}

[[nodiscard]] static std::vector<uindex> 
WildcardOnlyMatch(const CSADocumentSearcher &searcher, const WildcardPattern &pattern) {
    assert(pattern.is_wildcard_only());
    WildcardGap gap = pattern.gaps().front();
    auto doc_enum = searcher.get_documents();

    std::vector<uindex> result;
    while (doc_enum->next()) {
        Document doc = doc_enum->current();
        if (in_range(doc.content_length(), gap.min, gap.max))
            result.push_back(doc.id);
    }
    return result;
}

[[nodiscard]] static std::vector<uindex>
ForwardMatch(const CSADocumentSearcher &searcher, const WildcardPattern &pattern) {
    assert(!pattern.has_leading_wildcards());

    const auto &csa = searcher.csa();
    const auto &subpatterns = pattern.subpatterns();
    const auto &gaps = pattern.gaps();
    AcceptableIntervalList<true> cur_list, nxt_list;

    auto start = start_clock();

    // Initialize first subpattern.
    {
        ConstByteSpan subpattern = subpatterns.front();
        WildcardGap gap = gaps.front();

        auto suf_range = SuffixRangeWithFrontSep(csa, subpattern);

        //auto start2 = start_clock();
        auto iter = searcher.enumerate_documents(suf_range.first, suf_range.second);
        //auto end2 = end_clock();
        //std::cout << "itime: " << get_time(start2, end2, TimeUnit::kSeconds) << "s\n";

        while (iter->next()) {
            DocumentLocation loc = iter->current();
            cur_list.Add(subpattern, gap, loc.document(), loc.position() + 1);
        }
    }

    auto end = end_clock();
    std::cout << "initialize time: " << get_time(start, end, TimeUnit::kSeconds) << "s\n";

    start = start_clock();
    usize subpattern_count = subpatterns.size() - (!pattern.has_trailing_wildcards());
    for (uindex i = 1; i < subpattern_count; ++i) {
        ConstByteSpan subpattern = subpatterns[i];
        WildcardGap gap = gaps[i];

        nxt_list.Clear();

        //DocumentLocateResult result = searcher.locate_documents(subpattern);

        auto loc_enum = searcher.enumerate_documents(subpattern);
        if (!loc_enum->has_next())
            return {};
        FindAllValidLocationsV2(subpattern, gap, loc_enum.get(), cur_list, nxt_list);

        /*for (uindex j = 0; !cur_list.Empty() && j < result.size(); ++j) {
            uindex pos = result.get_position(j);
            cur_list.RemoveDeadIntervals(pos);
            if (cur_list.ValidateSubpattern(subpattern, pos))
                nxt_list.Add(subpattern, gap, result.get_document(j), pos);
        }*/
        cur_list.Swap(nxt_list);
    }
    end = end_clock();
    std::cout << "middle time: " << get_time(start, end, TimeUnit::kSeconds) << "s\n";

    std::vector<uindex> final_result;

    start = start_clock();

    if (pattern.has_trailing_wildcards()) {
        const auto &seps = searcher.document_separators();

        for (uindex i = 0; i < cur_list.Size(); ++i) {
            uindex max = cur_list.At(i).max - 1;
            if (seps[max]) {
                uindex id = seps.rank1(max) - 1;
                final_result.push_back(id);
            }
        }

        /*uindex search_start = seps.rank1(cur_list.At(0).min + 1);

        for (uindex i = search_start; !cur_list.Empty() && i <= searcher.document_count(); ++i) {
            uindex pos = seps.select1(i);
            cur_list.RemoveDeadIntervals(pos);
            if (cur_list.ValidateSubpattern({}, pos)) {
                final_result.push_back(i - 1);
            }
        }*/
    } else {
        ConstByteSpan subpattern = subpatterns.back();
        auto suf_range = SuffixRangeWithBackSep(csa, subpattern);

        auto loc_enum = searcher.enumerate_documents(suf_range.first, suf_range.second);
        FindAllValidLocationsV2(subpattern, loc_enum.get(), cur_list, final_result);

        /*DocumentLocateResult result = searcher.locate_documents(suf_range.first, suf_range.second);

        for (uindex i = 0; !cur_list.Empty() && i < result.size(); ++i) {
            uindex pos = result.get_position(i);
            cur_list.RemoveDeadIntervals(pos);
            if (cur_list.ValidateSubpattern(subpattern, pos)) {
                final_result.push_back(result.get_document(i).id);
            }
        }*/
    }

    end = end_clock();
    std::cout << "finalize time: " << get_time(start, end, TimeUnit::kSeconds) << "s\n";

    return final_result;
}

[[nodiscard]] static std::vector<uindex>
BackwardMatch(const CSADocumentSearcher &searcher, const WildcardPattern &pattern) {
    const auto &csa = searcher.csa();
    AcceptableIntervalList<false> cur_list, nxt_list;

    const auto &subpatterns = pattern.subpatterns();
    const auto &gaps_ref = pattern.gaps();

    auto gaps = std::span{gaps_ref};

    if (pattern.has_trailing_wildcards()) {
        gaps = gaps.first(gaps.size() - 1);
    }

    // Initialize first subpattern.
    {
        ConstByteSpan subpattern = subpatterns.back();
        WildcardGap gap = gaps.back();

        if (pattern.has_trailing_wildcards()) {
            DocumentLocateResult result = searcher.locate_documents(subpattern, SortOrder::kDesc);
            WildcardGap last_gap = gaps_ref.back();
            for (uindex i = 0; i < result.size(); ++i) {
                const Document &doc = result.get_document(i);
                uindex pos = result.get_position(i) + subpattern.size();
                usize dis = doc.end - pos - 1;
                if (in_range(dis, last_gap.min, last_gap.max))
                    cur_list.Add(subpattern, gap, doc, pos);
            }
        } else {
            auto suf_range = SuffixRangeWithBackSep(csa, subpattern);
            DocumentLocateResult result = searcher.locate_documents(suf_range.first, suf_range.second, SortOrder::kDesc);
            for (uindex i = 0; i < result.size(); ++i) {
                uindex pos = result.get_position(i) + subpattern.size();
                cur_list.Add(subpattern, gap, result.get_document(i), pos);
            }
        }
    }

    for (sindex i = subpatterns.size() - 2; i >= 0; --i) {
        ConstByteSpan subpattern = subpatterns[i];
        WildcardGap gap = gaps[i];

        nxt_list.Clear();

        DocumentLocateResult result = searcher.locate_documents(subpattern, SortOrder::kDesc);
        for (uindex j = 0; !cur_list.Empty() && j < result.size(); ++j) {
            uindex pos = result.get_position(j) + subpattern.size();
            cur_list.RemoveDeadIntervals(pos);
            if (cur_list.ValidateSubpattern(subpattern, pos - 1))
                nxt_list.Add(subpattern, gap, result.get_document(j), pos);
        }
        cur_list.Swap(nxt_list);
    }

    std::vector<uindex> final_result;

    if (pattern.has_leading_wildcards()) {
        auto gap = gaps.front();
        const auto &seps = searcher.document_separators();

        for (uindex i = 0; i < cur_list.Size(); ++i) {
            uindex min = cur_list.At(i).min;
            uindex id = seps.rank1(min) - 1;
            if (seps[min - 1])
                final_result.push_back(id);
        }
    } else {
        ConstByteSpan subpattern = subpatterns.front();
        auto suf_range = SuffixRangeWithFrontSep(csa, subpattern);

        DocumentLocateResult result = searcher.locate_documents(suf_range.first, suf_range.second, SortOrder::kDesc);

        for (uindex i = 0; !cur_list.Empty() && i < result.size(); ++i) {
            uindex pos = result.get_position(i) + subpattern.size();
            cur_list.RemoveDeadIntervals(pos);
            if (cur_list.ValidateSubpattern(subpattern, pos - 1)) {
                final_result.push_back(result.get_document(i).id);
            }
        }
    }

    std::reverse(final_result.begin(), final_result.end());
    
    return final_result;
}

CSAWildcardExecutor::CSAWildcardExecutor() { }

std::vector<uindex> 
CSAWildcardExecutor::match(const CSADocumentSearcher &searcher, const WildcardPattern &pattern) {    
    if (pattern.is_exact())
        return ExactMatch(searcher, pattern);
    else if (pattern.is_wildcard_only())
        return WildcardOnlyMatch(searcher, pattern);
    else if (pattern.has_leading_wildcards())
        return BackwardMatch(searcher, pattern);
    return ForwardMatch(searcher, pattern);
}

}