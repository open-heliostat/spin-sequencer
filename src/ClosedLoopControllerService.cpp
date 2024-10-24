#include <ClosedLoopControllerService.h>

ClosedLoopControllerStateService::ClosedLoopControllerStateService(EventSocket *socket,
                                ClosedLoopController *controller) :
                                                    _eventEndpoint(ClosedLoopControllerState::read,
                                                                    ClosedLoopControllerState::update,
                                                                    this,
                                                                    socket,
                                                                    CL_CONTROLLER_STATE_EVENT),
                                                    _controller(controller)
{
}

void ClosedLoopControllerStateService::begin()
{
    _eventEndpoint.begin();
    updateState();
}

void ClosedLoopControllerStateService::loop() {
    if (_controller->encoder.update()) updateState();
}


void ClosedLoopControllerStateService::updateState() {
    JsonDocument json;
    JsonObject jsonObject = json.to<JsonObject>();
    _state.readState(_controller, jsonObject);
    update(jsonObject, _state.update, "controller");
}