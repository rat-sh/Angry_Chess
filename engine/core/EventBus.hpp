#pragma once
#include <cstdint>
#include <functional>
#include <unordered_map>
#include <mutex>
#include <atomic>

namespace bge {

/// Lock-free single-threaded / mutex-protected multi-threaded pub-sub bus.
/// Each event type T gets its own EventBus<T> instance.
///
/// Usage:
///   EventBus<MoveMadeEvent> bus;
///   auto id = bus.subscribe([](const MoveMadeEvent& e){ ... });
///   bus.publish(MoveMadeEvent{...});
///   bus.unsubscribe(id);

using SubscriberID = uint64_t;

template<typename Event>
class EventBus {
public:
    using Handler = std::function<void(const Event&)>;

    /// Register a handler; returns a unique ID for later unsubscription.
    [[nodiscard]] SubscriberID subscribe(Handler handler) {
        std::lock_guard lock(mutex_);
        SubscriberID id = nextId_++;
        subscribers_[id] = std::move(handler);
        return id;
    }

    /// Remove a handler by the ID returned from subscribe().
    void unsubscribe(SubscriberID id) {
        std::lock_guard lock(mutex_);
        subscribers_.erase(id);
    }

    /// Deliver the event to all current subscribers synchronously.
    void publish(const Event& event) {
        // Snapshot handlers to avoid holding the lock during callbacks,
        // which prevents deadlocks if a handler subscribes/unsubscribes.
        std::vector<Handler> snapshot;
        {
            std::lock_guard lock(mutex_);
            snapshot.reserve(subscribers_.size());
            for (auto& [id, h] : subscribers_) snapshot.push_back(h);
        }
        for (auto& h : snapshot) h(event);
    }

    /// Number of current subscribers.
    [[nodiscard]] std::size_t subscriberCount() const {
        std::lock_guard lock(mutex_);
        return subscribers_.size();
    }

private:
    mutable std::mutex mutex_;
    std::unordered_map<SubscriberID, Handler> subscribers_;
    SubscriberID nextId_{1};
};

// ─── Built-in engine events ──────────────────────────────────────────────────

struct MoveMadeEvent {
    struct Position { int8_t rank, file; };
    Position    from;
    Position    to;
    uint8_t     pieceType;
    int8_t      playerColor;  // +1 = White, -1 = Black
};

struct PieceCapturedEvent {
    struct Position { int8_t rank, file; };
    Position    at;
    uint8_t     capturedType;
    int8_t      capturedColor;
};

struct CheckEvent {
    int8_t playerInCheck; // +1 White, -1 Black
};

struct GameOverEvent {
    int8_t winnerColor; // +1 White, -1 Black, 0 = draw
    uint8_t status;     // maps to GameStatus enum
};

struct TurnChangedEvent {
    int8_t activePlayer; // +1 White, -1 Black
};

struct ThemeChangedEvent {
    std::string newThemeName;
};

} // namespace bge
