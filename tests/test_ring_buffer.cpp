#include <gtest/gtest.h>
#include <vector>
#include "ring_buffer.h"

TEST(RingBufferTest, BasicPushAndData)
{
    // Create a ring buffer with capacity 3.
    RingBuffer<int> buffer(3);
    EXPECT_EQ(buffer.size(), 0);

    // Push values and check size.
    buffer.push(1);
    buffer.push(2);
    buffer.push(3);
    EXPECT_EQ(buffer.size(), 3);

    // The buffer should return values in oldest-to-newest order.
    std::vector<int> data = buffer.data();
    std::vector<int> expected = {1, 2, 3};
    EXPECT_EQ(data, expected);
}

TEST(RingBufferTest, OverwriteOldest)
{
    // Create a ring buffer with capacity 3.
    RingBuffer<int> buffer(3);

    // Fill the buffer.
    buffer.push(1);
    buffer.push(2);
    buffer.push(3);
    // This push should overwrite the oldest element (1).
    buffer.push(4);

    EXPECT_EQ(buffer.size(), 3);

    // Now the expected order is {2, 3, 4}.
    std::vector<int> data = buffer.data();
    std::vector<int> expected = {2, 3, 4};
    EXPECT_EQ(data, expected);
}

TEST(RingBufferTest, PartialFill)
{
    // Create a ring buffer with capacity 5.
    RingBuffer<int> buffer(5);

    // Push fewer elements than the capacity.
    buffer.push(10);
    buffer.push(20);

    EXPECT_EQ(buffer.size(), 2);

    std::vector<int> data = buffer.data();
    std::vector<int> expected = {10, 20};
    EXPECT_EQ(data, expected);
}

// Edge case - Empty buffer should return an empty vector.
TEST(RingBufferTest, EmptyBuffer)
{
    RingBuffer<int> buffer(4);
    EXPECT_EQ(buffer.size(), 0);
    std::vector<int> data = buffer.data();
    std::vector<int> expected = {};
    EXPECT_EQ(data, expected);
}

// Edge case - Multiple wraps: Push more than twice the capacity and verify the order.
TEST(RingBufferTest, MultipleWraps)
{
    const int capacity = 4;
    RingBuffer<int> buffer(capacity);

    // Push 10 elements, so there are multiple wraps.
    for (int i = 1; i <= 10; ++i)
    {
        buffer.push(i);
    }
    EXPECT_EQ(buffer.size(), capacity);

    // The underlying buffer should contain the last 'capacity' values:
    // for capacity 4 and 10 total pushes, expected order is {7, 8, 9, 10}
    std::vector<int> data = buffer.data();
    std::vector<int> expected = {7, 8, 9, 10};
    EXPECT_EQ(data, expected);
}

// Additional edge case - Single element buffer.
TEST(RingBufferTest, SingleElementBuffer)
{
    RingBuffer<int> buffer(1);
    EXPECT_EQ(buffer.size(), 0);

    // Only one element can be stored at a time.
    buffer.push(1);
    EXPECT_EQ(buffer.size(), 1);
    std::vector<int> data1 = buffer.data();
    std::vector<int> expected1 = {1};
    EXPECT_EQ(data1, expected1);

    buffer.push(2);
    EXPECT_EQ(buffer.size(), 1);
    std::vector<int> data2 = buffer.data();
    std::vector<int> expected2 = {2};
    EXPECT_EQ(data2, expected2);
}

// Additional edge case - Repeated values.
TEST(RingBufferTest, RepeatedValues)
{
    RingBuffer<int> buffer(3);
    buffer.push(5);
    buffer.push(5);
    buffer.push(5);
    EXPECT_EQ(buffer.size(), 3);
    std::vector<int> data = buffer.data();
    std::vector<int> expected = {5, 5, 5};
    EXPECT_EQ(data, expected);
}

TEST(RingBufferTest, DoubleValues)
{
    // Create a ring buffer with capacity 4 for doubles.
    RingBuffer<double> buffer(4);

    // Push some double values.
    buffer.push(1.1);
    buffer.push(2.2);
    buffer.push(3.3);

    // Check size and order.
    EXPECT_EQ(buffer.size(), 3);
    std::vector<double> data = buffer.data();
    std::vector<double> expected = {1.1, 2.2, 3.3};
    ASSERT_EQ(data.size(), expected.size());
    for (size_t i = 0; i < data.size(); ++i)
    {
        EXPECT_DOUBLE_EQ(data[i], expected[i]);
    }

    // Now test overwriting: Push additional values to fill capacity and trigger overwrite.
    buffer.push(4.4); // Buffer full now
    buffer.push(5.5); // Overwrites the oldest element (1.1)
    EXPECT_EQ(buffer.size(), 4);
    data = buffer.data();
    expected = {2.2, 3.3, 4.4, 5.5};
    ASSERT_EQ(data.size(), expected.size());
    for (size_t i = 0; i < data.size(); ++i)
    {
        EXPECT_DOUBLE_EQ(data[i], expected[i]);
    }
}

TEST(RingBufferExtraTest, OverwriteCandidate) {
    // Create a ring buffer with capacity 3.
    RingBuffer<int> buffer(3);

    // Fill the buffer.
    buffer.push(10);
    buffer.push(20);
    buffer.push(30);
    // With the buffer full, the candidate to be overwritten (oldest) should be 10.
    EXPECT_EQ(buffer.getOverwriteCandidate(), 10);

    // Push a new value to trigger an overwrite.
    buffer.push(40);  // This overwrites 10.
    // Now, the oldest element should be updated to 20.
    EXPECT_EQ(buffer.getOverwriteCandidate(), 20);

    // Push one more value.
    buffer.push(50); // Overwrites 20.
    EXPECT_EQ(buffer.getOverwriteCandidate(), 30);
}

TEST(RingBufferExtraTest, SnapshotConsistency) {
    // Create a ring buffer with capacity 5.
    RingBuffer<int> buffer(5);

    // Push some values.
    for (int i = 1; i <= 3; ++i) {
        buffer.push(i);
    }
    // Verify the snapshot returns the expected sequence.
    std::vector<int> snap = buffer.snapshot();
    std::vector<int> expected = {1, 2, 3};
    EXPECT_EQ(snap, expected);

    // Fill the buffer completely.
    buffer.push(4);
    buffer.push(5);   // Now buffer holds {1, 2, 3, 4, 5}
    snap = buffer.snapshot();
    expected = {1, 2, 3, 4, 5};
    EXPECT_EQ(snap, expected);

    // Push one more value to force an overwrite (overwriting 1).
    buffer.push(6);   // Expected new order: {2, 3, 4, 5, 6}
    snap = buffer.snapshot();
    expected = {2, 3, 4, 5, 6};
    EXPECT_EQ(snap, expected);
}
