//// filepath: /home/johannes/Projects/C++/temperature-messurement/src/main.cpp
#include <iostream>
#include <random>
#include <cmath>

#include "config.h"
#include "defaults.h"

// Generates a small random step, favoring small +/-1 changes
static double generateRandomStep(std::mt19937& engine) {
    static std::normal_distribution<double> dist(0.0, 1.0);
    return std::round(dist(engine));
}

int main() {
    // Load configuration from file or fall back to defaults
    Config cfg;
    if (!loadConfig("temperature_config.txt", cfg)) {
        cfg.baseOffset  = defaults::BASE_OFFSET;
        cfg.amplitude   = defaults::AMPLITUDE;
        cfg.daysInYear  = defaults::DAYS_IN_YEAR;
        cfg.minTemp     = defaults::MIN_TEMP;
        cfg.maxTemp     = defaults::MAX_TEMP;
        cfg.driftFactor = defaults::DRIFT_FACTOR;
    }

    std::random_device rd;
    std::mt19937 engine(rd());

    double currentTemp = cfg.baseOffset;  // Start near the average temperature

    std::cout << "Seasonal + Random Walk Temperature Generator\n"
              << "---------------------------------------------\n";

    // Generate a few days of data
    for (int day = 0; day < 30; ++day) {
        // Average (seasonal) temperature for the day
        double seasonalBase = cfg.baseOffset
            + cfg.amplitude * std::sin((2.0 * M_PI * day) / cfg.daysInYear);

        // Add the random walk step
        currentTemp += generateRandomStep(engine);

        // Pull currentTemp back toward seasonalBase
        currentTemp += cfg.driftFactor * (seasonalBase - currentTemp);

        // Clamp the temperature
        if (currentTemp < cfg.minTemp) {
            currentTemp = cfg.minTemp;
        } else if (currentTemp > cfg.maxTemp) {
            currentTemp = cfg.maxTemp;
        }

        std::cout << "Day " << (day + 1)
                  << ": " << currentTemp << " °C\n";
    }

    return 0;
}
