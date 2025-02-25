#ifndef TmcDriverService_h
#define TmcDriverService_h

#include <EventEndpoint.h>
#include <HttpRouterEndpoint.h>
#include <FSPersistence.h>
#include <StatelessService.h>

#include <tmcdriver.h>

class TMC5160StepperJsonRouter
{
public:
    static bool route(JsonVariant content, TMC5160Stepper &controller)
    {
        return router.route(content, controller);
    }
    static void read(TMC5160Stepper &state, JsonObject &root) 
    {
        router.serialize(state, root);
    }
    static void readForSave(TMC5160Stepper &state, JsonObject &root) 
    {
        getSaveMap(root);
        router.serialize(state, root);
        JsonDocument ref = getSaveMap();
        JsonSaveManager::filterFieldsRecursively(ref.as<JsonObject>(), root);
    }
    static StateUpdateResult update(JsonObject &root, TMC5160Stepper &state)
    { 
        if (router.parse(root, state) && JsonSaveManager::needsToSave(root, getSaveMap())) return StateUpdateResult::CHANGED;
        else return StateUpdateResult::UNCHANGED;
    }
    static const void getSaveMap(JsonObject &root) 
    {
    }
    static const JsonDocument getSaveMap() 
    {
        JsonDocument doc;
        JsonObject obj = doc.to<JsonObject>();
        getSaveMap(obj);
        return doc;
    }
    static JsonRouter<TMC5160Stepper> router;
private:
    static JsonEventRouter<TMC5160Stepper> chopConfigRouter;
    static JsonEventRouter<TMC5160Stepper> driverConfigRouter;
    static JsonEventRouter<TMC5160Stepper> stallConfigRouter;
    static JsonEventRouter<TMC5160Stepper> pwmConfigRouter;
};

class TmcDriverService : public StatefulService<TMC5160Stepper&>
{
public:
    TmcDriverService(PsychicHttpServer *server,
                    EventSocket *socket,
                    FS *fs,
                    SecurityManager *securityManager,
                    TMC5160Stepper &controller) :
                        _httpRouterEndpoint(_router.read, _router.update, this, server, "/rest/driver", securityManager),
                        _fsPersistence(_router.readForSave, _router.update, this, fs, "/config/driver.json"),
                        _eventEndpoint(_router.read, _router.update, this, socket, "driver"),
                        StatefulService(controller)
                        {}
    void begin();

private:
    EventEndpoint<TMC5160Stepper&> _eventEndpoint;
    HttpRouterEndpoint<TMC5160Stepper&> _httpRouterEndpoint;
    FSPersistence<TMC5160Stepper&> _fsPersistence;
    TMC5160StepperJsonRouter _router;
};

#endif