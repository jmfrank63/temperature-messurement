#include <gtest/gtest.h>
#include "temperature_buffer_deque.h"
#include <algorithm>
#include <vector>

TEST(TemperatureBufferDequeTest, SinglePushMinMax) {
    TemperatureBufferDeque buffer(3);
    buffer.push(15.0);
    EXPECT_DOUBLE_EQ(buffer.min(), 15.0);
    EXPECT_DOUBLE_EQ(buffer.max(), 15.0);
}

TEST(TemperatureBufferDequeTest, UpdateMinMaxWithOverwrites) {
    TemperatureBufferDeque buffer(3);
    // Fill the buffer.
    buffer.push(15.0);
    buffer.push(10.0);
    buffer.push(20.0);
    EXPECT_DOUBLE_EQ(buffer.min(), 10.0);
    EXPECT_DOUBLE_EQ(buffer.max(), 20.0);
    // This push will overwrite the oldest element (15.0)
    buffer.push(8.0);
    // Now expected data: {10.0, 20.0, 8.0} with min=8.0 and max=20.0.
    std::vector<double> data = buffer.data();
    double expectedMin = *std::min_element(data.begin(), data.end());
    double expectedMax = *std::max_element(data.begin(), data.end());
    EXPECT_DOUBLE_EQ(buffer.min(), expectedMin);
    EXPECT_DOUBLE_EQ(buffer.max(), expectedMax);
}

TEST(TemperatureBufferDequeTest, MultipleWraps) {
    const int capacity = 4;
    TemperatureBufferDeque buffer(capacity);
    for (int i = 1; i <= 10; ++i) {
        buffer.push(i * 1.0);
    }
    EXPECT_EQ(buffer.size(), capacity);
    std::vector<double> data = buffer.data();
    // For 10 pushes in buffer of capacity 4, expected elements are {7.0, 8.0, 9.0, 10.0}
    std::vector<double> expected = {7.0, 8.0, 9.0, 10.0};
    ASSERT_EQ(data.size(), expected.size());
    for (size_t i = 0; i < data.size(); ++i) {
        EXPECT_DOUBLE_EQ(data[i], expected[i]);
    }
}
