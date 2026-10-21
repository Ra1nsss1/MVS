#include "WeatherController.h"
#include "WeatherView.h"
#include <Windows.h>
int main() {
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);
    WeatherController controller;
    WeatherView view;
    view.run(controller);
    return 0;
}