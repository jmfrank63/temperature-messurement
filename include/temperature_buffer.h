#pragma once
#include "ring_buffer.h"
#include <limits>
#include <vector>
#include <algorithm>

class TemperatureBuffer {
public:
    explicit TemperatureBuffer(size_t capacity)
        : buffer_(capacity),
          capacity_(capacity),
          currentMin_(std::numeric_limits<double>::max()),
          currentMax_(std::numeric_limits<double>::lowest())
    {}

    // On push, update min and max. If buffer is full and the value being overwritten
    // equals our current min or max, we recalc over the whole buffer.
    void push(double value) {
        bool isFull = (buffer_.size() == capacity_);
        double overwritten = 0.0;
        if (isFull) {
            overwritten = buffer_.getOverwriteCandidate();
        }
        buffer_.push(value);

        if (!isFull) {
            // Buffer is not yet full: update min and max with the new value.
            if (value < currentMin_) currentMin_ = value;
            if (value > currentMax_) currentMax_ = value;
        } else {
            // If the overwritten value equals current min or max, rescan.
            if (overwritten == currentMin_ || overwritten == currentMax_) {
                recalcMinMax();
            }
            // Also update for the new value.
            if (value < currentMin_) currentMin_ = value;
            if (value > currentMax_) currentMax_ = value;
        }
    }

    double min() const { return currentMin_; }
    double max() const { return currentMax_; }
    std::vector<double> data() const { return buffer_.data(); }
    size_t size() const { return buffer_.size(); }

private:
    RingBuffer<double> buffer_;
    size_t capacity_;
    double currentMin_;
    double currentMax_;

    // Scan the entire buffer to recalc min and max.
    void recalcMinMax() {
        std::vector<double> d = buffer_.data();
        if (d.empty()) {
            currentMin_ = std::numeric_limits<double>::max();
            currentMax_ = std::numeric_limits<double>::lowest();
            return;
        }
        currentMin_ = *std::min_element(d.begin(), d.end());
        currentMax_ = *std::max_element(d.begin(), d.end());
    }
};
