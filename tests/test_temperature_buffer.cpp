#include <gtest/gtest.h>
#include "temperature_buffer.h"
#include <vector>
#include <algorithm>

TEST(TemperatureBufferTest, InitialEmptyBuffer) {
    TemperatureBuffer buffer(3);
    EXPECT_EQ(buffer.size(), 0);
}

TEST(TemperatureBufferTest, SinglePushMinMax) {
    TemperatureBuffer buffer(3);
    buffer.push(15.0);
    EXPECT_DOUBLE_EQ(buffer.min(), 15.0);
    EXPECT_DOUBLE_EQ(buffer.max(), 15.0);
}

TEST(TemperatureBufferTest, UpdateMinMaxWithoutOverwrite) {
    TemperatureBuffer buffer(3);
    buffer.push(15.0);
    buffer.push(10.0);
    buffer.push(20.0);
    EXPECT_DOUBLE_EQ(buffer.min(), 10.0);
    EXPECT_DOUBLE_EQ(buffer.max(), 20.0);
}

TEST(TemperatureBufferTest, UpdateMinMaxWithOverwrite) {
    TemperatureBuffer buffer(3);
    // Fill the buffer.
    buffer.push(15.0);
    buffer.push(10.0);
    buffer.push(20.0);
    // The current min is 10.0 and max is 20.0.
    // Now push a new value that will overwrite the oldest element.
    buffer.push(8.0);
    // The overwritten element (15.0) is not min or max, so update new value.
    // Now expected min should be 8.0 (from new push) and max remains 20.0.
    auto data = buffer.data();
    double minInBuffer = *std::min_element(data.begin(), data.end());
    double maxInBuffer = *std::max_element(data.begin(), data.end());
    EXPECT_DOUBLE_EQ(buffer.min(), minInBuffer);
    EXPECT_DOUBLE_EQ(buffer.max(), maxInBuffer);
}

TEST(TemperatureBufferTest, MultiplePushes) {
    TemperatureBuffer buffer(5);
    std::vector<double> values = {15.0, 14.0, 16.0, 13.0, 18.0, 17.0, 12.0};
    for (double v : values) {
         buffer.push(v);
    }
    auto data = buffer.data();
    double actualMin = *std::min_element(data.begin(), data.end());
    double actualMax = *std::max_element(data.begin(), data.end());
    EXPECT_DOUBLE_EQ(buffer.min(), actualMin);
    EXPECT_DOUBLE_EQ(buffer.max(), actualMax);
}

TEST(TemperatureBufferTest, OverwriteRescanOnMinMaxEqualCandidate)
{
    // Create a buffer with capacity 3 and fill with distinct values.
    TemperatureBuffer buffer(3);
    buffer.push(10.0);  // min = 10, max = 10
    buffer.push(20.0);  // min = 10, max = 20
    buffer.push(30.0);  // min = 10, max = 30

    // The next push will overwrite the oldest (10.0), which is the current min.
    // Let's push a value equal to the old min.
    buffer.push(10.0);
    // Now, a rescan should update the min.
    auto data = buffer.data();
    double expectedMin = *std::min_element(data.begin(), data.end());
    double expectedMax = *std::max_element(data.begin(), data.end());
    EXPECT_DOUBLE_EQ(buffer.min(), expectedMin);
    EXPECT_DOUBLE_EQ(buffer.max(), expectedMax);
}

TEST(TemperatureBufferTest, AllValuesEqual)
{
    TemperatureBuffer buffer(4);
    // Push the same value repeatedly.
    buffer.push(25.0);
    buffer.push(25.0);
    buffer.push(25.0);
    buffer.push(25.0);
    EXPECT_DOUBLE_EQ(buffer.min(), 25.0);
    EXPECT_DOUBLE_EQ(buffer.max(), 25.0);
}

TEST(TemperatureBufferTest, NegativeAndMixedValues)
{
    TemperatureBuffer buffer(5);
    // Use a mix of negative and positive values.
    buffer.push(-5.0);
    buffer.push(0.0);
    buffer.push(3.0);
    buffer.push(-10.0);
    buffer.push(7.0);
    auto data = buffer.data();
    double expectedMin = *std::min_element(data.begin(), data.end()); // should be -10.0
    double expectedMax = *std::max_element(data.begin(), data.end()); // should be 7.0
    EXPECT_DOUBLE_EQ(buffer.min(), expectedMin);
    EXPECT_DOUBLE_EQ(buffer.max(), expectedMax);
}

TEST(TemperatureBufferTest, PopFunctionality) {
    TemperatureBuffer tBuffer(4);
    tBuffer.push(20.0);
    tBuffer.push(25.0);
    tBuffer.push(22.0);
    EXPECT_EQ(tBuffer.size(), 3);

    double value = 0.0;
    EXPECT_TRUE(tBuffer.pop(value));
    EXPECT_DOUBLE_EQ(value, 20.0);
    EXPECT_EQ(tBuffer.size(), 2);

    // Remaining buffer should be {25,22}.
    std::vector<double> expected = {25.0, 22.0};
    EXPECT_EQ(tBuffer.data(), expected);
}

TEST(TemperatureBufferTest, PopEmptyBuffer) {
    TemperatureBuffer tBuffer(3);
    double value = 0.0;
    EXPECT_FALSE(tBuffer.pop(value));
    EXPECT_EQ(tBuffer.size(), 0);
}
