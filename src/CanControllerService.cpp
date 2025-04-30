#include "CanControllerService.h"

JsonRouter<CanIsoTPController<CanIsoTPMessage>> CanControllerJsonRouter::router = JsonRouter<CanIsoTPController<CanIsoTPMessage>>(
{
    {"sendMessage", [](JsonVariant value, CanIsoTPController<CanIsoTPMessage>& controller) {
        CanIsoTPMessage message;
        if (value.is<JsonObject>() && value["message"].is<String>()) {
            strcpy(message.message, value["message"].as<String>().c_str());
            // make sure the message is null terminated
            message.message[sizeof(message.message) - 1] = '\0';
            if (value["txId"].is<uint32_t>()) {
                if (controller.sendMessage(message, value["txId"].as<uint32_t>())) value.as<JsonObject>()["error"] = "OK";
                else value.as<JsonObject>()["error"] = "FAILED";
            }
            else if (controller.sendMessage(message)) value.as<JsonObject>()["error"] = "OK";
            else value.as<JsonObject>()["error"] = "FAILED";
        }
        else if (value.is<String>()) {
            strcpy(message.message, value.as<String>().c_str());
            // make sure the message is null terminated
            message.message[sizeof(message.message) - 1] = '\0';
            if (controller.sendMessage(message)) value.as<JsonObject>()["error"] = "OK";
            else value.as<JsonObject>()["error"] = "FAILED";
        }
        else {
            value.as<JsonObject>()["error"] = "INVALID MESSAGE";
        }
        return false;
    }},
    {"txId", [](JsonVariant value, CanIsoTPController<CanIsoTPMessage>& controller) {
        if (value.is<uint32_t>()) {
            controller.setTxId(value.as<uint32_t>());
            return true;
        }
        return false;
    }},
    {"rxId", [](JsonVariant value, CanIsoTPController<CanIsoTPMessage>& controller) {
        if (value.is<uint32_t>()) {
            controller.setRxId(value.as<uint32_t>());
            return true;
        }
        return false;
    }},
    {"enabled", [](JsonVariant value, CanIsoTPController<CanIsoTPMessage>& controller) {
        if (value.is<bool>()) {
            controller.enabled = value.as<bool>();
            return true;
        }
        return false;
    }}
},
{
    {"messageHistory", [](CanIsoTPController<CanIsoTPMessage>& controller, JsonVariant content) {
        JsonArray messages = content.to<JsonArray>();
        // create array from message history vector
        int size = controller.getMessageHistory().size();
        if (size > 0) {
            String msgs[size];
            for (int i = 0; i < size; i++) {
                msgs[i] = controller.getMessageHistory()[i];
            }
            copyArray(msgs, size, messages);
        }
    }},
    {"txId", [](CanIsoTPController<CanIsoTPMessage>& controller, JsonVariant content) {
        content.set(controller.txId);
    }},
    {"rxId", [](CanIsoTPController<CanIsoTPMessage>& controller, JsonVariant content) {
        content.set(controller.rxId);
    }},
    {"enabled", [](CanIsoTPController<CanIsoTPMessage>& controller, JsonVariant content) {
        content.set(controller.enabled);
    }}
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