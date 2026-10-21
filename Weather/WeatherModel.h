#pragma once
#include <string>

class WeatherModel {
private:
    std::string city;
    double temperature;
    int humidity;
    double windSpeed;

public:
    WeatherModel();
    WeatherModel(const std::string& city, double temperature,
        int humidity, double windSpeed);
    const std::string& getCity() const;
    double getTemperature() const;
    int getHumidity() const;
    double getWindSpeed() const;
};