#pragma once
#include "WeatherModel.h"
#include <string>

class WeatherService {
public:
    bool getWeather(const std::string& city, WeatherModel& weather,
        std::string& error) const;
};