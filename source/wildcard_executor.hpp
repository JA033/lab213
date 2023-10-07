#include <deque>
#include <unordered_map>

#include "gecsa/gecsa.hpp"

#include "document_searcher.hpp"
#include "wildcard_pattern.hpp"

namespace wildcard {

class WildcardExecutor {
public:
    [[nodiscard]] virtual usize count(const CSADocumentSearcher &searcher, const WildcardPattern &pattern) = 0;
    [[nodiscard]] virtual std::vector<uindex> match(const CSADocumentSearcher &searcher, const WildcardPattern &pattern) = 0;

    virtual usize exists(const CSADocumentSearcher &searcher, const WildcardPattern &pattern) {
        return count(searcher, pattern) != 0;
    }
};

/*class WildcardExecutorStrategy {
public:
    [[nodiscard]] virtual usize count(const CSADocumentSearcher &searcher, const WildcardPattern &pattern) = 0;
    [[nodiscard]] virtual std::vector<uindex> match(const CSADocumentSearcher &searcher, const WildcardPattern &pattern) = 0;
    [[nodiscard]] virtual u32 score(const WildcardPattern &pattern) const;
};

class AdaptiveWildcardExcutor : public WildcardExecutor {
public:
    std::vector<uindex> match(const CSADocumentSearcher &searcher, const WildcardPattern &pattern) override {
        u32 best_score = 0;
        WildcardExecutorStrategy *best = nullptr;
        for (WildcardExecutorStrategy *strategy : strategies_) {
            u32 score = strategy->score(pattern);
            if (score > best_score) {
                best_score = score;
                best = strategy;
            }
        }
        return best->match(searcher, pattern);
    }

private:
    std::vector<WildcardExecutorStrategy *> strategies_;
};*/

class CSAWildcardExecutor : public WildcardExecutor {
public:
    CSAWildcardExecutor();

    [[nodiscard]] usize count(const CSADocumentSearcher &searcher, const WildcardPattern &pattern) override { return 0; }
    [[nodiscard]] std::vector<uindex> match(const CSADocumentSearcher &searcher, const WildcardPattern &pattern) override;

private:
};

}