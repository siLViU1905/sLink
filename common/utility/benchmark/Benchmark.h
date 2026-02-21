#ifndef SLINK_BENCHMARK_H
#define SLINK_BENCHMARK_H

#include <chrono>
#include <print>

#ifndef NDEBUG
#define SLINK_START_BENCHMARK auto startTime = std::chrono::steady_clock::now();

#define SLINK_END_BENCHMARK(prefix, label) \
{ \
auto endTime = std::chrono::steady_clock::now(); \
std::chrono::duration<float, std::milli> duration = endTime - startTime; \
std::println("{} {}: {:.3f} ms", prefix, label, duration.count()); \
}
#else

#define SLINK_START_BENCHMARK
#define SLINK_END_BENCHMARK(prefix, label)

#endif

#endif
