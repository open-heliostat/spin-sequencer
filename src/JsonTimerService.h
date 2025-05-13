#ifndef JsonTimerService_h
#define JsonTimerService_h

#include <ESP32SvelteKit.h>
#include <StatefulService.h>
#include <HttpRouterEndpoint.h>
#include <FSPersistence.h>
#include "JsonTimer.h"

class JsonTimerRouter 
{
public:
    static bool route(JsonVariant content, JsonTimer& timer)
    {
        return router.route(content, timer);
    }

    static void read(JsonTimer& timer, JsonObject& root)
    {
        router.serialize(timer, root);
    }

    static void readForSave(JsonTimer& timer, JsonObject& root)
    {
        getSaveMap(root);
        router.serialize(timer, root);
        JsonDocument ref = root;
        ref = getSaveMap();
        JsonSaveManager::filterFieldsRecursively(ref.as<JsonObject>(), root);
    }

    static StateUpdateResult update(JsonObject& root, JsonTimer& timer)
    {
        if (router.parse(root, timer) && JsonSaveManager::needsToSave(root, getSaveMap())) {
            return StateUpdateResult::CHANGED;
        }
        return StateUpdateResult::UNCHANGED;
    }

    static const JsonDocument getSaveMap()
    {
        JsonDocument doc;
        JsonObject obj = doc.to<JsonObject>();
        getSaveMap(obj);
        return doc;
    }

    static const void getSaveMap(JsonObject& root)
    {
        root["timers"] = true;
    }

    static JsonRouter<JsonTimer> router;
};

class JsonTimerService : public StatefulService<JsonTimer>
{
public:
    JsonTimerService(PsychicHttpServer* server,
                    ESP32SvelteKit* sveltekit,
                    JsonSeq& sequencer) :
        _timer(sequencer),
        _httpRouterEndpoint(_router.read, 
                           _router.update, 
                           this, 
                           server, 
                           "/rest/timer", 
                           sveltekit->getSecurityManager()),
        _fsPersistence(_router.readForSave,
                      _router.update,
                      this,
                      sveltekit->getFS(),
                      "/config/timers.json"),
        StatefulService(_timer) {}

    void begin();
    void loop();

private:
    JsonTimer _timer;
    HttpRouterEndpoint<JsonTimer> _httpRouterEndpoint;
    FSPersistence<JsonTimer> _fsPersistence;
    JsonTimerRouter _router;
};

#endif