#ifndef ClosedLoopControllerService_h
#define ClosedLoopControllerService_h

#include <EventEndpoint.h>

#include <closedloopcontroller.h>

#define CL_CONTROLLER_STATE_EVENT "closedloopcontroller"

class ClosedLoopControllerState
{
public:
    double angle;

    static void read(ClosedLoopControllerState &state, JsonObject &root) {
        root["angle"] = state.angle;
    }

    static StateUpdateResult update(JsonObject &root, ClosedLoopControllerState &state) {
        bool changed = false;
        if (root["angle"].is<double>() & state.angle != root["angle"]) {
            state.angle = root["angle"];
            changed = true;
        }
        if (changed) return StateUpdateResult::CHANGED;
        else return StateUpdateResult::UNCHANGED;
    }

    static void readState(ClosedLoopController *controller, JsonObject &root) {
        root["angle"] = controller->targetAngle;
    }
};

class ClosedLoopControllerStateService : public StatefulService<ClosedLoopControllerState>
{
public:
    ClosedLoopControllerStateService( EventSocket *socket,
                         ClosedLoopController *controller);
    void begin();
    void updateState();
    void loop();

private:
    EventEndpoint<ClosedLoopControllerState> _eventEndpoint;

    void onConfigUpdated();
    ClosedLoopController *_controller;
};
#endif