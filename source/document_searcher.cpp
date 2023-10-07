#include "document_searcher.hpp"

#include <cassert>
#include <cstring>
#include <iostream> // FIXME: Delete. 
#include <iomanip>

namespace wildcard {

class DocumentListEnumerator : public DocumentEnumerator {
public:
    DocumentListEnumerator(std::vector<Document> &&list)
        : index_(-1), list_(std::move(list)) { }

    [[nodiscard]] bool has_next() const override {
        return index_ != list_.size();
    }

    [[nodiscard]] reference current() const override {
        assert(index_ >= 0 && index_ < list_.size());
        return list_[index_];
    }

    bool next() override {
        ++index_;
        return has_next();
    }

private:
    sindex                index_;
    std::vector<Document> list_;
};

class DocumentLocationListEnumerator : public DocumentLocationEnumerator {
    using Location = std::pair<uindex, uindex>;
public:
    DocumentLocationListEnumerator(DocumentLocateResult &&list)
        : index_(-1), list_(std::move(list)) { }

    [[nodiscard]] reference current() const override {
        assert(index_ >= 0 && index_ < list_.size());
        Location loc = list_.locations()[index_];
        return DocumentLocation(&list_.documents()[loc.first], loc.second);
    }

    [[nodiscard]] bool has_next() const override {
        return index_ != list_.size();
    }

    bool next() override {
        ++index_;
        return index_ != list_.size();
    }

private:
    sindex               index_;
    DocumentLocateResult list_;
};

CSADocumentSearcher::CSADocumentSearcher(gecsa::GeCSA &&csa, gecsa::SparseBitMap &&doc_seps)
    : csa_(std::move(csa)), doc_seps_(std::move(doc_seps)) {
    doc_count_ = doc_seps_.rank1(doc_seps_.size());
}

CSADocumentSearcher::CSADocumentSearcher(const std::vector<ConstByteSpan> docs, u32 sample_rate)
    : doc_count_(docs.size()) {
    usize size_in_bytes = 0;
    for (const auto &doc : docs) {
        size_in_bytes += doc.size();
    }
    size_in_bytes += document_count() + 1;
    auto buf = std::make_unique<u8 []>(size_in_bytes);
    uindex buf_writer_offset = 0;

    std::vector<uindex> sep_positions;

    auto write_sep = [&]() { 
        sep_positions.push_back(buf_writer_offset);
        buf[buf_writer_offset++] = 0;
    };
    auto write_data = [&](ConstByteSpan bytes) { 
        memcpy(buf.get() + buf_writer_offset, bytes.data(), bytes.size());
        buf_writer_offset += bytes.size();
    };

    for (const auto &doc : docs) {
        write_sep();
        write_data(doc);
    }
    write_sep();

    csa_ = gecsa::GeCSA::create_from_str({buf.get(), size_in_bytes}, 256, sample_rate);
    doc_seps_ = gecsa::SparseBitMap(size_in_bytes, sep_positions);

    /*auto range = csa_.suffix_range(std::string_view{"/"});
    std::cout << "range.first: " << range.first << " " << "range.second: " << range.second << "\n";

    std::cout << "check start\n";
    auto dd = csa_.locate(std::string_view{"/"});

    for (uindex i = 0; i < dd.size(); ++i) {
        if (buf[dd[i]] != '/') {
            std::cout << "check err: " << i << " " << dd[i] << "\"" << buf[dd[i]] << "\"\n";
        }
    }
    std::cout << "check finished\n";*/
}

std::unique_ptr<DocumentEnumerator>
CSADocumentSearcher::get_documents(SortOrder order) const {
    if (document_count() == 0)
        return {};

    std::vector<Document> docs(document_count());
    docs[0].id = 0;
    docs[0].start = doc_seps_.select1(0);

    for (uindex i = 1; i < document_count(); ++i) {
        docs[i].id = i;
        docs[i].start = doc_seps_.select1(i);
        docs[i - 1].end = docs[i].start;
    }
    docs.back().end = doc_seps_.select1(document_count());
    return std::make_unique<DocumentListEnumerator>(std::move(docs));
}

DocumentLocateResult 
CSADocumentSearcher::locate_documents(uindex sa_l, uindex sa_r, SortOrder order) const {
    assert(order != SortOrder::kNone);
    auto positions = csa_.suffix_range_to_indices({sa_l, sa_r}, order);

    std::vector<Document> docs;
    std::vector<std::pair<uindex, uindex>> locs(positions.size());

    for (uindex i = 0; i < positions.size(); ++i) {
        uindex position = positions[i];
        uindex doc_idx = doc_seps_.rank1(position + 1);
        doc_idx = doc_idx == document_count() + 1 ? document_count() : doc_idx;
        if (docs.empty() || docs.back().id != doc_idx - 1) {
            uindex doc_start = doc_seps_.select1(doc_idx - 1);
            uindex doc_end = doc_seps_.select1(doc_idx) + 1;
            docs.push_back({doc_idx - 1, doc_start, doc_end});
        }
        locs[i].first = docs.size() - 1;
        locs[i].second = position - docs.back().start;
    }
    return {std::move(docs), std::move(locs)};
}

DocumentLocateResult CSADocumentSearcher::locate_documents(ConstByteSpan pattern, SortOrder order) const {
    auto suf_range = csa_.suffix_range(pattern);
    return locate_documents(suf_range.first, suf_range.second, order);
}

std::unique_ptr<DocumentLocationEnumerator>
CSADocumentSearcher::enumerate_documents(uindex sa_l, uindex sa_r, SortOrder order) const {
    DocumentLocateResult result = locate_documents(sa_l, sa_r, order);
    return std::make_unique<DocumentLocationListEnumerator>(std::move(result));
}

std::unique_ptr<DocumentLocationEnumerator>
CSADocumentSearcher::enumerate_documents(ConstByteSpan pattern, SortOrder order) const {
    DocumentLocateResult result = locate_documents(pattern, order);
    return std::make_unique<DocumentLocationListEnumerator>(std::move(result));
}

}