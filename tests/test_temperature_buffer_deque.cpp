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

TEST(TemperatureBufferDequeTest, PushPopOrdering) {
    TemperatureBufferDeque buffer(4);

    // Initially empty.
    EXPECT_EQ(buffer.size(), 0);

    // Push values.
    buffer.push(10.0);
    buffer.push(20.0);
    buffer.push(30.0);
    buffer.push(40.0);
    EXPECT_EQ(buffer.size(), 4);

    // Expected ordering: {10, 20, 30, 40}
    std::vector<double> expected = {10.0, 20.0, 30.0, 40.0};
    EXPECT_EQ(buffer.data(), expected);

    // Pop one value; should be 10.0.
    double value = 0.0;
    EXPECT_TRUE(buffer.pop(value));
    EXPECT_DOUBLE_EQ(value, 10.0);
    EXPECT_EQ(buffer.size(), 3);

    // Expected ordering now: {20, 30, 40}
    expected = {20.0, 30.0, 40.0};
    EXPECT_EQ(buffer.data(), expected);
}

TEST(TemperatureBufferDequeTest, WrapAround) {
    TemperatureBufferDeque buffer(3);

    buffer.push(1.0);
    buffer.push(2.0);
    buffer.push(3.0);
    EXPECT_EQ(buffer.size(), 3);

    // Next push triggers overwrite.
    buffer.push(4.0);  // Overwrites the oldest element (1.0)
    EXPECT_EQ(buffer.size(), 3);
    // Expected ordering: {2.0, 3.0, 4.0}
    std::vector<double> expected = {2.0, 3.0, 4.0};
    EXPECT_EQ(buffer.data(), expected);
}

TEST(TemperatureBufferDequeTest, MinMaxAndPopEmpty) {
    TemperatureBufferDeque buffer(5);

    buffer.push(5.0);
    buffer.push(3.0);
    buffer.push(7.0);
    buffer.push(2.0);
    buffer.push(9.0);
    // Buffer full: {5,3,7,2,9}
    EXPECT_EQ(buffer.size(), 5);
    EXPECT_DOUBLE_EQ(buffer.min(), 2.0);
    EXPECT_DOUBLE_EQ(buffer.max(), 9.0);

    // Pop one value; expect 5.0.
    double value = 0.0;
    EXPECT_TRUE(buffer.pop(value));
    EXPECT_DOUBLE_EQ(value, 5.0);
    EXPECT_EQ(buffer.size(), 4);
    // Min and max remain (3,2,7,9) ==> min = 2.0, max = 9.0.
    EXPECT_DOUBLE_EQ(buffer.min(), 2.0);
    EXPECT_DOUBLE_EQ(buffer.max(), 9.0);

    // Empty the buffer.
    while(buffer.pop(value)) { }
    EXPECT_EQ(buffer.size(), 0);
    // Popping now should fail.
    EXPECT_FALSE(buffer.pop(value));
}
