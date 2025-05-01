#include <HTTPLocalClient.h>

String HTTPGetLocal(const String& path) {
    HTTPClient http;
    http.begin("http://127.0.0.1/rest/" + path);
    int httpCode = http.GET();
    if (httpCode > 0) {
        String payload = http.getString();
        ESP_LOGI("HTTP", "Received response: %s", payload.c_str());
        return payload;
    } else {
        ESP_LOGI("HTTP", "Failed to fetch data: %d", httpCode);
        return "";
    }
}

String HTTPPostLocal(const String& path, const String& payload) {
    HTTPClient http;
    http.begin("http://127.0.0.1/rest/" + path);
    http.addHeader("Content-Type", "application/json");
    int httpCode = http.POST(payload);
    if (httpCode > 0) {
        String payload = http.getString();
        ESP_LOGI("HTTP", "Received response: %s", payload.c_str());
        return payload;
    } else {
        ESP_LOGI("HTTP", "Failed to fetch data: %d", httpCode);
        return "";
    }
}