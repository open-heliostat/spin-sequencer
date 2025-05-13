#include "JsonTimerService.h"

// Define the main router
JsonRouter<JsonTimer> JsonTimerRouter::router = JsonRouter<JsonTimer>(
{
    {"add", [](JsonVariant content, JsonTimer &timer) {
        if (content.is<JsonObject>()) {
            JsonObjectConst obj = content.as<JsonObjectConst>();
            if (obj.containsKey("hour") && obj.containsKey("minute") && obj.containsKey("command")) {
                timer.addJsonDailyTimer(
                    obj["hour"].as<uint8_t>(),
                    obj["minute"].as<uint8_t>(),
                    obj["command"].as<String>()
                );
                return true;
            }
        }
        return false;
    }},
    {"remove", [](JsonVariant content, JsonTimer &timer) {
        if (content.is<JsonObject>()) {
            JsonObjectConst obj = content.as<JsonObjectConst>();
            if (obj.containsKey("index")) {
                return timer.removeTimer(obj["index"].as<size_t>());
            }
            if (obj.containsKey("hour") && obj.containsKey("minute")) {
                return timer.removeDailyTimer(
                    obj["hour"].as<uint8_t>(),
                    obj["minute"].as<uint8_t>()
                );
            }
        }
        return false;
    }}
},
{
    {"timers", [](JsonTimer &timer, JsonVariant target) {
        JsonArray timers = target.to<JsonArray>();
        for (const auto& t : timer.getTimers()) {
            JsonObject timerObj = timers.add<JsonObject>();
            timerObj["hour"] = t.hour;
            timerObj["minute"] = t.minute;
            timerObj["command"] = t.jsonCommand;
            timerObj["executed"] = t.executed;
        }
    }}
});

void JsonTimerService::begin() {
    _httpRouterEndpoint.begin();
    _fsPersistence.readFromFS();
}

void JsonTimerService::loop() {
}