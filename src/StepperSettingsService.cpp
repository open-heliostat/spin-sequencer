#include <StepperSettingsService.h>

StepperSettingsService::StepperSettingsService(PsychicHttpServer *server,
                                                FS *fs,
                                                SecurityManager *securityManager,
                                                std::vector<TMC5160Controller*>& steppers) :                  
                                                                            _httpEndpoint(MultiStepperSettings::read,
                                                                                            MultiStepperSettings::update,
                                                                                            this,
                                                                                            server,
                                                                                            STEPPER_SETTINGS_ENDPOINT_PATH,
                                                                                            securityManager,
                                                                                            AuthenticationPredicates::IS_AUTHENTICATED),
                                                                            _fsPersistence(MultiStepperSettings::read, MultiStepperSettings::update, this, fs, STEPPER_SETTINGS_FILE),
                                                                            _steppers(steppers)
{

    for (TMC5160Controller *s : steppers) {
        StepperSettings settings = StepperSettings();
        _state.settings.push_back(settings);
    }

    // configure settings service update handler to update LED state
    addUpdateHandler([&](const String &originId)
                     { onConfigUpdated(); },
                     false);
}

void StepperSettingsService::begin()
{
    _httpEndpoint.begin();
    _fsPersistence.readFromFS();
    // _stepper->init();
    onConfigUpdated();
    for (int i = 0; i < _steppers.size(); i++) {
        if (_state.settings[i].enableOnStart) _steppers[i]->enable();
        else _steppers[i]->disable();
    }
    // updateState();
}

// int32_t StepperSettingsService::getMaxSpeed() {
//     return _state.maxSpeed;
// }

// int32_t StepperSettingsService::getMaxAccel() {
//     return _state.maxAcceleration;
// }

MultiStepperSettings StepperSettingsService::getState() {
    return _state;
}

void StepperSettingsService::onConfigUpdated()
{
    for (int i = 0; i < _steppers.size(); i++) {
        _steppers[i]->maxSpeed = _state.settings[i].maxSpeed;
        _steppers[i]->maxAccel = _state.settings[i].maxAcceleration;
        _steppers[i]->driver.rms_current(_state.settings[i].current);
        _steppers[i]->driver.shaft(_state.settings[i].invertDirection);
    }
}

// void StepperSettingsService::updateState() {
    
//     DynamicJsonDocument json(1024);
//     JsonObject jsonObject = json.to<JsonObject>();
//     _state.readState(_stepper, jsonObject);
//     update(jsonObject, _state.update, "timer");
// }