//// filepath: /home/johannes/Projects/C++/temperature-messurement/include/config.h
#pragma once

#include <string>

struct Config {
    double baseOffset;
    double amplitude;
    double daysInYear;
    double minTemp;
    double maxTemp;
    double driftFactor;
};

// Reads config from file 'filename'. On success, fills in cfg and returns true.
// On failure, returns false, so the caller can use defaults.
bool loadConfig(const std::string &filename, Config &cfg);
