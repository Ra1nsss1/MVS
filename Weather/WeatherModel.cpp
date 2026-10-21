#include "WeatherModel.h"

WeatherModel::WeatherModel()
    : temperature(0), humidity(0), windSpeed(0) {
}

WeatherModel::WeatherModel(const std::string& city, double temperature,
    int humidity, double windSpeed)
    : city(city), temperature(temperature), humidity(humidity),
    windSpeed(windSpeed) {
}

const std::string& WeatherModel::getCity() const {
    return city;
}

double WeatherModel::getTemperature() const {
    return temperature;
}

int WeatherModel::getHumidity() const {
    return humidity;
}

double WeatherModel::getWindSpeed() const {
    return windSpeed;
}