#ifndef StepperService_h
#define StepperService_h

#include <EventEndpoint.h>
#include <HttpRouterEndpoint.h>
#include <FSPersistence.h>
#include <StatelessService.h>

#include <tmcdriver.h>
#include <TmcDriverService.h>

class TMC5160ControllerJsonRouter
{
public:
    static bool route(JsonVariant content, TMC5160Controller &controller)
    {
        return router.route(content, controller);
    }
    static void read(TMC5160Controller &state, JsonObject &root) 
    {
        router.serialize(state, root);
    }
    static void readForSave(TMC5160Controller &state, JsonObject &root) 
    {
        getSaveMap(root);
        router.serialize(state, root);
        JsonDocument ref = getSaveMap();
        JsonSaveManager::filterFieldsRecursively(ref.as<JsonObject>(), root);
    }
    static StateUpdateResult update(JsonObject &root, TMC5160Controller &state)
    { 
        if (router.parse(root, state) && JsonSaveManager::needsToSave(root, getSaveMap())) return StateUpdateResult::CHANGED;
        else return StateUpdateResult::UNCHANGED;
    }
    static const void getSaveMap(JsonObject &root) 
    {
        root["config"]["enabled"] = true;
        root["config"]["invertDirection"] = true;
        root["config"]["maxSpeed"] = true;
        root["config"]["maxAccel"] = true;
        root["config"]["stepsPerRot"] = true;
        root["config"]["driverCurrent"] = true;
    }
    static const JsonDocument getSaveMap() 
    {
        JsonDocument doc;
        JsonObject obj = doc.to<JsonObject>();
        getSaveMap(obj);
        return doc;
    }
    static JsonRouter<TMC5160Controller> router;
    static JsonEventRouter<TMC5160Controller> controlRouter;
    static JsonEventRouter<TMC5160Controller> configRouter;
    static TMC5160StepperJsonRouter driverRouter;
};

class StepperService : public StatefulService<TMC5160Controller&>
{
public:
    StepperService(PsychicHttpServer *server,
                    EventSocket *socket,
                    FS *fs,
                    SecurityManager *securityManager,
                    TMC5160Controller &controller) :
                        _httpRouterEndpoint(_router.read, _router.update, this, server, "/rest/stepper", securityManager),
                        _fsPersistence(_router.readForSave, _router.update, this, fs, "/config/stepper.json"),
                        _eventEndpoint(_router.read, _router.update, this, socket, "stepper"),
                        StatefulService(controller)
                        {}
    void begin();

private:
    EventEndpoint<TMC5160Controller&> _eventEndpoint;
    HttpRouterEndpoint<TMC5160Controller&> _httpRouterEndpoint;
    FSPersistence<TMC5160Controller&> _fsPersistence;
    TMC5160ControllerJsonRouter _router;
};

#endif