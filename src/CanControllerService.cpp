#include "CanControllerService.h"

JsonRouter<CanIsoTPController<String>> CanControllerJsonRouter::router = JsonRouter<CanIsoTPController<String>>(
{
    {"sendMessage", [](JsonVariant value, CanIsoTPController<String>& controller) {
        if (!value.is<String>()) return false;
        if (controller.sendMessage(value.as<String>())) value.as<String>() = "OK";
        else value.as<String>() = "FAILED";
        return true;
    }},
    {"txId", [](JsonVariant value, CanIsoTPController<String>& controller) {
        if (value.is<uint32_t>()) {
            controller.txId = value.as<uint32_t>();
            ESP_LOGI("CAN", "Set txId: %s", value.as<String>().c_str());
            return true;
        }
        ESP_LOGI("CAN", "Received txId: %s", value.as<String>().c_str());
        return false;
    }},
    {"rxId", [](JsonVariant value, CanIsoTPController<String>& controller) {
        if (value.is<uint32_t>()) {
            controller.rxId = value.as<uint32_t>();
            return true;
        }
        return false;
    }},
},
{
    {"messageHistory", [](CanIsoTPController<String>& controller, JsonVariant content) {
        JsonArray messages = content.to<JsonArray>();
        copyArray(controller.getMessageHistory(), 10, messages);
    }},
    {"txId", [](CanIsoTPController<String>& controller, JsonVariant content) {
        content.set(controller.txId);
    }},
    {"rxId", [](CanIsoTPController<String>& controller, JsonVariant content) {
        content.set(controller.rxId);
    }},
});

void CanControllerService::begin()
{
    _httpRouterEndpoint.begin();
    _fsPersistence.readFromFS();
    _state.begin();
}

void CanControllerService::loop()
{
    _state.loop();
}