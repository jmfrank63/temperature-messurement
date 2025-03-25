#include <iostream>
#include <random>
#include <cmath>
#include <unistd.h>
#include <limits.h>
#include <libgen.h>
#include <string>

#include "config.h"
#include "defaults.h"
#include "temperature_buffer.h"       // Naive implementation
#include "temperature_buffer_deque.h" // Fast implementation

// Returns the directory containing the running executable.
std::string getExecutableDir()
{
    char path[PATH_MAX];
    ssize_t count = readlink("/proc/self/exe", path, PATH_MAX);
    if (count == -1)
        return "";
    path[count] = '\0';
    return std::string(dirname(path));
}

// Generates a small random step.
static double generateRandomStep(std::mt19937 &engine)
{
    static std::normal_distribution<double> dist(0.0, 1.0);
    return std::round(dist(engine));
}

int main(int argc, char *argv[])
{
    std::string exeDir = getExecutableDir();
    std::string configPath = exeDir + "/" + defaults::DEFAULT_CONFIG_RELATIVE;
    bool useNaive = defaults::DEFAULT_USE_NAIVE;

    // Process command-line arguments.
    // Options: --fast|-f, --naive|-n, and --config|-c <path>
    for (int i = 1; i < argc; ++i)
    {
        std::string arg = argv[i];
        if (arg == "--fast" || arg == "-f")
        {
            useNaive = false;
        }
        else if (arg == "--naive" || arg == "-n")
        {
            useNaive = true;
        }
        else if (arg == "--config" || arg == "-c")
        {
            if (i + 1 < argc)
            {
                configPath = argv[i + 1];
                ++i; // Skip next argument since it is the config path.
            }
        }
    }

    Config cfg;
    if (!loadConfig(configPath, cfg))
    {
        cfg.baseOffset = defaults::BASE_OFFSET;
        cfg.amplitude = defaults::AMPLITUDE;
        cfg.daysInYear = defaults::DAYS_IN_YEAR;
        cfg.minTemp = defaults::MIN_TEMP;
        cfg.maxTemp = defaults::MAX_TEMP;
        cfg.driftFactor = defaults::DRIFT_FACTOR;
        cfg.bufferSize = defaults::BUFFER_SIZE;
        cfg.simulationValues = defaults::SIMULATION_VALUES;
    }

    std::random_device rd;
    std::mt19937 engine(rd());

    double currentTemp = cfg.baseOffset;

    // Pointers to a common interface.
    // Here we use lambda functions for demonstration.
    auto pushValue = [&](double value)
    {
        if (useNaive)
        {
            // Using the naive TemperatureBuffer.
            static TemperatureBuffer naiveBuffer(cfg.bufferSize);
            naiveBuffer.push(value);
            // For demonstration, print min and max.
            std::cout << "Naive: min = " << naiveBuffer.min() << ", max = " << naiveBuffer.max() << "\n";
        }
        else
        {
            // Using the fast deque-based TemperatureBuffer.
            static TemperatureBufferDeque fastBuffer(cfg.bufferSize);
            fastBuffer.push(value);
            std::cout << "Fast: min = " << fastBuffer.min() << ", max = " << fastBuffer.max() << "\n";
        }
    };

    std::cout << "Temperature Generator with " << (useNaive ? "Naive" : "Fast") << " min/max tracking\n";

    std::cout << "-------------------------------------------------\n";
    std::cout << "Simulation days: " << cfg.simulationValues << "\n";
    for (int value = 0; value < cfg.simulationValues; ++value)
    {
        double seasonalBase = cfg.baseOffset +
                              cfg.amplitude * std::sin((2.0 * M_PI * value) / cfg.daysInYear);

        currentTemp += generateRandomStep(engine);
        currentTemp += cfg.driftFactor * (seasonalBase - currentTemp);

        if (currentTemp < cfg.minTemp)
            currentTemp = cfg.minTemp;
        else if (currentTemp > cfg.maxTemp)
            currentTemp = cfg.maxTemp;

        // Push the measurement into the selected buffer.
        pushValue(currentTemp);

        std::cout << "Value " << (value + 1)
                  << ": " << currentTemp << " °C\n";
    }

    return 0;
}
