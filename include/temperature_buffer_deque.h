#pragma once
#include <vector>
#include <deque>
#include <limits>

class TemperatureBufferDeque
{
public:
    explicit TemperatureBufferDeque(size_t capacity)
        : capacity_(capacity), buffer_(capacity), index_(0), size_(0)
    {
    }

    // Push a new temperature measurement.
    // If the buffer is full, the oldest value is overwritten.
    void push(double value)
    {
        // If the buffer is full, remove the overwritten element from deques if needed.
        if (size_ == capacity_)
        {
            double overwritten = buffer_[index_];
            if (!maxDeque_.empty() && maxDeque_.front() == overwritten)
                maxDeque_.pop_front();
            if (!minDeque_.empty() && minDeque_.front() == overwritten)
                minDeque_.pop_front();
        }
        else
        {
            ++size_;
        }

        // Write new value into ring buffer.
        buffer_[index_] = value;
        index_ = (index_ + 1) % capacity_;

        // Update max deque.
        while (!maxDeque_.empty() && maxDeque_.back() < value)
            maxDeque_.pop_back();
        maxDeque_.push_back(value);

        // Update min deque.
        while (!minDeque_.empty() && minDeque_.back() > value)
            minDeque_.pop_back();
        minDeque_.push_back(value);
    }

    // Pop the oldest value from the buffer.
    // Returns true if a value was popped; false if the buffer is empty.
    bool pop(double &value)
    {
        if (size_ == 0)
            return false;
        // Compute tail index: the oldest element is at:
        size_t tail = (index_ + capacity_ - size_) % capacity_;
        value = buffer_[tail];

        // Update min deque if needed.
        if (!minDeque_.empty() && minDeque_.front() == value)
            minDeque_.pop_front();
        // Update max deque if needed.
        if (!maxDeque_.empty() && maxDeque_.front() == value)
            maxDeque_.pop_front();

        --size_;
        return true;
    }

    // Returns the current minimum.
    double min() const
    {
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
           result.reserve(size_);
           // Compute tail index the same way as in pop().
           size_t tail = (index_ + capacity_ - size_) % capacity_;
           for (size_t i = 0; i < size_; ++i)
               result.push_back(buffer_[(tail + i) % capacity_]);
           return result;
       }

    size_t size() const { return size_; }
    size_t capacity() const { return capacity_; }

private:
    size_t capacity_;
    std::vector<double> buffer_;
    size_t index_;
    size_t size_;
    std::deque<double> minDeque_;
    std::deque<double> maxDeque_;
};
