#pragma once

#include <spdlog/spdlog.h>

#if defined(__GNUC__) // GCC, Clang, ICC
#include <array>
#include <cxxabi.h>
#include <execinfo.h>
#include <string_view>
#include <unistd.h>

#define intrinsic_trap() __builtin_trap()
#define intrinsic_unreachable() __builtin_unreachable()
#define intrinsic_unlikely(x) __builtin_expect(!!(x), 0)
#elif defined(_MSC_VER) // MSVC
#define intrinsic_trap() __debugbreak()
#define intrinsic_unreachable() __assume(false)
#define intrinsic_unlikely(x) (!!(x))
#else
#define intrinsic_trap() std::abort()
#define intrinsic_unreachable() std::abort()
#define intrinsic_unlikely(x) (!!(x))
#endif

namespace impl {

#if defined(__GNUC__)

inline void print_backtrace(const char* b) {
    std::string_view bt = b;
    size_t start = bt.find('(');

    if(start == std::string_view::npos) {
        fmt::println("{}", bt);
        return;
    }

    std::string s;

    size_t end = bt.find('+', ++start);
    if(end == std::string_view::npos || end == start) {
        ++start; // Skip '+'
        end = bt.find(')', end);

        if(end == std::string::npos)
            s.assign(b);
        else
            s.assign(b, start, end - start);
    }
    else {
        s.assign(b, start, end - start);

        size_t len{};
        int status{};
        char* unmangled =
            ::abi::__cxa_demangle(s.c_str(), nullptr, &len, &status);

        if(unmangled) {
            s.assign(unmangled);
            std::free(unmangled);
        }
    }

    fmt::println("{}", s);
}

inline void print_backtrace() {
    fmt::println("---------- BACKTRACE ----------");

    std::array<void*, 10> trace;
    int size = ::backtrace(trace.data(), trace.size());
    char** symbols = ::backtrace_symbols(trace.data(), size);

    for(int i = 0; i < size; i++) {
        fmt::print("#{} ", i);
        impl::print_backtrace(symbols[i]);
    }

    std::free(symbols);
    fmt::println("-------------------------------");
}

#else

inline void print_backtrace() {}

#endif

[[noreturn]] inline void trap() {
    impl::print_backtrace();
    std::fflush(nullptr);
    intrinsic_trap();
}

[[noreturn]] inline void abort() {
    impl::print_backtrace();
    std::fflush(nullptr);
    std::abort();
}

[[noreturn]] inline void unreachable() {
#if defined(NDEBUG) // Release
    impl::print_backtrace();
    std::fflush(nullptr);
    intrinsic_unreachable();
#else
    impl::trap();
#endif
}

} // namespace impl

#define print_backtrace core::impl::print_backtrace();

#define assume(...)                                                            \
    do {                                                                       \
        if(intrinsic_unlikely(!(__VA_ARGS__))) {                               \
            SPDLOG_CRITICAL("Assume condition failed: '{}'", #__VA_ARGS__);    \
            ::impl::trap();                                                    \
        }                                                                      \
    } while(false)

#define unreachable                                                            \
    do {                                                                       \
        SPDLOG_CRITICAL("Unreachable code detected");                          \
        ::impl::unreachable();                                                 \
    } while(false)

#define except(...)                                                            \
    do {                                                                       \
        SPDLOG_CRITICAL(__VA_ARGS__);                                          \
        ::impl::abort();                                                       \
    } while(false)
