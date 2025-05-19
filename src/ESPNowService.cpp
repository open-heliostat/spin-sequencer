#include "ESPNowService.h"

JsonRouter<ESPNowState> ESPNowJsonRouter::router = JsonRouter<ESPNowState>(
{
    {"sendMessage", [](JsonVariant value, ESPNowState& controller) {
        if (value.is<JsonObject>()) {
            JsonObject obj = value.as<JsonObject>();
            if (obj["message"].is<String>() && obj["address"].is<String>()) {
                String message = obj["message"].as<String>();
                String address = obj["address"].as<String>();
                // Parse MAC address string to uint8_t array
                uint8_t macAddr[6];
                sscanf(address.c_str(), "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx",
                       &macAddr[0], &macAddr[1], &macAddr[2],
                       &macAddr[3], &macAddr[4], &macAddr[5]);
                
                if (ESPNow::sendMessage(message, macAddr)) {
                    obj["error"] = "OK";
                } else {
                    obj["error"] = "FAILED";
                }
            }
        }
        return false;
    }},
    {"broadcast", [](JsonVariant value, ESPNowState& controller) {
        if (value.is<String>()) {
            String message = value.as<String>();
            ESPNow::broadcast(message);
            return true;
        }
        return false;
    }},
    {"reply", [](JsonVariant value, ESPNowState& controller) {
        if (value.is<String>()) {
            String message = value.as<String>();
            ESPNow::reply(message);
            return true;
        }
        return false;
    }},
    {"ping", [](JsonVariant value, ESPNowState& controller) {
        if (value.is<String>()) {
            String address = value.as<String>();
            uint8_t macAddr[6];
            sscanf(address.c_str(), "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx",
                   &macAddr[0], &macAddr[1], &macAddr[2],
                   &macAddr[3], &macAddr[4], &macAddr[5]);
            ESPNow::ping(macAddr);
            return true;
        }
        return false;
    }}
},
{
    {"lastAddress", [](ESPNowState& controller, JsonVariant content) {
        char macStr[18];
        ESPNow::formatMacAddress(ESPNow::lastAddress, macStr, 18);
        content.set(macStr);
    }},
});

void ESPNowService::begin()
{
    _httpRouterEndpoint.begin();
    _fsPersistence.readFromFS();
    ESPNow::setup();
}

void ESPNowService::loop()
{
    ESPNow::update(millis());
}