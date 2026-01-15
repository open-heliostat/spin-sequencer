#ifndef SpinSequencerService_h
#define SpinSequencerService_h

#include <ESP32SvelteKit.h>
#include <EventEndpoint.h>
#include <FSPersistence.h>
#include <StatelessService.h>
#include <HttpRouterEndpoint.h>
#include <UdpRouterEndpoint.h>
#include <ClosedLoopControllerService.h>
#include <JseqService.h>

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
        // Serial.println(ref.as<String>());
        // ESP_LOGI("Spin Seq", "%s", ref.as<String>().c_str());
        ref = getSaveMap();
        JsonSaveManager::filterFieldsRecursively(ref.as<JsonObject>(), root);
    }
    static StateUpdateResult update(JsonObject &root, SpinSequencerController &state, const String &originId)
    {
        (void)originId; // origin unused for spin sequencer updates
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
        root["sequencer"] = JsonSeqJsonRouter::getSaveMap();
        root["remotes"] = RemoteJsonRouter::getSaveMap();
        root["welcome"] = true;
        root["timers"] = JsonTimerRouter::getSaveMap();
        root["io"]["startButtonPin"] = true;
        root["io"]["startButtonActiveLow"] = true;
        root["io"]["startButtonDebounceMs"] = true;
        root["io"]["statusLedPin"] = true;
        root["io"]["statusLedActiveHigh"] = true;
    }
    static JsonRouter<SpinSequencerController> router;
};

class SpinSequencerService : public StatefulService<SpinSequencerController&>
{
public:
    SpinSequencerService(PsychicHttpServer *server,
                         ESP32SvelteKit *sveltekit,
                         SpinSequencerController &controller) :
                            _udpRouterEndpoint(_router.read, _router.update, this, "/rest/spin-seq", JSON_ROUTER_UDP_PORT, [this](const String &msg) {
                                _state.appendUdpMessage(msg);
                            }),
                            _httpRouterEndpoint(_router.read, _router.update, this, server, "/rest/spin-seq", sveltekit->getSecurityManager()),
                            _fsPersistence(_router.readForSave, _router.update, this, sveltekit->getFS(), "/config/spin-seq.json"),
                            StatefulService(controller) {}
    void begin();
    void loop();

private:
    UdpRouterEndpoint<SpinSequencerController&> _udpRouterEndpoint;
    HttpRouterEndpoint<SpinSequencerController&> _httpRouterEndpoint;
    FSPersistence<SpinSequencerController&> _fsPersistence;
    SpinSequencerControllerJsonRouter _router;
};

#endif

