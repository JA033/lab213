#pragma once

#include <chrono>

#include "typedefs.hpp"

namespace wildcard {

using FloatNanos  = std::chrono::duration<f64, std::nano>;
using FloatMicros = std::chrono::duration<f64, std::micro>;
using FloatMillis = std::chrono::duration<f64, std::milli>;
using FloatSecs   = std::chrono::duration<f64, std::ratio<1, 1>>;

enum class TimeUnit {
    kNanoseconds,
    kMicroseconds,
    kMilliseconds,
    kSeconds
};

inline f64 get_time(const std::chrono::high_resolution_clock::time_point &start, 
                    const std::chrono::high_resolution_clock::time_point &end, 
                    TimeUnit unit) {
    f64 total = 0.0F;
    using enum TimeUnit;
    switch (unit) {
    case kNanoseconds  : total = FloatNanos(end - start).count(); break;
    case kMicroseconds : total = FloatMicros(end - start).count(); break;
    case kMilliseconds : total = FloatMillis(end - start).count(); break;
    case kSeconds      : total = FloatSecs(end - start).count(); break;
    }
    return total;
}

inline auto start_clock() {
    return std::chrono::high_resolution_clock::now();
}

inline auto end_clock() {
    return std::chrono::high_resolution_clock::now();
}

template<typename F, typename ...ArgTs>
inline f64 measure_time(TimeUnit unit, F &&f, ArgTs &&...args) {
    auto start = start_clock();
    f(std::forward<ArgTs>(args)...);
    auto end = end_clock();
    return get_time(start, end, unit);
}

}