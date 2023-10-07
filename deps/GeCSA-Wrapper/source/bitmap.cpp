#include "gecsa/bitmap.hpp"

#include "InArray.h"
#include "undef_damn_macros.hpp"

namespace gecsa {

BitMap::BitMap() = default;

BitMap::BitMap(const BitMap &other) {
    impl_ = std::make_unique<InArray>(other.size(), 1);
    for (uindex i = 0; i < other.size(); ++i)
        set_value(i, other[i]);
}

BitMap::BitMap(BitMap &&) = default;

BitMap::BitMap(usize size) {
    impl_ = std::make_unique<InArray>(size, 1);
}

BitMap::BitMap(std::initializer_list<bool> values) {
    impl_ = std::make_unique<InArray>(values.size(), 1);
    for (uindex i = 0; bool val : values)
        set_value(i, val);
}

BitMap::~BitMap() = default;

BitMap &BitMap::operator=(const BitMap &other) {
    BitMap cpy(other);
    swap(cpy);
    return *this;
}

BitMap &BitMap::operator=(BitMap &&) = default;

usize BitMap::size() const {
    return impl_->GetNum();
}

bool BitMap::empty() const {
    return size() == 0;
}

usize BitMap::size_in_bytes() const {
    return impl_->GetMemorySize();
}

const u32 *BitMap::raw() const {
    return impl_->rawdata();
}

u32 *BitMap::raw() {
    return impl_->rawdata();
}

bool BitMap::get_value(uindex index) const {
    return impl_->GetValue(index);
}

void BitMap::set_value(uindex index, bool value) {
    impl_->SetValue(index, value);
}

bool BitMap::operator[](uindex index) const {
    return impl_->GetValue(index);
}

BitMap::Ref BitMap::operator[](uindex index) {
    return Ref(this, index);
}

void BitMap::save(const std::string &path) const {
    savekit kit(path.c_str());
    impl_->write(kit);
}

void BitMap::save(FILE *file) const {
    savekit kit;
    kit.setfile(file);
    impl_->write(kit);
    kit.setfile();
}

void BitMap::swap(BitMap &other) {
    impl_.swap(other.impl_);
}

BitMap BitMap::load_from_file(const std::string &path) {
    BitMap bitmap;
    bitmap.impl_ = std::make_unique<InArray>();
    loadkit kit(path.c_str());
    bitmap.impl_->load(kit, 0);
    return std::move(bitmap);
}

BitMap BitMap::load_from_file(FILE *file) {
    BitMap bitmap;
    bitmap.impl_ = std::make_unique<InArray>();
    loadkit kit;
    kit.setfile(file);
    bitmap.impl_->load(kit, 0);
    kit.setfile();
    return std::move(bitmap);
}

}