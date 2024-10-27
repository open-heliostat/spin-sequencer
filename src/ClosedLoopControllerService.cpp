#include <ClosedLoopControllerService.h>

ClosedLoopControllerStateService::ClosedLoopControllerStateService(EventSocket *socket,
                                                                   std::vector<ClosedLoopController*>& controllers) :
                                                                            _eventEndpoint(ClosedLoopControllerStates::read,
                                                                                            ClosedLoopControllerStates::update,
                                                                                            this,
                                                                                            socket,
                                                                                            CL_CONTROLLER_STATE_EVENT),
                                                                            _controllers(controllers)
{
}

void ClosedLoopControllerStateService::begin()
{
    for (ClosedLoopController *c : _controllers) {
        ClosedLoopControllerState controller = ClosedLoopControllerState();
        _state.controllers.push_back(controller);
    }
    _eventEndpoint.begin();
    updateState();
    onConfigUpdated("begin");
}

void ClosedLoopControllerStateService::loop() {
    bool changed = false;
    for (int i = 0; i < _controllers.size(); i++) {
        if (_controllers[i]->encoder.update()) changed = true;
    }
    if (changed) updateState();
}

void ClosedLoopControllerStateService::onConfigUpdated(const String &originId)
{
    if (originId != "driver") {
        for (int i = 0; i < _controllers.size(); i++) {
            _controllers[i]->enabled = _state.controllers[i].enabled;
            _controllers[i]->targetAngle = _state.controllers[i].targetAngle;
            _controllers[i]->tolerance = _state.controllers[i].tolerance;
        }
    }
}

void ClosedLoopControllerStateService::updateState() {
    JsonDocument json;
    JsonObject jsonObject = json.to<JsonObject>();
    _state.readState(_controllers, jsonObject);
    update(jsonObject, _state.update, "controllers");
}