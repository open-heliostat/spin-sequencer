#include "JsonTimerService.h"

// Define the main router
JsonRouter<JsonTimer> JsonTimerRouter::router = JsonRouter<JsonTimer>(
{
    {"add", [](JsonVariant content, JsonTimer &timer) {
        if (content.is<JsonObject>()) {
            JsonObjectConst obj = content.as<JsonObjectConst>();
            if (obj["hour"].is<uint8_t>() && 
                obj["minute"].is<uint8_t>() && 
                obj["command"].is<const char*>()) {
                
                // Default to all days if not specified
                DaysOfWeek days = obj["days"].is<uint8_t>() ? 
                    obj["days"].as<uint8_t>() : DOW_ALL;

                timer.addJsonDailyTimer(
                    obj["hour"].as<uint8_t>(),
                    obj["minute"].as<uint8_t>(),
                    days,
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
            if (obj["index"].is<size_t>()) {
                return timer.removeTimer(obj["index"].as<size_t>());
            }
            if (obj["hour"].is<uint8_t>() && 
                obj["minute"].is<uint8_t>()) {
                DaysOfWeek days = obj["days"].is<uint8_t>() ? 
                    obj["days"].as<uint8_t>() : DOW_ALL;
                    
                return timer.removeDailyTimer(
                    obj["hour"].as<uint8_t>(),
                    obj["minute"].as<uint8_t>(),
                    days
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
            timerObj["days"] = t.days;
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