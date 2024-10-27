#ifndef ClosedLoopControllerService_h
#define ClosedLoopControllerService_h

#include <EventEndpoint.h>

#include <closedloopcontroller.h>

#define CL_CONTROLLER_STATE_EVENT "closedloopcontroller"

class ClosedLoopControllerState
{
public:
    bool enabled;
    double targetAngle;
    double curAngle;
    double tolerance;

    static void read(ClosedLoopControllerState &state, JsonObject &root) {
        root["targetAngle"] = state.targetAngle;
        root["curAngle"] = state.curAngle;
        root["tolerance"] = state.tolerance;
        root["enabled"] = state.enabled;
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
        if (root["tolerance"].is<double>() & state.tolerance != root["tolerance"]) {
            state.tolerance = root["tolerance"];
            changed = true;
        }
        if (root["enabled"].is<bool>() & state.enabled != root["enabled"]) {
            state.enabled = root["enabled"];
            changed = true;
        }
        if (changed) return StateUpdateResult::CHANGED;
        else return StateUpdateResult::UNCHANGED;
    }

    static void readState(ClosedLoopController *controller, JsonObject &root) {
        root["targetAngle"] = controller->targetAngle;
        root["curAngle"] = controller->encoder.angle;
        root["tolerance"] = controller->tolerance;
        root["enabled"] = controller->enabled;
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
#endif