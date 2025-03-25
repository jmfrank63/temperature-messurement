//// filepath: /home/johannes/Projects/C++/temperature-messurement/src/config.cpp
#include "config.h"
#include "defaults.h"

#include <fstream>
#include <sstream>
#include <iostream>

bool loadConfig(const std::string &filename, Config &cfg)
{
    std::ifstream file(filename);
    if (!file.is_open())
    {
        return false; // Could not open file -> use defaults
    }

    // Temporary variables
    double baseOffset = defaults::BASE_OFFSET;
    double amplitude = defaults::AMPLITUDE;
    double daysInYear = defaults::DAYS_IN_YEAR;
    double minTemp = defaults::MIN_TEMP;
    double maxTemp = defaults::MAX_TEMP;
    double driftFactor = defaults::DRIFT_FACTOR;
    int bufferSize = defaults::BUFFER_SIZE;
    int simulationValues = defaults::SIMULATION_VALUES;

    std::string line;
    while (std::getline(file, line))
    {
        // Skip empty lines or comment lines
        if (line.empty() || line[0] == '#')
        {
            continue;
        }
        std::istringstream iss(line);
        std::string key;
        double value;
        if (iss >> key >> value) {
            if (key == "baseOffset")       baseOffset  = value;
            else if (key == "amplitude")     amplitude   = value;
            else if (key == "daysInYear")    daysInYear  = value;
            else if (key == "minTemp")       minTemp     = value;
            else if (key == "maxTemp")       maxTemp     = value;
            else if (key == "driftFactor")   driftFactor = value;
            else if (key == "bufferSize")    bufferSize  = static_cast<int>(value);
            else if (key == "simulationValues") simulationValues = static_cast<int>(value);
        }
    }

    cfg.baseOffset = baseOffset;
    cfg.amplitude = amplitude;
    cfg.daysInYear = daysInYear;
    cfg.minTemp = minTemp;
    cfg.maxTemp = maxTemp;
    cfg.driftFactor = driftFactor;
    cfg.bufferSize = bufferSize;
    cfg.simulationValues = simulationValues;

    return true; // Successfully read at least some parameters
}
