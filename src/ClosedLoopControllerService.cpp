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

JsonRouter<ClosedLoopController> ClosedLoopControllerJsonRouter::router = JsonRouter<ClosedLoopController>(
{
    {"target", [](JsonVariant content, ClosedLoopController &controller) {
        if (content.is<double>()) {
            controller.setAngle(content.as<double>());
            return true;
        }
        else return false;
    }},
    {"tolerance", [](JsonVariant content, ClosedLoopController &controller) {
        if (content.is<double>()) {
            controller.tolerance = content.as<double>();
            return true;
        }
        else return false;
    }},
    {"calibration", [](JsonVariant content, ClosedLoopController &controller) {
        return calibrationRouter.parse(content, controller);
    }},
    {"limits", [](JsonVariant content, ClosedLoopController &controller) {
        return limitsRouter.parse(content, controller);
    }}
},
{
    {"position", [](ClosedLoopController &controller, const JsonVariant target) {
        target.set(controller.getAngle());
    }},
    {"target", [](ClosedLoopController &controller, const JsonVariant target) {
        target.set(controller.targetAngle);
    }},
    {"tolerance", [](ClosedLoopController &controller, const JsonVariant target) {
        target.set(controller.tolerance);
    }},
    {"limits", [](ClosedLoopController &controller, const JsonVariant target) {
        target["enabled"] = controller.hasLimits;
        target["begin"] = controller.limitA;
        target["end"] = controller.limitB;
    }},
    {"calibration", [](ClosedLoopController &controller, const JsonVariant target) {
        target["running"].set(controller.calibrationRunning);
        target["enabled"].set(controller.hasCalibration);
        target["steps"].set(controller.calibrationSteps);
        target["speed"].set(controller.calibrationSpeed);
        target["decay"].set(controller.calibrationDecay);
        if (target["offsets"].is<JsonVariant>()) {
            auto array = target["offsets"].to<JsonArray>();
            copyArray(controller.calibrationOffsets, array);
        }
    }}
});

JsonEventRouter<ClosedLoopController> ClosedLoopControllerJsonRouter::calibrationRouter = JsonEventRouter<ClosedLoopController>({
    {"start", [](JsonVariant content, ClosedLoopController &controller) {
        controller.startCalibration();
        return true;
    }},
    {"stop", [](JsonVariant content, ClosedLoopController &controller) {
        controller.stopCalibration();
        return true;
    }},
    {"reset", [](JsonVariant content, ClosedLoopController &controller) {
        controller.resetCalibration();
        return true;
    }},
    {"speed", [](JsonVariant content, ClosedLoopController &controller) {
        if (content.is<int>()) {
            controller.setCalibrationSpeed(content.as<int>());
            return true;
        }
        else return false;
    }},
    {"decay", [](JsonVariant content, ClosedLoopController &controller) {
        if (content.is<double>()) {
            controller.calibrationDecay = content.as<double>();
            return true;
        }
        else return false;
    }},
    {"enabled", [](JsonVariant content, ClosedLoopController &controller) {
        if (content.is<bool>()) {
            controller.hasCalibration = content.as<bool>();
            return true;
        }
        else return false;
    }},
    {"running", [](JsonVariant content, ClosedLoopController &controller) {
        if (content.is<bool>()) {
            if (content.as<bool>()) controller.startCalibration();
            else controller.stopCalibration();
            return true;
        }
        else return false;
    }},
    {"offsets", [](JsonVariant content, ClosedLoopController &controller) {
        if (content.is<JsonArray>()) {
            auto array = content.as<JsonArray>();
            if (array.size() == controller.calibrationSteps) {
                copyArray(array, controller.calibrationOffsets);
                return true;
            }
        }
        return false;
    }},
});

JsonEventRouter<ClosedLoopController> ClosedLoopControllerJsonRouter::limitsRouter = JsonEventRouter<ClosedLoopController>({
    {"enabled", [](JsonVariant content, ClosedLoopController &controller) {
        if (content.is<bool>()) {
            controller.hasLimits = content.as<bool>();
            return true;
        }
        else return false;
    }},
    {"begin", [](JsonVariant content, ClosedLoopController &controller) {
        if (content.is<double>()) {
            controller.limitA = content.as<double>();
            return true;
        }
        else return false;
    }},
    {"end", [](JsonVariant content, ClosedLoopController &controller) {
        if (content.is<double>()) {
            controller.limitB = content.as<double>();
            return true;
        }
        else return false;
    }}
});

void ClosedLoopControllerService::begin() {
    _httpRouterEndpoint.begin();
    _fsPersistence.readFromFS();
}