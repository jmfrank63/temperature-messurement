#pragma once

namespace defaults {
    // Default seasonal sinus parameters
    inline constexpr double BASE_OFFSET = 15.0;    // Average temperature
    inline constexpr double AMPLITUDE   = 10.0;    // Peak deviation
    inline constexpr double DAYS_IN_YEAR= 365.0;

    // Define min and max possible temperatures
    inline constexpr double MIN_TEMP    = -20.0;
    inline constexpr double MAX_TEMP    = 50.0;

    // Default drift factor
    inline constexpr double DRIFT_FACTOR = 0.1;

    // default ring buffer size
    inline constexpr int BUFFER_SIZE = 50;

    // default simulation days
    inline constexpr int SIMULATION_VALUES = 1000;

    // Command line default values:
    inline const std::string DEFAULT_CONFIG_RELATIVE = "../share/temperature_measurement/config/temperature_config.txt";
    inline constexpr bool DEFAULT_USE_NAIVE = true;
}
