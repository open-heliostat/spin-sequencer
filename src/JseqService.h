#ifndef JseqService_h
#define JseqService_h

#include <EventEndpoint.h>
#include <HttpRouterEndpoint.h>
#include <FSPersistence.h>
#include <StatelessService.h>
#include <ClassicControllerService.h>
#include "jseq.h"

class JsonSeqJsonRouter
{
public:
    static bool route(JsonVariant content, JsonSeq &sequencer)
    {
        return router.route(content, sequencer);
    }

    static void read(JsonSeq &state, JsonObject &root) 
    {
        router.serialize(state, root);
    }

    static void readForSave(JsonSeq &state, JsonObject &root) 
    {
        getSaveMap(root);
        router.serialize(state, root);
        JsonDocument ref = getSaveMap();
        JsonSaveManager::filterFieldsRecursively(ref.as<JsonObject>(), root);
    }

    static StateUpdateResult update(JsonObject &root, JsonSeq &state)
    { 
        if (router.parse(root, state) && JsonSaveManager::needsToSave(root, getSaveMap())) 
            return StateUpdateResult::CHANGED;
        else 
            return StateUpdateResult::UNCHANGED;
    }

    static const void getSaveMap(JsonObject &root) 
    {
        root["config"]["commands"] = true;
        root["config"]["selectedCommand"] = true;
        root["config"]["isRunning"] = true;
        root["controller"] = true;
    }

    static const JsonDocument getSaveMap() 
    {
        JsonDocument doc;
        JsonObject obj = doc.to<JsonObject>();
        getSaveMap(obj);
        return doc;
    }

    static JsonRouter<JsonSeq> router;
    static JsonEventRouter<JsonSeq> controlRouter;
    static JsonEventRouter<JsonSeq> configRouter;
};

class JseqService : public StatefulService<JsonSeq&>
{
public:
    JseqService(PsychicHttpServer *server,
                EventSocket *socket,
                FS *fs,
                SecurityManager *securityManager,
                JsonSeq &sequencer) :
                    _httpRouterEndpoint(_router.read, _router.update, this, server, "/rest/sequencer", securityManager),
                    _fsPersistence(_router.readForSave, _router.update, this, fs, "/config/sequencer.json"),
                    _eventEndpoint(_router.read, _router.update, this, socket, "sequencer"),
                    StatefulService(sequencer)
                    {}
    void begin() {
        _httpRouterEndpoint.begin();
        _eventEndpoint.begin();
        _fsPersistence.readFromFS();
    }

private:
    EventEndpoint<JsonSeq&> _eventEndpoint;
    HttpRouterEndpoint<JsonSeq&> _httpRouterEndpoint;
    FSPersistence<JsonSeq&> _fsPersistence;
    JsonSeqJsonRouter _router;
};

#endif