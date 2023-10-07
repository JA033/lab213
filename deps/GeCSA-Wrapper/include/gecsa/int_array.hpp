#pragma once

#include <memory>

#include "int_types.hpp"

class InArray;

namespace gecsa {

class IntArray {
public:
    class Ref {
        friend class IntArray;
    public:
        Ref(const Ref &) = default;

        Ref &operator=(const Ref &other) {
            arr_->set_value(index_, u64(other));
            return *this;
        }

        Ref &operator=(u64 value) {
            arr_->set_value(index_, value);
            return *this;
        }

        operator u64() const {
            return arr_->get_value(index_);
        }

        Ref &operator++() {
            arr_->set_value(index_, u64(*this) + 1);
            return *this;
        }

        Ref operator++(i32) {
            Ref cpy(*this);
            arr_->set_value(index_, u64(*this) + 1);
            return cpy;
        }

        Ref &operator--() {
            arr_->set_value(index_, u64(*this) - 1);
            return *this;
        }

        Ref operator--(i32) {
            Ref cpy(*this);
            arr_->set_value(index_, u64(*this) - 1);
            return cpy;
        }

        Ref &operator+=(u64 v) {
            arr_->set_value(index_, u64(*this) + v);
            return *this;
        }

        Ref &operator-=(u64 v) {
            arr_->set_value(index_, u64(*this) - v);
            return *this;
        }

        Ref &operator*=(u64 v) {
            arr_->set_value(index_, u64(*this) * v);
            return *this;
        }

        Ref &operator/=(u64 v) {
            arr_->set_value(index_, u64(*this) / v);
            return *this;
        }

        Ref &operator%=(u64 v) {
            arr_->set_value(index_, u64(*this) % v);
            return *this;
        }

        Ref &operator<<=(u64 v) {
            arr_->set_value(index_, u64(*this) << v);
            return *this;
        }

        Ref &operator>>=(u64 v) {
            arr_->set_value(index_, u64(*this) >> v);
            return *this;
        }

        Ref &operator&=(u64 v) {
            arr_->set_value(index_, u64(*this) & v);
            return *this;
        }

        Ref &operator|=(u64 v) {
            arr_->set_value(index_, u64(*this) | v);
            return *this;
        }

        Ref &operator^=(u64 v) {
            arr_->set_value(index_, u64(*this) ^ v);
            return *this;
        }

        void swap(Ref other) {
            u64 old_value = u64(*this);
            *this = u64(other);
            other = old_value;
        }

        void swap(u64 &other) {
            u64 old_value = u64(*this);
            *this = u64(other);
            other = old_value;    
        }

    private:
        IntArray *arr_;
        uindex    index_;

        Ref(IntArray *arr, uindex index)
            : arr_(arr), index_(index) { }
    };

public:
    IntArray();
    IntArray(const IntArray &other);
    IntArray(IntArray &&other);
    IntArray(usize size, u32 width);
    IntArray(std::initializer_list<u64> values);

    ~IntArray();

    IntArray &operator=(const IntArray &other);
    IntArray &operator=(IntArray &&other);

    [[nodiscard]] usize size() const;
    [[nodiscard]] bool empty() const;
    [[nodiscard]] u32 width() const;
    [[nodiscard]] usize size_in_bytes() const;

    [[nodiscard]] u64 get_value(uindex index) const;
    void set_value(uindex index, u64 value);
    [[nodiscard]] u64 operator[](uindex index) const;
    [[nodiscard]] Ref operator[](uindex index);

    void save(const std::string &path) const;
    void save(FILE *file) const;

    void swap(IntArray &other);

    [[nodiscard]] static IntArray load_from_file(const std::string &path);
    [[nodiscard]] static IntArray load_from_file(FILE *file);

private:
    std::unique_ptr<InArray> impl_;
};

}