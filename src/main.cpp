#include <iostream>
#include <fstream>
#include <random>
#include <cmath>
#include <string>
#include <thread>
#include <atomic>
#include <chrono>
#include <unistd.h>
#include <limits.h>
#include <libgen.h>

#include "config.h"
#include "defaults.h"
#include "temperature_buffer.h"       // Naive implementation
#include "temperature_buffer_deque.h" // buffer (deque-based) implementation

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
    for (int i = 1; i < argc; ++i)
    {
        std::string arg = argv[i];
        if (arg == "--buffer" || arg == "-b")
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
                ++i;
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

    std::cout << "Temperature Generator with " << (useNaive ? "Naive" : "buffer")
              << " min/max tracking\n";
    std::cout << "Using config file: " << configPath << "\n";
    std::cout << "Simulation values: " << cfg.simulationValues << "\n";
    std::cout << "-------------------------------------------------\n";

    // We'll use an atomic flag to signal when the generator is done.
    std::atomic<bool> generatorDone(false);

    if (useNaive)
    {
        TemperatureBuffer buffer(cfg.bufferSize);
        std::thread generator([&]()
                              {
            for (int day = 0; day < cfg.simulationValues; ++day)
            {
                double seasonalBase = cfg.baseOffset +
                                      cfg.amplitude * std::sin((2.0 * M_PI * day) / cfg.daysInYear);
                currentTemp += generateRandomStep(engine);
                currentTemp += cfg.driftFactor * (seasonalBase - currentTemp);
                if (currentTemp < cfg.minTemp)
                    currentTemp = cfg.minTemp;
                else if (currentTemp > cfg.maxTemp)
                    currentTemp = cfg.maxTemp;
                buffer.push(currentTemp);
            }
            generatorDone.store(true); });

        // Open output file for streaming values.
        std::ofstream outFile("temperature_output.txt");
        if (!outFile.is_open())
        {
            std::cerr << "Failed to open output file.\n";
            return 1;
        }

        double value;
        // Consumer loop: while the generator is running or there are values in the buffer.
        while (!generatorDone.load() || buffer.size() > 0)
        {
            // Pop and write values if available.
            if (buffer.pop(value))
            {
                outFile << value << "\n";
            }
            else
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        }
        generator.join();
        outFile.close();
        std::cout << "Final Naive: min = " << buffer.min()
                  << ", max = " << buffer.max() << "\n";
    }
    else
    {
        TemperatureBufferDeque buffer(cfg.bufferSize);
        std::thread generator([&]()
                              {
            for (int day = 0; day < cfg.simulationValues; ++day)
            {
                double seasonalBase = cfg.baseOffset +
                    cfg.amplitude * std::sin((2.0 * M_PI * day) / cfg.daysInYear);
                currentTemp += generateRandomStep(engine);
                currentTemp += cfg.driftFactor * (seasonalBase - currentTemp);
                if (currentTemp < cfg.minTemp)
                    currentTemp = cfg.minTemp;
                else if (currentTemp > cfg.maxTemp)
                    currentTemp = cfg.maxTemp;
                buffer.push(currentTemp);
            }
            generatorDone.store(true); });

        // Open output file for streaming values.
        std::ofstream outFile("temperature_output.txt");
        if (!outFile.is_open())
        {
            std::cerr << "Failed to open output file.\n";
            return 1;
        }

        double value;
        // Consumer loop: while the generator is running or there are values in the buffer.
        while (!generatorDone.load() || buffer.size() > 0)
        {
            if (buffer.pop(value))
            {
                outFile << value << "\n";
            }
            else
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        }
        generator.join();
        outFile.close();
        std::cout << "Final buffer: min = " << buffer.min()
                  << ", max = " << buffer.max() << "\n";
    }

    return 0;
}
