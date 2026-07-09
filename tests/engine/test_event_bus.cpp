#include <catch2/catch_test_macros.hpp>
#include "engine/core/EventBus.hpp"
#include <atomic>

using namespace bge;

TEST_CASE("EventBus: subscribe and publish", "[eventbus]") {
    EventBus<MoveMadeEvent> bus;
    int callCount = 0;
    bus.subscribe([&](const MoveMadeEvent&) { ++callCount; });

    bus.publish(MoveMadeEvent{{0,0},{1,1},1,1});
    bus.publish(MoveMadeEvent{{0,0},{1,1},1,1});
    CHECK(callCount == 2);
}

TEST_CASE("EventBus: multiple subscribers", "[eventbus]") {
    EventBus<CheckEvent> bus;
    int a = 0, b = 0;
    [[maybe_unused]] auto id1 = bus.subscribe([&](const CheckEvent&) { ++a; });
    [[maybe_unused]] auto id2 = bus.subscribe([&](const CheckEvent&) { ++b; });

    bus.publish(CheckEvent{1});
    CHECK(a == 1);
    CHECK(b == 1);
}

TEST_CASE("EventBus: unsubscribe", "[eventbus]") {
    EventBus<GameOverEvent> bus;
    int calls = 0;
    auto id = bus.subscribe([&](const GameOverEvent&) { ++calls; });

    bus.publish(GameOverEvent{1, 2});
    CHECK(calls == 1);

    bus.unsubscribe(id);
    bus.publish(GameOverEvent{1, 2});
    CHECK(calls == 1); // no additional calls after unsubscribe
}

TEST_CASE("EventBus: subscriberCount", "[eventbus]") {
    EventBus<TurnChangedEvent> bus;
    CHECK(bus.subscriberCount() == 0);

    auto id1 = bus.subscribe([](const TurnChangedEvent&) {});
    auto id2 = bus.subscribe([](const TurnChangedEvent&) {});
    CHECK(bus.subscriberCount() == 2);

    bus.unsubscribe(id1);
    CHECK(bus.subscriberCount() == 1);

    bus.unsubscribe(id2);
    CHECK(bus.subscriberCount() == 0);
}

TEST_CASE("EventBus: unsubscribing non-existent ID is a no-op", "[eventbus]") {
    EventBus<CheckEvent> bus;
    CHECK_NOTHROW(bus.unsubscribe(999));
}
