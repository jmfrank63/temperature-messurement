#pragma once
#include <vector>
#include <deque>
#include <limits>
#include <atomic>

// A very simple spin lock for SPSC scenarios.
class SpinLock {
public:
    SpinLock() : flag(ATOMIC_FLAG_INIT) {}

    void lock() {
        while (flag.test_and_set(std::memory_order_acquire)) { }
    }

    void unlock() {
        flag.clear(std::memory_order_release);
    }

private:
    std::atomic_flag flag;
};

class TemperatureBufferDeque
{
public:
    explicit TemperatureBufferDeque(size_t capacity)
        : capacity_(capacity),
          buffer_(capacity),
          index_(0),
          size_(0)
    {
    }

    // Push a new temperature measurement.
    // If the buffer is full, the oldest value is overwritten.
    void push(double value)
    {
        size_t currentSize = size_.load(std::memory_order_relaxed);
        if (currentSize == capacity_) {
            double overwritten = buffer_[ index_.load(std::memory_order_relaxed) ];
            // Since only the producer calls push(), these deque operations are safe,
            // but we still protect against concurrent access with the spin lock.
            spinLock.lock();
            if (!maxDeque_.empty() && maxDeque_.front() == overwritten)
                maxDeque_.pop_front();
            if (!minDeque_.empty() && minDeque_.front() == overwritten)
                minDeque_.pop_front();
            spinLock.unlock();
        }
        else {
            size_.fetch_add(1, std::memory_order_relaxed);
        }
        buffer_[ index_.load(std::memory_order_relaxed) ] = value;
        index_.store((index_.load(std::memory_order_relaxed) + 1) % capacity_, std::memory_order_release);

        spinLock.lock();
        // Update max deque.
        while (!maxDeque_.empty() && maxDeque_.back() < value)
            maxDeque_.pop_back();
        maxDeque_.push_back(value);

        // Update min deque.
        while (!minDeque_.empty() && minDeque_.back() > value)
            minDeque_.pop_back();
        minDeque_.push_back(value);
        spinLock.unlock();
    }

    // Pop the oldest value from the buffer.
    // Returns true if a value was popped; false if the buffer is empty.
    bool pop(double &value)
    {
        size_t currentSize = size_.load(std::memory_order_acquire);
        if (currentSize == 0)
            return false;
        size_t tail = (index_.load(std::memory_order_acquire) + capacity_ - currentSize) % capacity_;
        value = buffer_[tail];

        spinLock.lock();
        if (!minDeque_.empty() && minDeque_.front() == value)
            minDeque_.pop_front();
        if (!maxDeque_.empty() && maxDeque_.front() == value)
            maxDeque_.pop_front();
        spinLock.unlock();

        size_.fetch_sub(1, std::memory_order_release);
        return true;
    }

    // Returns the current minimum.
    double min() const
    {
        // No lock here since this is a read and if a concurrent update occurs, we accept the transient state.
        return minDeque_.empty() ? std::numeric_limits<double>::max() : minDeque_.front();
    }

    // Returns the current maximum.
    double max() const
    {
        return maxDeque_.empty() ? std::numeric_limits<double>::lowest() : maxDeque_.front();
    }

    // Returns the current stored data in FIFO order.
    std::vector<double> data() const
    {
        std::vector<double> result;
        size_t cnt = size_.load(std::memory_order_acquire);
        result.reserve(cnt);
        size_t tail = (index_.load(std::memory_order_acquire) + capacity_ - cnt) % capacity_;
        for (size_t i = 0; i < cnt; ++i)
            result.push_back(buffer_[(tail + i) % capacity_]);
        return result;
    }

    size_t size() const { return size_.load(std::memory_order_acquire); }
    size_t capacity() const { return capacity_; }

private:
    size_t capacity_;
    std::vector<double> buffer_;
    std::atomic<size_t> index_;
    std::atomic<size_t> size_;
    std::deque<double> minDeque_;
    std::deque<double> maxDeque_;
    mutable SpinLock spinLock; // Protects deque operations.
};
