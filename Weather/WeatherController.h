#pragma once
#include "WeatherModel.h"
#include "WeatherService.h"
#include <string>

class WeatherController {
private:
    WeatherService service;

public:
    bool search(const std::string& city, WeatherModel& weather,
        std::string& error) const;
};