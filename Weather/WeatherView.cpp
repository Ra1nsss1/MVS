#include "WeatherView.h"
#include <iomanip>
#include <iostream>
#include <string>

void WeatherView::run(const WeatherController& controller) const {
    std::cout << "Пошук погоди за містом\n"
        << "Введіть місто або 0 для виходу.\n";
    std::string city;
    while (true) {
        std::cout << "\nМісто: ";
        if (!std::getline(std::cin, city) || city == "0") break;

        WeatherModel weather;
        std::string error;
        if (!controller.search(city, weather, error)) {
            std::cout << error << '\n';
            continue;
        }
        std::cout << "Погода у місті " << weather.getCity() << ":\n"
            << "Температура: " << std::fixed << std::setprecision(1)
            << weather.getTemperature() << " °C\n"
            << "Вологість: " << weather.getHumidity() << " %\n"
            << "Швидкість вітру: " << weather.getWindSpeed()
            << " км/год\n";
    }
}