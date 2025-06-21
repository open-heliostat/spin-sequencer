#include "JsonTimerService.h"

// Define the main router
JsonRouter<JsonTimer> JsonTimerRouter::router = JsonRouter<JsonTimer>(
{
    {"add", [](JsonVariant content, JsonTimer &timer) {
        if (content.is<JsonObject>()) {
            JsonObject obj = content.as<JsonObject>();
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
    }},
    {"timers", [](JsonVariant content, JsonTimer &timer) {
        if (content.is<JsonArray>()) {
            JsonArray arr = content.as<JsonArray>();
            for (JsonVariant item : arr) {
                if (item.is<JsonObject>()) {
                    JsonObject obj = item.as<JsonObject>();
                    if (obj["hour"].is<uint8_t>() && 
                        obj["minute"].is<uint8_t>() && 
                        obj["command"].is<const char*>()) {
                        
                        DaysOfWeek days = obj["days"].is<uint8_t>() ? 
                            obj["days"].as<uint8_t>() : DOW_ALL;

                        // Create timer and restore state
                        timer.addJsonDailyTimer(
                            obj["hour"].as<uint8_t>(),
                            obj["minute"].as<uint8_t>(),
                            days,
                            obj["command"].as<String>()
                        );
                        
                        // Restore executed state and lastExecuted timestamp if available
                        auto& timers = timer.getTimersRef();
                        if (!timers.empty()) {
                            auto& lastTimer = timers.back();
                            if (obj["executed"].is<bool>()) {
                                lastTimer.executed = obj["executed"].as<bool>();
                            }
                            if (obj["lastExecuted"].is<time_t>()) {
                                lastTimer.lastExecuted = obj["lastExecuted"].as<time_t>();
                            }
                        }
                    }
                }
            }
            return true;
        }
        return false;
    }},
    {"clear", [](JsonVariant content, JsonTimer &timer) {
        timer.getTimersRef().clear();
        return true;
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
            timerObj["lastExecuted"] = t.lastExecuted;
        }
    }}
});

void JsonTimerService::begin() {
    _httpRouterEndpoint.begin();
    _fsPersistence.readFromFS();
}

void JsonTimerService::loop() {
}