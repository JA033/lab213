#pragma once

#include "gecsa/gecsa.hpp"
#include "gecsa/sparse_bitmap.hpp"

#include "utils.hpp"
#include "typedefs.hpp"

namespace wildcard {

// A complete document is of the form $data$, where $ is the separator. The
// `start` indicates the position of the first $ in the document and the `end`
// indicates the position of the last $ in the document.
struct Document {
    uindex id;
    uindex start;
    uindex end;

    [[nodiscard]] usize content_length() const {
        return end - start - 1;
    }
};

class DocumentLocation {
public:
    DocumentLocation(const Document *doc, uindex offset)
        : doc_(doc), offset_(offset) { }

    const Document &document() const { return *doc_; }
    uindex position() const { return document().start + offset_; }

private:
    const Document *doc_;
    uindex          offset_;
};

class DocumentLocateResult {
public:
    DocumentLocateResult() { }

    DocumentLocateResult(const DocumentLocateResult &) = default;
    DocumentLocateResult(DocumentLocateResult &&) = default;

    DocumentLocateResult(const std::vector<Document> &documents,
                         const std::vector<std::pair<uindex, uindex>> &locations)
        : docs_(documents), locs_(locations) { }

    DocumentLocateResult(std::vector<Document> &&documents,
                         std::vector<std::pair<uindex, uindex>> &&locations)
        : docs_(std::move(documents)), locs_(std::move(locations)) { }
    
    DocumentLocateResult &operator=(const DocumentLocateResult &) = default;
    DocumentLocateResult &operator=(DocumentLocateResult &&) = default;

    [[nodiscard]] usize size() const { return locs_.size(); }
    [[nodiscard]] const std::vector<Document> &documents() const { return docs_; }
    [[nodiscard]] const std::vector<std::pair<uindex, uindex>> &locations() const { return locs_; }

    [[nodiscard]] const Document &get_document(uindex loc_index) const {
        return docs_[locs_[loc_index].first];
    }

    [[nodiscard]] uindex get_position(uindex loc_index) const {
        return get_document(loc_index).start + locs_[loc_index].second;
    }

private:
    std::vector<Document>                  docs_;
    std::vector<std::pair<uindex, uindex>> locs_;
};

using DocumentEnumerator = Enumerator<Document, true>;
using DocumentLocationEnumerator = Enumerator<DocumentLocation, false>;

class DocumentSearcher {
public:
    [[nodiscard]] virtual usize document_count() const = 0;
    [[nodiscard]] virtual usize size_in_bytes() const = 0;

    [[nodiscard]] virtual std::unique_ptr<DocumentEnumerator>
    get_documents(SortOrder order = SortOrder::kAsc) const = 0;

    [[nodiscard]] virtual DocumentLocateResult
    locate_documents(uindex sa_l, uindex sa_r, SortOrder order = SortOrder::kAsc) const = 0;
    
    [[nodiscard]] virtual DocumentLocateResult
    locate_documents(ConstByteSpan pattern, SortOrder order = SortOrder::kAsc) const = 0;

    [[nodiscard]] virtual std::unique_ptr<DocumentLocationEnumerator>
    enumerate_documents(uindex sa_l, uindex sa_r, SortOrder order = SortOrder::kAsc) const = 0;

    [[nodiscard]] virtual std::unique_ptr<DocumentLocationEnumerator>
    enumerate_documents(ConstByteSpan pattern, SortOrder order = SortOrder::kAsc) const = 0;  
};

class CSADocumentSearcher : public DocumentSearcher {
public:
    CSADocumentSearcher(gecsa::GeCSA &&csa, gecsa::SparseBitMap &&doc_seps);    
    CSADocumentSearcher(const std::vector<ConstByteSpan> docs, u32 sample_rate = 32);

    [[nodiscard]] const gecsa::GeCSA &csa() const { return csa_; }
    [[nodiscard]] const gecsa::SparseBitMap &document_separators() const { return doc_seps_; };

    [[nodiscard]] usize document_count() const override { return doc_count_; }
    [[nodiscard]] usize size_in_bytes() const override { return csa_.size_in_bytes() + doc_seps_.size_in_bytes(); }

    [[nodiscard]] std::unique_ptr<DocumentEnumerator>
    get_documents(SortOrder order = SortOrder::kAsc) const override;

    [[nodiscard]] DocumentLocateResult
    locate_documents(uindex sa_l, uindex sa_r, SortOrder order = SortOrder::kAsc) const override;

    [[nodiscard]] DocumentLocateResult
    locate_documents(ConstByteSpan pattern, SortOrder order = SortOrder::kAsc) const override;

    [[nodiscard]] std::unique_ptr<DocumentLocationEnumerator>
    enumerate_documents(uindex sa_l, uindex sa_r, SortOrder order = SortOrder::kAsc) const override;

    [[nodiscard]] std::unique_ptr<DocumentLocationEnumerator>
    enumerate_documents(ConstByteSpan pattern, SortOrder order = SortOrder::kAsc) const override;  

private:
    usize               doc_count_;
    gecsa::GeCSA        csa_;
    gecsa::SparseBitMap doc_seps_;
};

}