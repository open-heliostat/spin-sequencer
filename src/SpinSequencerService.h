#ifndef SpinSequencerService_h
#define SpinSequencerService_h

#include <ESP32SvelteKit.h>
#include <EventEndpoint.h>
#include <FSPersistence.h>
#include <StatelessService.h>
#include <HttpRouterEndpoint.h>
#include <ClosedLoopControllerService.h>

#include <spinsequencer.h>


class SpinSequencerControllerJsonRouter
{
public:
    static bool route(JsonVariant content, SpinSequencerController &controller)
    {
        return router.route(content, controller);
    }
    static void read(SpinSequencerController &state, JsonObject &root)
    {
        router.serialize(state, root);
    }
    static void readForSave(SpinSequencerController &state, JsonObject &root)
    {
        getSaveMap(root);
        router.serialize(state, root);
        JsonDocument ref = root;
        Serial.println(ref.as<String>());
        ref = getSaveMap();
        JsonSaveManager::filterFieldsRecursively(ref.as<JsonObject>(), root);
    }
    static StateUpdateResult update(JsonObject &root, SpinSequencerController &state)
    {
        if (router.parse(root, state) && JsonSaveManager::needsToSave(root, getSaveMap())) return StateUpdateResult::CHANGED;
        else return StateUpdateResult::UNCHANGED;
    }
    static const JsonDocument getSaveMap()
    {
        JsonDocument doc;
        JsonObject obj = doc.to<JsonObject>();
        getSaveMap(obj);
        return doc;
    }
    static const void getSaveMap(JsonObject &root)
    {
        root["controller"] = ClosedLoopControllerJsonRouter::getSaveMap();
    }
    static JsonRouter<SpinSequencerController> router;
};

class SpinSequencerService : public StatefulService<SpinSequencerController&>
{
public:
    SpinSequencerService(PsychicHttpServer *server,
                         ESP32SvelteKit *sveltekit,
                         SpinSequencerController &controller) :
                            _httpRouterEndpoint(_router.read, _router.update, this, server, "/rest/spin-seq", sveltekit->getSecurityManager()),
                            _fsPersistence(_router.readForSave, _router.update, this, sveltekit->getFS(), "/config/spin-seq.json"),
                            StatefulService(controller) {}
    void begin();
    void loop();

private:
    HttpRouterEndpoint<SpinSequencerController&> _httpRouterEndpoint;
    FSPersistence<SpinSequencerController&> _fsPersistence;
    SpinSequencerControllerJsonRouter _router;
};

#endif

