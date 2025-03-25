#include <iostream>
#include <random>
#include <cmath>
#include "config.h"
#include "defaults.h"
#include "ring_buffer.h"

// Generates a small random step, favoring small +/-1 changes.
static double generateRandomStep(std::mt19937 &engine) {
    static std::normal_distribution<double> dist(0.0, 1.0);
    return std::round(dist(engine));
}

int main() {
    // Load configuration from file or use defaults.
    Config cfg;
    if (!loadConfig("config/temperature_config.txt", cfg)) {
        cfg.baseOffset  = defaults::BASE_OFFSET;
        cfg.amplitude   = defaults::AMPLITUDE;
        cfg.daysInYear  = defaults::DAYS_IN_YEAR;
        cfg.minTemp     = defaults::MIN_TEMP;
        cfg.maxTemp     = defaults::MAX_TEMP;
        cfg.driftFactor = defaults::DRIFT_FACTOR;
        cfg.bufferSize  = defaults::BUFFER_SIZE;
    }

    std::random_device rd;
    std::mt19937 engine(rd());

    // Start near average temperature.
    double currentTemp = cfg.baseOffset;

    std::cout << "Seasonal + Random Walk Temperature Generator with Ring Buffer\n"
              << "----------------------------------------------------------------\n";

    // Create a ring buffer to store the temperatures.
    RingBuffer<double> buffer(cfg.bufferSize);

    // Simulate 100 days of measurements.
    for (int day = 0; day < 100; ++day) {
        double seasonalBase = cfg.baseOffset
            + cfg.amplitude * std::sin((2.0 * M_PI * day) / cfg.daysInYear);

        currentTemp += generateRandomStep(engine);
        currentTemp += cfg.driftFactor * (seasonalBase - currentTemp);

        if (currentTemp < cfg.minTemp) {
            currentTemp = cfg.minTemp;
        } else if (currentTemp > cfg.maxTemp) {
            currentTemp = cfg.maxTemp;
        }

        // Write measurement into the ring buffer.
        buffer.push(currentTemp);

        std::cout << "Day " << (day + 1) << ": " << currentTemp << " °C\n";
    }

    // Optionally, get the data from the ring buffer and process or display it.
    auto data = buffer.data();
    std::cout << "\nStored " << data.size() << " measurements in the ring buffer.\n";

    return 0;
}
