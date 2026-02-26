#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

#include <utility/timestamp/Timestamp.h>

#include <thread>
#include <chrono>

TEST_CASE("Timestamp default constructor captures current time", "[timestamp]")
{
    const auto before = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();

    sLink::utility::Timestamp ts;

    const auto after = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();

    CHECK(ts.getMs() >= before);
    CHECK(ts.getMs() <= after);
}

TEST_CASE("Timestamp constructed from fixed value stores it correctly", "[timestamp]")
{
    constexpr int64_t fixed = 1'700'000'000'000LL;

    sLink::utility::Timestamp ts(fixed);

    CHECK(ts.getMs() == fixed);
}

TEST_CASE("Timestamp with value 0 does not crash", "[timestamp]")
{
    sLink::utility::Timestamp ts(0);
    CHECK(ts.getMs() == 0);
    REQUIRE_FALSE(ts.toString().empty());
}

TEST_CASE("Timestamp::toString returns non-empty string", "[timestamp]")
{
    sLink::utility::Timestamp ts;
    REQUIRE_FALSE(ts.toString().empty());
}

TEST_CASE("Timestamp::toString contains date separators", "[timestamp]")
{
    sLink::utility::Timestamp ts(1'700'000'000'000LL);
    const auto str = ts.toString();

    CHECK_THAT(str, Catch::Matchers::ContainsSubstring("/"));
    CHECK_THAT(str, Catch::Matchers::ContainsSubstring(":"));
}

TEST_CASE("Two default Timestamps are monotonically non-decreasing", "[timestamp]")
{
    sLink::utility::Timestamp t1;
    std::this_thread::sleep_for(std::chrono::milliseconds(2));
    sLink::utility::Timestamp t2;

    CHECK(t2.getMs() >= t1.getMs());
}