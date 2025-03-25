#include <iostream>
#include <random>
#include <cmath>
#include "config.h"
#include "defaults.h"
#include "temperature_buffer.h"

// Generates a small random step.
static double generateRandomStep(std::mt19937 &engine) {
    static std::normal_distribution<double> dist(0.0, 1.0);
    return std::round(dist(engine));
}

int main() {
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

    double currentTemp = cfg.baseOffset;

    std::cout << "Temperature Generator with Min/Max Tracking\n"
              << "---------------------------------------------\n";

    TemperatureBuffer tempBuffer(cfg.bufferSize);

    for (int day = 0; day < 100; ++day) {
        double seasonalBase = cfg.baseOffset +
                              cfg.amplitude * std::sin((2.0 * M_PI * day) / cfg.daysInYear);

        currentTemp += generateRandomStep(engine);
        currentTemp += cfg.driftFactor * (seasonalBase - currentTemp);

        if (currentTemp < cfg.minTemp) {
            currentTemp = cfg.minTemp;
        } else if (currentTemp > cfg.maxTemp) {
            currentTemp = cfg.maxTemp;
        }

        tempBuffer.push(currentTemp);

        std::cout << "Day " << (day + 1) << ": " << currentTemp << " °C\n";
    }

    std::cout << "\nBuffer contains " << tempBuffer.size() << " measurements.\n"
              << "Minimum temperature: " << tempBuffer.min() << "\n"
              << "Maximum temperature: " << tempBuffer.max() << "\n";

    return 0;
}
