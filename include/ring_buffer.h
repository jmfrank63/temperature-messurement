#pragma once
#include <vector>

template <typename T>
class RingBuffer {
public:
    explicit RingBuffer(size_t capacity)
        : capacity_(capacity), index_(0), size_(0)
    {
        buffer_.resize(capacity);
    }

    void push(const T& value) {
        buffer_[index_] = value;
        index_ = (index_ + 1) % capacity_;
        if (size_ < capacity_) {
            ++size_;
        }
    }

    std::vector<T> data() const {
        std::vector<T> result;
        result.reserve(size_);
        size_t start = (size_ == capacity_) ? index_ : 0;
        for (size_t i = 0; i < size_; ++i) {
            result.push_back(buffer_[(start + i) % capacity_]);
        }
        return result;
    }

    size_t size() const { return size_; }
    size_t capacity() const { return capacity_; }

    // New: Return the element that will be overwritten next.
    const T& getOverwriteCandidate() const { return buffer_[index_]; }

private:
    std::vector<T> buffer_;
    size_t capacity_;
    size_t index_;
    size_t size_;
};
