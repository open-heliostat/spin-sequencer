#ifndef CanControllerService_h
#define CanControllerService_h

#include <ESP32SvelteKit.h>
#include <StatelessService.h>
#include <HttpRouterEndpoint.h>
#include <FSPersistence.h>
#include "can.h"

class CanControllerJsonRouter
{
public:
    static bool route(JsonVariant content, CanIsoTPController<String>& controller)
    {
        return router.route(content, controller);
    }

    static void read(CanIsoTPController<String>& state, JsonObject& root)
    {
        router.serialize(state, root);
    }

    static void readForSave(CanIsoTPController<String>& state, JsonObject& root)
    {
        getSaveMap(root);
        router.serialize(state, root);
        JsonDocument ref = root;
        Serial.println(ref.as<String>());
        ref = getSaveMap();
        JsonSaveManager::filterFieldsRecursively(ref.as<JsonObject>(), root);
    }

    static StateUpdateResult update(JsonObject& root, CanIsoTPController<String>& state)
    {
        if (router.parse(root, state) && JsonSaveManager::needsToSave(root, getSaveMap())) {
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
        root["txId"] = true;
        root["rxId"] = true;
    }

    static JsonRouter<CanIsoTPController<String>> router;
};

class CanControllerService : public StatefulService<CanIsoTPController<String>&>
{
public:
    CanControllerService(PsychicHttpServer* server,
                        ESP32SvelteKit* sveltekit,
                        CanIsoTPController<String>& controller) :
        _httpRouterEndpoint(_router.read, 
                           _router.update, 
                           this, 
                           server, 
                           "/rest/can", 
                           sveltekit->getSecurityManager()),
        _fsPersistence(_router.readForSave,
                      _router.update,
                      this,
                      sveltekit->getFS(),
                      "/config/can.json"),
        StatefulService(controller) {}

    void begin();

    void loop();

private:
    HttpRouterEndpoint<CanIsoTPController<String>&> _httpRouterEndpoint;
    FSPersistence<CanIsoTPController<String>&> _fsPersistence;
    CanControllerJsonRouter _router;
};

#endif