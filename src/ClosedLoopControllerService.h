#ifndef ClosedLoopControllerService_h
#define ClosedLoopControllerService_h

#include <EventEndpoint.h>
#include <FSPersistence.h>
#include <StatelessService.h>

#include <closedloopcontroller.h>

#define CL_CONTROLLER_STATE_EVENT "controller"
#define CL_CONTROLLER_SETTINGS_EVENT "controllersettings"
#define CL_SETTINGS_FILE "/config/controllerSettings.json"

class ClosedLoopControllerJsonRouter
{
public:
    static bool route(JsonVariant content, ClosedLoopController &controller)
    {
        return router.route(content, controller);
    }
    static void read(ClosedLoopController &state, JsonObject &root) 
    {
        router.route(root, state);
    }
    static StateUpdateResult update(JsonObject &root, ClosedLoopController &state)
    { 
        if (router.route(root, state)) return StateUpdateResult::CHANGED;
        else return StateUpdateResult::UNCHANGED;
    }
    static const JsonDocument getSaveMap() {
        JsonDocument doc;
        doc.to<JsonObject>();
        doc["calibration"]["enabled"] = true;
        doc["calibration"]["offsets"] = true;
        doc["limits"]["enabled"] = true;
        doc["limits"]["begin"] = true;
        doc["limits"]["end"] = true;
        return doc;
    }
    static JsonRouter<ClosedLoopController> router;
private:
    static JsonEventRouter<ClosedLoopController> calibrationRouter;
    static JsonEventRouter<ClosedLoopController> limitsRouter;
};


class ClosedLoopControllerState
{
public:
    double targetAngle;
    double curAngle;

    static void read(ClosedLoopControllerState &state, JsonObject &root) {
        root["targetAngle"] = state.targetAngle;
        root["curAngle"] = state.curAngle;
    }

    static StateUpdateResult update(JsonObject &root, ClosedLoopControllerState &state) {
        bool changed = false;
        if (root["targetAngle"].is<double>() & state.targetAngle != root["targetAngle"]) {
            state.targetAngle = root["targetAngle"];
            changed = true;
        }
        if (root["curAngle"].is<double>() & state.curAngle != root["curAngle"]) {
            state.curAngle = root["curAngle"];
            changed = true;
        }
        if (changed) return StateUpdateResult::CHANGED;
        else return StateUpdateResult::UNCHANGED;
    }

    static void readState(ClosedLoopController *controller, JsonObject &root) {
        root["targetAngle"] = controller->targetAngle;
        root["curAngle"] = (int)(controller->encoder.angle * 100 + 0.5) / 100.0;
    }
};

class ClosedLoopControllerStates
{
public:
    std::vector<ClosedLoopControllerState> controllers;
    static void read(ClosedLoopControllerStates &controllers, JsonObject &root)
    {
        JsonArray jsonArray = root["controllers"].to<JsonArray>();
        for (ClosedLoopControllerState controller : controllers.controllers) {
            JsonObject obj = jsonArray.add<JsonObject>();
            controller.read(controller, obj);
        }
    }
    static StateUpdateResult update(JsonObject &root, ClosedLoopControllerStates &controllers)
    {
        JsonArray jsonArray = root["controllers"].as<JsonArray>();
        bool hasChanged = false;
        for (int i = 0; i < min(jsonArray.size(), controllers.controllers.size()); i++) {
            JsonObject obj = jsonArray[i];
            if (controllers.controllers[i].update(obj, controllers.controllers[i]) == StateUpdateResult::CHANGED) hasChanged = true;
        }
        return hasChanged ? StateUpdateResult::CHANGED : StateUpdateResult::UNCHANGED;
    }
    static void readState(std::vector<ClosedLoopController*> &controllers, JsonObject &root) {
        JsonArray jsonArray = root["controllers"].to<JsonArray>();
        for (ClosedLoopController *controller : controllers) {
            JsonObject obj = jsonArray.add<JsonObject>();
            ClosedLoopControllerState::readState(controller, obj);
        }
    }
};

class ClosedLoopControllerStateService : public StatefulService<ClosedLoopControllerStates>
{
public:
    ClosedLoopControllerStateService( EventSocket *socket,
                                      std::vector<ClosedLoopController*>& controllers);
    void begin();
    void updateState();
    void loop();

private:
    EventEndpoint<ClosedLoopControllerStates> _eventEndpoint;
    std::vector<ClosedLoopController*>& _controllers;

    void onConfigUpdated(const String &originId);
};

class ClosedLoopControllerSettings
{
public:
    bool enabled;
    bool hasLimits;
    double tolerance;
    double limitA;
    double limitB;
    String name;

    static void read(ClosedLoopControllerSettings &state, JsonObject &root) {
        root["tolerance"] = state.tolerance;
        root["enabled"] = state.enabled;
        root["limitA"] = state.limitA;
        root["limitB"] = state.limitB;
        root["name"] = state.name;
    }
    static StateUpdateResult update(JsonObject &root, ClosedLoopControllerSettings &state) {
        state.tolerance = root["tolerance"] | 0.2;
        state.enabled = root["enabled"] | false;
        state.hasLimits = root["hasLimits"] | false;
        state.limitA = root["limitA"] | 0.;
        state.limitB = root["limitB"] | 360.;
        state.name = root["name"] | "Controller";
        return StateUpdateResult::CHANGED;
    }

    static void readState(ClosedLoopController *controller, JsonObject &root) {
        root["tolerance"] = controller->tolerance;
        root["enabled"] = controller->enabled;
        root["hasLimits"] = controller->hasLimits;
        root["limitA"] = controller->limitA;
        root["limitB"] = controller->limitB;
    }
};

class MultiClosedLoopControllerSettings
{
public:
    std::vector<ClosedLoopControllerSettings> settings;
    static void read(MultiClosedLoopControllerSettings &settings, JsonObject &root)
    {
        JsonArray jsonArray = root["controllers"].to<JsonArray>();
        for (ClosedLoopControllerSettings controller : settings.settings) {
            JsonObject obj = jsonArray.add<JsonObject>();
            controller.read(controller, obj);
        }
    }
    static StateUpdateResult update(JsonObject &root, MultiClosedLoopControllerSettings &settings)
    {
        JsonArray jsonArray = root["controllers"].as<JsonArray>();
        bool hasChanged = false;
        for (int i = 0; i < min(jsonArray.size(), settings.settings.size()); i++) {
            JsonObject obj = jsonArray[i];
            if (settings.settings[i].update(obj, settings.settings[i]) == StateUpdateResult::CHANGED) hasChanged = true;
        }
        return hasChanged ? StateUpdateResult::CHANGED : StateUpdateResult::UNCHANGED;
    }
};

class ClosedLoopControllerSettingsService : public StatefulService<MultiClosedLoopControllerSettings>
{
public:
    ClosedLoopControllerSettingsService(EventSocket *socket,
                                        FS *fs,
                                        std::vector<ClosedLoopController*>& controllers);
    void begin();
    void loop();

private:
    EventEndpoint<MultiClosedLoopControllerSettings> _eventEndpoint;
    FSPersistence<MultiClosedLoopControllerSettings> _fsPersistence;
    std::vector<ClosedLoopController*>& _controllers;
    ClosedLoopControllerStateService _closedLoopControllerStateService;

    void onConfigUpdated();
};

class ClosedLoopControllerService : public StatefulService<ClosedLoopController&>
{
public:
    ClosedLoopControllerService(EventSocket *socket,
                                FS *fs,
                                ClosedLoopController &controller) :
                                    _eventEndpoint(_router.read, _router.update, this, socket, "controllers"),
                                    _fsPersistence(_router.read, _router.update, this, fs, "/config/controllers.json"),
                                    StatefulService(controller)
                                    {}
    void begin();

private:
    EventEndpoint<ClosedLoopController&> _eventEndpoint;
    FSPersistence<ClosedLoopController&> _fsPersistence;
    ClosedLoopControllerJsonRouter _router;
};
#endif