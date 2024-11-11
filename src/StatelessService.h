#ifndef StatelessService_h
#define StatelessService_h

#include <map>
#include <ArduinoJson.h>
#include <EventSocket.h>

using JsonEventTrigger = const std::function<bool (const JsonVariant& content)>;
using JsonEventTriggerMap = std::map<const String, JsonEventTrigger>;

class JsonEventRouter
{
public:
    JsonEventRouter(JsonEventTriggerMap eventMap) :
        eventMap(eventMap) {}
    bool route(JsonVariant obj) 
    {
        bool success = false;
        for (auto const& e : eventMap) {
            if (obj[e.first].is<JsonVariant>()) if (e.second(obj[e.first])) success = true;
        }
        return success;
    }
private:
    const JsonEventTriggerMap eventMap;
};

using StateReader = const std::function<void (const JsonVariant& target)>;
using JsonStateReaderMap = std::map<const String, StateReader>;
using EventEmitter = std::function<void (JsonObject event)>;

class JsonStateRouter
{
public:
    JsonStateRouter(JsonStateReaderMap readerMap, EventEmitter emitter = [](JsonObject event) {}) :
        readerMap(readerMap),
        emitter(emitter) {}
    void route(JsonVariant target) 
    {
        String jsonStr;
        serializeJson(target, jsonStr);
        ESP_LOGI("State Router", "%s", jsonStr.c_str());
        JsonDocument doc;
        JsonObject obj = doc["obj"].to<JsonObject>();
        if (target.as<JsonObject>().size() > 0) {
            obj = target.as<JsonObject>();
            for (auto const& e : readerMap) if (obj[e.first].is<JsonVariant>()) e.second(obj[e.first].as<JsonVariant>());
        }
        else if (target.is<JsonVariant>()) {
            obj = target.to<JsonObject>();
            for (auto const& e : readerMap) e.second(obj[e.first].to<JsonVariant>());
        }
        emitter(target.as<JsonObject>());
        serializeJson(target, jsonStr);
        ESP_LOGI("State Router", "%s", jsonStr.c_str());
    }
private:
    const JsonStateReaderMap readerMap;
    const EventEmitter emitter;
};

class JsonRouter
{
public:
    JsonRouter(JsonEventTriggerMap eventRouterMap, JsonStateReaderMap stateReaderMap = {}, EventEmitter emitter = [](JsonObject event) {}) :
        eventRouter(eventRouterMap), stateRouter(stateReaderMap, emitter) {}
    bool route(JsonVariant content)
    {
        if (eventRouter.route(content)) return true;
        else stateRouter.route(content);
        return false;
    }
private:
    JsonEventRouter eventRouter;
    JsonStateRouter stateRouter;
};

class StatelessService
{
public:
    void begin() 
    {
        socket->registerEvent(eventName);
        socket->onEvent(eventName, [&](JsonObject &root, int originID) { route(root); });
        ESP_LOGI("Stateless Service", "Registered Json Event : %s", eventName);
    }
    bool route(JsonVariant contents) 
    {
        return router.route(contents);
    }
private:
    void emitEvent(JsonObject &json) 
    {
        String jsonStr;
        serializeJson(json, jsonStr);
        ESP_LOGI("State Router", "%s", jsonStr.c_str());
        socket->emitEvent(eventName, json);
    }
    const char* eventName;
    EventSocket *socket;
    JsonRouter router;
};

#endif