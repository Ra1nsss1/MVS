#include "WeatherController.h"

bool WeatherController::search(const std::string& city,
    WeatherModel& weather,
    std::string& error) const {
    if (city.find_first_not_of(" \t") == std::string::npos) {
        error = "Введіть назву міста.";
        return false;
    }
    return service.getWeather(city, weather, error);
}