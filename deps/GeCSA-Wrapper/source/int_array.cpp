#include "gecsa/int_array.hpp"

#include <algorithm>
#include <bit>
#include <cassert>

#include "InArray.h"

#include "undef_damn_macros.hpp"

namespace gecsa {

IntArray::IntArray() = default;

IntArray::IntArray(const IntArray &other) {
    impl_ = std::make_unique<InArray>(other.size(), other.width());
    for (uindex i = 0; i < other.size(); ++i)
        set_value(i, other[i]);
}

IntArray::IntArray(IntArray &&) = default;

IntArray::IntArray(usize size, u32 width) {
    impl_ = std::make_unique<InArray>(size, width);
}

IntArray::IntArray(std::initializer_list<u64> values) {
    u64 min_val = std::min(values);
    u32 width = std::bit_width(min_val);
    impl_ = std::make_unique<InArray>(values.size(), width);
    for (uindex i = 0; auto val : values) {
        set_value(i++, val);
    }
}

IntArray::~IntArray() = default;

IntArray &IntArray::operator=(const IntArray &other) {
    IntArray cpy(other);
    swap(cpy);
    return *this;
}

IntArray &IntArray::operator=(IntArray &&) = default;

usize IntArray::size() const {
    return impl_->GetNum();
}

bool IntArray::empty() const {
    return size() == 0;
}

u32 IntArray::width() const {
    return impl_->GetDataWidth();
}

usize IntArray::size_in_bytes() const {
    return impl_->GetMemorySize();
}

u64 IntArray::get_value(uindex index) const {
    assert(index < size());
    return impl_->GetValue(index);
}

void IntArray::set_value(uindex index, u64 value) {
    assert(index < size());
    impl_->SetValue(index, value);
}

u64 IntArray::operator[](uindex index) const {
    assert(index < size());
    return get_value(index);
}

IntArray::Ref IntArray::operator[](uindex index) {
    assert(index < size());
    return Ref(this, index);
}

void IntArray::save(const std::string &path) const {
    savekit kit(path.c_str());
    impl_->write(kit);
}

void IntArray::save(FILE *file) const {
    savekit kit;
    kit.setfile(file);
    impl_->write(kit);
    kit.setfile();
}

void IntArray::swap(IntArray &other) { 
    impl_.swap(other.impl_);
}

IntArray IntArray::load_from_file(const std::string &path) {
    IntArray array;
    array.impl_ = std::make_unique<InArray>();
    loadkit kit(path.c_str());
    array.impl_->load(kit, 0);
    return std::move(array);
}

IntArray IntArray::load_from_file(FILE *file) {
    IntArray array;
    array.impl_ = std::make_unique<InArray>();
    loadkit kit;
    kit.setfile(file);
    array.impl_->load(kit, 0);
    kit.setfile();
    return std::move(array);
}

}