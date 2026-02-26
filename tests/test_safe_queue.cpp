#include <catch2/catch_test_macros.hpp>

#include <utility/safe_queue/SafeQueue.h>

#include <thread>
#include <vector>
#include <atomic>

TEST_CASE("SafeQueue tryPop on empty queue returns nullopt", "[safe_queue]")
{
    sLink::utility::SafeQueue<int> q;
    auto result = q.tryPop();
    REQUIRE_FALSE(result.has_value());
}

TEST_CASE("SafeQueue push lvalue then tryPop returns value", "[safe_queue]")
{
    sLink::utility::SafeQueue<int> q;

    int val = 42;
    q.push(val);

    auto result = q.tryPop();
    REQUIRE(result.has_value());
    CHECK(*result == 42);
}

TEST_CASE("SafeQueue push rvalue then tryPop returns value", "[safe_queue]")
{
    sLink::utility::SafeQueue<std::string> q;
    q.push(std::string("hello"));

    auto result = q.tryPop();
    REQUIRE(result.has_value());
    CHECK(*result == "hello");
}

TEST_CASE("SafeQueue is FIFO", "[safe_queue]")
{
    sLink::utility::SafeQueue<int> q;

    q.push(1);
    q.push(2);
    q.push(3);

    CHECK(*q.tryPop() == 1);
    CHECK(*q.tryPop() == 2);
    CHECK(*q.tryPop() == 3);
    CHECK_FALSE(q.tryPop().has_value());
}

TEST_CASE("SafeQueue empties correctly after multiple pops", "[safe_queue]")
{
    sLink::utility::SafeQueue<int> q;

    for (int i = 0; i < 5; ++i)
        q.push(i);

    for (int i = 0; i < 5; ++i)
        REQUIRE(q.tryPop().has_value());

    CHECK_FALSE(q.tryPop().has_value());
}


TEST_CASE("SafeQueue handles concurrent push from multiple threads", "[safe_queue][threading]")
{
    sLink::utility::SafeQueue<int> q;

    constexpr int producers  = 4;
    constexpr int items_each = 250;
    constexpr int total      = producers * items_each;

    std::vector<std::thread> threads;
    threads.reserve(producers);

    for (int t = 0; t < producers; ++t)
        threads.emplace_back([&q, t]()
        {
            for (int i = 0; i < items_each; ++i)
                q.push(t * items_each + i);
        });

    for (auto &th : threads)
        th.join();

    int count = 0;
    while (q.tryPop().has_value())
        ++count;

    CHECK(count == total);
}

TEST_CASE("SafeQueue handles concurrent push and pop", "[safe_queue][threading]")
{
    sLink::utility::SafeQueue<int> q;

    constexpr int items = 1000;

    std::atomic<int> consumed{0};

    std::thread producer([&]()
    {
        for (int i = 0; i < items; ++i)
            q.push(i);
    });

    std::thread consumer([&]()
    {
        int received = 0;
        while (received < items)
        {
            if (q.tryPop().has_value())
                ++received;
        }
        consumed.store(received);
    });

    producer.join();
    consumer.join();

    CHECK(consumed.load() == items);
}

TEST_CASE("SafeQueue works with move-only types", "[safe_queue]")
{
    sLink::utility::SafeQueue<std::unique_ptr<int>> q;

    q.push(std::make_unique<int>(99));

    auto result = q.tryPop();
    REQUIRE(result.has_value());
    CHECK(**result == 99);
}