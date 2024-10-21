#include <StepperControlService.h>

StepperControlService::StepperControlService(EventSocket *socket,
                                             StepperSettingsService *stepperSettingsService,
                                             TMC5160Controller *stepper,
                                             FeaturesService *featuresService) :
                                                    _eventEndpoint(StepperControl::read,
                                                                    StepperControl::update,
                                                                    this,
                                                                    socket,
                                                                    STEPPER_CONTROL_EVENT),
                                                    _stepperSettingsService(stepperSettingsService),
                                                    _stepper(stepper),
                                                    _featuresService(featuresService)
{
    // configure update handler for when the stepper settings change
    // _stepperSettingsService->addUpdateHandler([&](const String &originId)
    //                                             { onConfigUpdated(originId); },
    //                                             false);

    addUpdateHandler([&](const String &originId)
                     { onConfigUpdated(originId); },
                     false);
    
    _featuresService->addFeature("stepper", true);
}

void StepperControlService::begin()
{
    _eventEndpoint.begin();
    updateState();
    onConfigUpdated("begin");
}

void StepperControlService::loop() {
    unsigned long now = millis();
    if (now - lastUpdate > 500) {
        updateState();
        lastUpdate = now;
    }
}

void StepperControlService::onConfigUpdated(const String &originId)
{
    // _state.settings = _stepperSettingsService->getState();
    if (originId != "driver") {
        Serial.println(originId);
        if (_state.isEnabled & !_stepper->enabled) _stepper->enable();
        else if(!_state.isEnabled & _stepper->enabled) _stepper->disable();
        _stepper->setAcceleration(_state.acceleration);
        _stepper->setSpeed(_state.direction ? _state.speed : -_state.speed);
        if (abs(_state.newMove)>0.001) {
            _stepper->move(_state.newMove*360.);
            _state.newMove = 0;
        }
    }
}

void StepperControlService::updateState() {
    JsonDocument json;
    JsonObject jsonObject = json.to<JsonObject>();
    _state.readState(_stepper, _stepperSettingsService, jsonObject);
    update(jsonObject, _state.update, "driver");
}