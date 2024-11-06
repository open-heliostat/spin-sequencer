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
    for (ClosedLoopController *c : _controllers) {
        ClosedLoopControllerState controller = ClosedLoopControllerState();
        _state.controllers.push_back(controller);
    }
    addUpdateHandler([&](const String &originId)
                     { onConfigUpdated(originId); },
                     false);
}

void ClosedLoopControllerStateService::begin()
{
    _eventEndpoint.begin();
    for (auto controller : _controllers) if (controller->enabled) controller->setAngle(controller->getAngle());
    updateState();
    onConfigUpdated("begin");
}

void ClosedLoopControllerStateService::loop() {
    bool changed = false;
    for (int i = 0; i < _controllers.size(); i++) {
        if (_controllers[i]->encoder.update()) changed = true;
        _controllers[i]->run();
    }
    // if (changed) updateState();
    updateState();
}

void ClosedLoopControllerStateService::onConfigUpdated(const String &originId)
{
    if (originId != "stateUpdate") {
        for (int i = 0; i < _controllers.size(); i++) {
            auto controller = _controllers[i];
            auto state = _state.controllers[i];
            controller->targetAngle = state.targetAngle;
        }
    }
}

void ClosedLoopControllerStateService::updateState() {
    JsonDocument json;
    JsonObject jsonObject = json.to<JsonObject>();
    _state.readState(_controllers, jsonObject);
    update(jsonObject, _state.update, "stateUpdate");
}

ClosedLoopControllerSettingsService::ClosedLoopControllerSettingsService(EventSocket *socket,
                                                                         FS *fs,
                                                                         std::vector<ClosedLoopController*>& controllers) :                  
                                                                                _eventEndpoint(MultiClosedLoopControllerSettings::read,
                                                                                                MultiClosedLoopControllerSettings::update,
                                                                                                this,
                                                                                                socket,
                                                                                                CL_CONTROLLER_SETTINGS_EVENT),
                                                                                _fsPersistence(MultiClosedLoopControllerSettings::read, MultiClosedLoopControllerSettings::update, this, fs, CL_SETTINGS_FILE),
                                                                                _controllers(controllers),
                                                                                _closedLoopControllerStateService(socket, controllers)
{

    for (ClosedLoopController *s : controllers) {
        ClosedLoopControllerSettings settings = ClosedLoopControllerSettings();
        _state.settings.push_back(settings);
    }

    // configure settings service update handler to update LED state
    addUpdateHandler([&](const String &originId)
                     { onConfigUpdated(); },
                     false);
}

void ClosedLoopControllerSettingsService::begin()
{
    _eventEndpoint.begin();
    _fsPersistence.readFromFS();
    onConfigUpdated();
    _closedLoopControllerStateService.begin();
}

void ClosedLoopControllerSettingsService::loop() {
    _closedLoopControllerStateService.loop();
}


void ClosedLoopControllerSettingsService::onConfigUpdated()
{
    for (int i = 0; i < _controllers.size(); i++) {
        auto controller = _controllers[i];
        auto settings = _state.settings[i];
        controller->enabled = settings.enabled;
        controller->hasLimits = settings.hasLimits;
        controller->tolerance = settings.tolerance;
        controller->limitA = settings.limitA;
        controller->limitB = settings.limitB;
    }
}