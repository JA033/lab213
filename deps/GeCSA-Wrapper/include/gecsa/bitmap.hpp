#pragma once

#include <cstdint>
#include <cstdlib>
#include <memory>

#include "int_types.hpp"

class InArray;

namespace gecsa {

class BitMap {
public:
    class Ref {
        friend class BitMap;
    public:
        Ref(const Ref &) = default;

        Ref &operator=(const Ref &other) { return (*this) = bool(other); }

        Ref &operator=(bool value) {
            bitmap_->set_value(index_, value);
            return *this;
        }

        [[nodiscard]] operator bool() const { return bitmap_->get_value(index_); }
        [[nodiscard]] bool operator!() const { return !bool(*this); }
        [[nodiscard]] bool unref() const { return bool(*this); }

        Ref &operator|=(bool value) { return (*this) = bool(*this) | value; }
        Ref &operator&=(bool value) { return (*this) = bool(*this) & value; }
        Ref &operator^=(bool value) { return (*this) = bool(*this) ^ value; }

        void swap(Ref other) {
            auto old_value = bool(*this);
            *this = bool(other);
            other = old_value;
        }

        void swap(bool &other) {
            auto old_value = bool(*this);
            *this = other;
            other = old_value;
        }

    private:
        BitMap   *bitmap_;
        uindex index_;

        Ref(BitMap *vector, uindex index)
            : bitmap_(vector), index_(index) { }
    };

public:
    BitMap();
    BitMap(const BitMap &other);
    BitMap(BitMap &&other);
    BitMap(usize size);
    BitMap(std::initializer_list<bool> values);
    ~BitMap();

    BitMap &operator=(const BitMap &other);
    BitMap &operator=(BitMap &&other);

    [[nodiscard]] usize size() const;
    [[nodiscard]] bool empty() const;
    [[nodiscard]] usize size_in_bytes() const;
    [[nodiscard]] const u32 *raw() const;
    [[nodiscard]] u32 *raw();

    [[nodiscard]] bool get_value(uindex index) const;
    void set_value(uindex index, bool value);
    [[nodiscard]] bool operator[](uindex index) const;
    [[nodiscard]] Ref operator[](uindex index);

    void save(const std::string &path) const;
    void save(FILE *file) const;

    void swap(BitMap &other);

    [[nodiscard]] static BitMap load_from_file(const std::string &path);
    [[nodiscard]] static BitMap load_from_file(FILE *file);

private:
    std::unique_ptr<InArray> impl_;
};

}