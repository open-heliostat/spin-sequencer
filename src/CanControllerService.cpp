#include "CanControllerService.h"

JsonRouter<CanIsoTPController<CanIsoTPMessage>> CanControllerJsonRouter::router = JsonRouter<CanIsoTPController<CanIsoTPMessage>>(
{
    {"sendMessage", [](JsonVariant value, CanIsoTPController<CanIsoTPMessage>& controller) {
        if (!value.is<String>()) value.to<JsonObject>()["error"] = "NOT A STRING";
        CanIsoTPMessage message;
        strcpy(message.message, value.as<String>().c_str());
        if (controller.sendMessage(message)) value.to<JsonObject>()["error"] = "OK";
        else value.to<JsonObject>()["error"] = "FAILED";
        return false;
    }},
    {"txId", [](JsonVariant value, CanIsoTPController<CanIsoTPMessage>& controller) {
        if (value.is<uint32_t>()) {
            controller.txId = value.as<uint32_t>();
            ESP_LOGI("CAN", "Set txId: %s", value.as<String>().c_str());
            return true;
        }
        ESP_LOGI("CAN", "Received txId: %s", value.as<String>().c_str());
        return false;
    }},
    {"rxId", [](JsonVariant value, CanIsoTPController<CanIsoTPMessage>& controller) {
        if (value.is<uint32_t>()) {
            controller.rxId = value.as<uint32_t>();
            return true;
        }
        return false;
    }},
},
{
    {"messageHistory", [](CanIsoTPController<CanIsoTPMessage>& controller, JsonVariant content) {
        JsonArray messages = content.to<JsonArray>();
        // convert char array to string
        for (int i = 0; i < 10; i++) {
            // if (controller.getMessageHistory()[i].message[0] != '\0') {
                // create a new string and make sure it is null terminated
                String message = String(controller.getMessageHistory()[i].message);
                message.trim();
                messages.add(message);
            // }
        }
    }},
    {"txId", [](CanIsoTPController<CanIsoTPMessage>& controller, JsonVariant content) {
        content.set(controller.txId);
    }},
    {"rxId", [](CanIsoTPController<CanIsoTPMessage>& controller, JsonVariant content) {
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