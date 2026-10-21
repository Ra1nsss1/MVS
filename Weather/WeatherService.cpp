#include "WeatherService.h"
#include <windows.h>
#include <winhttp.h>
#include <cerrno>
#include <cmath>
#include <cstdlib>
#include <iomanip>
#include <sstream>
#pragma comment(lib, "winhttp.lib")
namespace {
    std::string encodeCity(const std::string& city) {
        std::ostringstream encoded;
        encoded << std::uppercase << std::hex;
        for (unsigned char ch : city) {
            if ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z') ||
                (ch >= '0' && ch <= '9') || ch == '-' || ch == '_') {
                encoded << static_cast<char>(ch);
            }
            else {
                encoded << '%' << std::setw(2) << std::setfill('0')
                    << static_cast<int>(ch);
            }
        }
        return encoded.str();
    }
    bool readNumber(const std::string& json, const std::string& key,
        size_t from, double& value) {
        size_t position = json.find('"' + key + '"', from);
        if (position == std::string::npos) return false;
        position = json.find(':', position);
        if (position == std::string::npos) return false;
        const char* start = json.c_str() + position + 1;
        char* end = nullptr;
        errno = 0;
        value = std::strtod(start, &end);
        return start != end && errno != ERANGE && std::isfinite(value);
    }
    bool getJson(const wchar_t* host, const std::string& path,
        std::string& body, std::string& error) {
        HINTERNET session = WinHttpOpen(L"WeatherMVC/1.0",
            WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME,
            WINHTTP_NO_PROXY_BYPASS, 0);
        if (!session) {
            error = "Не вдалося відкрити мережеве з'єднання.";
            return false;
        }
        HINTERNET connection = nullptr;
        HINTERNET request = nullptr;
        bool success = false;
        do {
            WinHttpSetTimeouts(session, 5000, 5000, 5000, 10000);
            connection = WinHttpConnect(session, host, INTERNET_DEFAULT_HTTPS_PORT, 0);
            if (!connection) break;
            std::wstring widePath(path.begin(), path.end());
            request = WinHttpOpenRequest(connection, L"GET", widePath.c_str(),
                nullptr, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES,
                WINHTTP_FLAG_SECURE);
            if (!request) break;
            if (!WinHttpSendRequest(request, WINHTTP_NO_ADDITIONAL_HEADERS, 0,
                WINHTTP_NO_REQUEST_DATA, 0, 0, 0) ||
                !WinHttpReceiveResponse(request, nullptr)) break;
            DWORD status = 0;
            DWORD statusSize = sizeof(status);
            if (!WinHttpQueryHeaders(request,
                WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
                WINHTTP_HEADER_NAME_BY_INDEX, &status, &statusSize,
                WINHTTP_NO_HEADER_INDEX)) break;
            if (status != 200) {
                error = "Сервіс погоди відповів з помилкою.";
                break;
            }
            char buffer[4096];
            DWORD bytesRead = 0;
            success = true;
            while (WinHttpReadData(request, buffer, sizeof(buffer), &bytesRead)) {
                if (bytesRead == 0) break;
                body.append(buffer, bytesRead);
                if (body.size() > 262144) {
                    success = false;
                    error = "Відповідь сервісу завелика.";
                    break;
                }
            }
            if (bytesRead != 0) success = false;
        } while (false);
        if (!success && error.empty())
            error = "Не вдалося отримати дані. Перевірте інтернет-з'єднання.";
        if (request) WinHttpCloseHandle(request);
        if (connection) WinHttpCloseHandle(connection);
        WinHttpCloseHandle(session);
        return success;
    }
}
bool WeatherService::getWeather(const std::string& city,
    WeatherModel& weather,
    std::string& error) const {
    std::string locationJson;
    std::string locationPath = "/v1/search?name=" + encodeCity(city) +
        "&count=1&language=uk&format=json";
    if (!getJson(L"geocoding-api.open-meteo.com", locationPath,
        locationJson, error)) return false;
    size_t results = locationJson.find("\"results\"");
    if (results == std::string::npos) {
        error = "Місто не знайдено. Перевірте назву й спробуйте ще раз.";
        return false;
    }
    double latitude = 0;
    double longitude = 0;
    if (!readNumber(locationJson, "latitude", results, latitude) ||
        !readNumber(locationJson, "longitude", results, longitude)) {
        error = "Не вдалося прочитати координати міста.";
        return false;
    }
    std::ostringstream forecastPath;
    forecastPath << "/v1/forecast?latitude=" << latitude
        << "&longitude=" << longitude
        << "&current=temperature_2m,relative_humidity_2m,wind_speed_10m";
    std::string forecastJson;
    if (!getJson(L"api.open-meteo.com", forecastPath.str(),
        forecastJson, error)) return false;
    size_t current = forecastJson.find("\"current\"");
    double temperature = 0;
    double humidity = 0;
    double windSpeed = 0;
    if (current == std::string::npos ||
        !readNumber(forecastJson, "temperature_2m", current, temperature) ||
        !readNumber(forecastJson, "relative_humidity_2m", current, humidity) ||
        !readNumber(forecastJson, "wind_speed_10m", current, windSpeed)) {
        error = "Не вдалося прочитати поточну погоду.";
        return false;
    }
    weather = WeatherModel(city, temperature,
        static_cast<int>(humidity), windSpeed);
    return true;
}