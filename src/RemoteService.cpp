#include "RemoteService.h"

JsonRouter<RemotesController> RemoteJsonRouter::router = JsonRouter<RemotesController>(
{
    {"addRemote", [](JsonVariant content, RemotesController& controller) {
        if (content.is<JsonObject>()) {
            JsonObject remote = content.as<JsonObject>();
            if (remote["ip"].is<String>() || remote["rxId"].is<uint32_t>()) {
                controller.addRemote(
                    remote["hostname"].as<String>(),
                    remote["ip"].as<String>(),
                    remote["rxId"].as<uint32_t>()
                );
                return true;
            }
        }
        return false;
    }},
    {"removeRemote", [](JsonVariant content, RemotesController& controller) {
        if (content.is<JsonObject>()) {
            JsonObject remote = content.as<JsonObject>();
            if (remote["ip"].is<String>()) {
                controller.removeRemote(remote["ip"].as<String>());
                return true;
            } else if (remote["rxId"].is<String>()) {
                controller.removeRemote(remote["rxId"].as<uint32_t>());
                return true;
            } else if (remote["index"].is<int>()) {
                controller.removeRemote(remote["index"].as<int>());
                return true;
            }
        }
        return false;
    }},
    {"clearRemotes", [](JsonVariant content, RemotesController& controller) {
        controller.clearRemotes();
        return true;
    }}
},
{
    {"remotes", [](RemotesController& controller, JsonVariant content) {
        JsonArray remotes = content.to<JsonArray>();
        for (const auto& remote : controller.remotes) {
            JsonObject obj = remotes.add<JsonObject>();
            obj["hostname"] = remote.hostname;
            obj["ip"] = remote.ip;
            obj["rxId"] = remote.rxId;
        }
    }}
});

void RemoteService::begin()
{
    _httpRouterEndpoint.begin();
    _fsPersistence.readFromFS();
}

void RemoteService::loop()
{
    // No continuous operations needed
}