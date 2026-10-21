#pragma once
#include "WeatherController.h"

class WeatherView {
public:
    void run(const WeatherController& controller) const;
};