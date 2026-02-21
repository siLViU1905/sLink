#ifndef SLINK_BENCHMARK_H
#define SLINK_BENCHMARK_H

#include <chrono>
#include <print>

#ifndef NDEBUG
    #define SLINK_CL_CLR_RESET       "\033[0m"
    #define SLINK_CL_CLR_GREEN       "\033[1;32m"
    #define SLINK_CL_CLR_YELLOW      "\033[1;33m"
    #define SLINK_CL_CLR_CYAN        "\033[1;36m"
    #define SLINK_CL_CLR_MAGENTA     "\033[1;35m"
    #define SLINK_CL_CLR_RED         "\033[1;31m"

    #define SLINK_START_BENCHMARK auto startTime = std::chrono::steady_clock::now();

    #define SLINK_END_BENCHMARK(prefix, label, color) \
    { \
    auto endTime = std::chrono::steady_clock::now(); \
    std::chrono::duration<float, std::milli> duration = endTime - startTime; \
    std::println("{}{} {}: {:.3f} ms{}", \
    color, prefix, label, duration.count(), SLINK_CL_CLR_RESET); \
    }
#else
    #define SLINK_CL_CLR_RESET       ""
    #define SLINK_CL_CLR_GREEN       ""
    #define SLINK_CL_CLR_YELLOW      ""
    #define SLINK_CL_CLR_CYAN        ""
    #define SLINK_CL_CLR_MAGENTA     ""
    #define SLINK_CL_CLR_RED         ""

#define SLINK_START_BENCHMARK

#define SLINK_END_BENCHMARK(prefix, label, color)

#endif

#endif
