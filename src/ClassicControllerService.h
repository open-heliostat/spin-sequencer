#ifndef ClassicControllerService_h
#define ClassicControllerService_h

#include <EventEndpoint.h>
#include <FSPersistence.h>
#include <StatelessService.h>
#include <HttpRouterEndpoint.h>
#include <StepperService.h>
#include "controller.h"

#define CLASSIC_CONTROLLER_STATE_EVENT "classiccontroller"
#define CLASSIC_SETTINGS_FILE "/config/classiccontroller.json"

class ClassicControllerJsonRouter
{
public:
    static bool route(JsonVariant content, MotorController &controller)
    {
        return router.route(content, controller);
    }

    static void read(MotorController &state, JsonObject &root) 
    {
        router.serialize(state, root);
    }

    static void readForSave(MotorController &state, JsonObject &root) 
    {
        getSaveMap(root);
        router.serialize(state, root);
        JsonDocument ref = getSaveMap();
        JsonSaveManager::filterFieldsRecursively(ref.as<JsonObject>(), root);
    }

    static StateUpdateResult update(JsonObject &root, MotorController &state)
    { 
        if (router.parse(root, state) && JsonSaveManager::needsToSave(root, getSaveMap())) 
            return StateUpdateResult::CHANGED;
        return StateUpdateResult::UNCHANGED;
    }

    static const void getSaveMap(JsonObject &root) 
    {
        root["pingPong"] = true;
        root["stopPosition"] = true;
        root["dist"] = true;
        root["speed"] = true;
        root["offset"] = true;
    }

    static const JsonDocument getSaveMap() 
    {
        JsonDocument doc;
        JsonObject obj = doc.to<JsonObject>();
        getSaveMap(obj);
        return doc;
    }

    static JsonRouter<MotorController> router;
    static JsonEventRouter<MotorController> controlRouter;
};

class ClassicControllerState
{
public:
    double targetPosition;
    double currentPosition;
    bool isMoving;
    bool pingPong;

    static void read(ClassicControllerState &state, JsonObject &root) {
        root["targetPosition"] = state.targetPosition;
        root["currentPosition"] = state.currentPosition;
        root["isMoving"] = state.isMoving;
        root["pingPong"] = state.pingPong;
    }

    static StateUpdateResult update(JsonObject &root, ClassicControllerState &state) {
        bool changed = false;
        
        if (root["targetPosition"].is<double>() && state.targetPosition != root["targetPosition"]) {
            state.targetPosition = root["targetPosition"];
            changed = true;
        }
        if (root["pingPong"].is<bool>() && state.pingPong != root["pingPong"]) {
            state.pingPong = root["pingPong"];
            changed = true;
        }
        
        return changed ? StateUpdateResult::CHANGED : StateUpdateResult::UNCHANGED;
    }

    static void readState(MotorController &controller, JsonObject &root) {
        root["currentPosition"] = controller.getPosition();
        root["isMoving"] = controller.isMoving;
        root["pingPong"] = controller.pingPong;
        root["stopPosition"] = controller.stopPosition;
        root["dist"] = controller.dist;
        root["speed"] = controller.speed;
    }
};

class ClassicControllerService : public StatefulService<MotorController&>
{
public:
    ClassicControllerService(PsychicHttpServer *server,
                           EventSocket *socket,
                           FS *fs,
                           SecurityManager *securityManager,
                           MotorController &controller) :
        _httpRouterEndpoint(_router.read, _router.update, this, server, "/rest/classic", securityManager),
        _fsPersistence(_router.readForSave, _router.update, this, fs, CLASSIC_SETTINGS_FILE),
        _eventEndpoint(_router.read, _router.update, this, socket, CLASSIC_CONTROLLER_STATE_EVENT),
        StatefulService(controller)
    {}

    void begin() {
        _httpRouterEndpoint.begin();
        _eventEndpoint.begin();
        _fsPersistence.readFromFS();
    }

    void loop() {
        _state.tick();
        updateState();
    }

private:
    EventEndpoint<MotorController&> _eventEndpoint;
    HttpRouterEndpoint<MotorController&> _httpRouterEndpoint;
    FSPersistence<MotorController&> _fsPersistence;
    ClassicControllerJsonRouter _router;

    void updateState() {
        JsonDocument json;
        JsonObject jsonObject = json.to<JsonObject>();
        ClassicControllerState::readState(_state, jsonObject);
        update(jsonObject, _router.update, "stateUpdate");
    }
};

#endif