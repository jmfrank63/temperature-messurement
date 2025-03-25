#pragma once
#include <vector>
#include <atomic>

template <typename T>
class RingBuffer {
public:
    // capacity_: the fixed size of the circular buffer.
    explicit RingBuffer(size_t capacity)
        : capacity_(capacity), head_(0), tail_(0), count_(0)
    {
        buffer_.resize(capacity);
    }

    // Producer: push a new value into the buffer.
    // If the buffer is full, the oldest value is overwritten.
    void push(const T& value) {
        size_t head = head_.load(std::memory_order_relaxed);
        buffer_[head] = value;
        size_t next = (head + 1) % capacity_;
        head_.store(next, std::memory_order_release);

        // If not full, increment count; otherwise advance tail.
        size_t currentCount = count_.load(std::memory_order_relaxed);
        if (currentCount < capacity_) {
            count_.fetch_add(1, std::memory_order_release);
        } else {
            size_t currentTail = tail_.load(std::memory_order_relaxed);
            tail_.store((currentTail + 1) % capacity_, std::memory_order_release);
        }
    }

    // Consumer: attempts to pop the oldest value.
    // Returns true if a value was popped.
    bool pop(T &value) {
        size_t currentCount = count_.load(std::memory_order_acquire);
        if (currentCount == 0)
            return false; // Buffer empty.
        size_t tail = tail_.load(std::memory_order_relaxed);
        value = buffer_[tail];
        tail_.store((tail + 1) % capacity_, std::memory_order_release);
        count_.fetch_sub(1, std::memory_order_release);
        return true;
    }

    // Returns a snapshot of the current buffer contents in order (oldest first).
    std::vector<T> snapshot() const {
        std::vector<T> result;
        size_t cnt = size();
        result.reserve(cnt);
        size_t tail = tail_.load(std::memory_order_acquire);
        for (size_t i = 0; i < cnt; ++i) {
            result.push_back(buffer_[(tail + i) % capacity_]);
        }
        return result;
    }

    // For backwards compatibility with tests.
    std::vector<T> data() const { return snapshot(); }

    // Returns the current number of elements in the buffer.
    size_t size() const {
        return count_.load(std::memory_order_acquire);
    }

    // Returns the element that will be overwritten next (oldest element).
    const T& getOverwriteCandidate() const {
        size_t tail = tail_.load(std::memory_order_acquire);
        return buffer_[tail];
    }

private:
    std::vector<T> buffer_;
    const size_t capacity_;
    std::atomic<size_t> head_;   // Next write position.
    std::atomic<size_t> tail_;   // Oldest element (next to be popped).
    std::atomic<size_t> count_;  // Current number of elements.
};
