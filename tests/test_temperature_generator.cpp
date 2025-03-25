#include <gtest/gtest.h>
#include <random>
#include <cmath>
#include "config.h"
#include "defaults.h"

static double generateRandomStep(std::mt19937& engine) {
    static std::normal_distribution<double> dist(0.0, 0.7);
    return std::round(dist(engine));
}

TEST(TemperatureGeneratorTest, BoundsAndAverage) {
    // Load or fall back to defaults
    Config cfg;
    if (!loadConfig("temperature_config.txt", cfg)) {
        cfg.baseOffset  = defaults::BASE_OFFSET;
        cfg.amplitude   = defaults::AMPLITUDE;
        cfg.daysInYear  = defaults::DAYS_IN_YEAR;
        cfg.minTemp     = defaults::MIN_TEMP;
        cfg.maxTemp     = defaults::MAX_TEMP;
        cfg.driftFactor = defaults::DRIFT_FACTOR;
    }

    // std::random_device rd;
    std::mt19937 engine(12345); // Fixed seed for reproducibility

    double currentTemp = cfg.baseOffset;
    double totalTemp   = 0.0;
    int    numValues     = 100; // test with 100 iterations

    for (int value = 0; value < numValues; ++value) {
        // Compute seasonal base
        double seasonalBase = cfg.baseOffset
            + cfg.amplitude * std::sin((2.0 * M_PI * value) / cfg.daysInYear);

        // Add random step
        currentTemp += generateRandomStep(engine);

        // Pull back toward seasonal base
        currentTemp += cfg.driftFactor * (seasonalBase - currentTemp);

        // Clamp between min and max
        if (currentTemp < cfg.minTemp) {
            currentTemp = cfg.minTemp;
        } else if (currentTemp > cfg.maxTemp) {
            currentTemp = cfg.maxTemp;
        }

        // Accumulate for average check
        totalTemp += currentTemp;

        // Check bounds each iteration
        EXPECT_GE(currentTemp, cfg.minTemp);
        EXPECT_LE(currentTemp, cfg.maxTemp);
    }

    // Check that the average is near baseOffset
    double averageTemp = totalTemp / numValues;
    EXPECT_NEAR(averageTemp, cfg.baseOffset, 7.0); // Allow some margin due to randomness
}
