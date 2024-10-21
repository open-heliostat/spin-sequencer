#ifndef StepperSettingsService_h
#define StepperSettingsService_h

#include <HttpEndpoint.h>
#include <FSPersistence.h>
#include <tmcdriver.h>
#include <vector>

#define STEPPER_SETTINGS_FILE "/config/stepperSettings.json"
#define STEPPER_SETTINGS_ENDPOINT_PATH "/rest/stepperSettings"

class StepperSettings
{
public:
    bool enableOnStart;
    bool invertDirection;
    int32_t maxSpeed;
    uint32_t maxAcceleration;
    uint16_t current;

    static void read(StepperSettings &settings, JsonObject &root)
    {
        root["enableOnStart"] = settings.enableOnStart;
        root["invertDirection"] = settings.invertDirection;
        root["maxSpeed"] = settings.maxSpeed;
        root["maxAcceleration"] = settings.maxAcceleration;
        root["current"] = settings.current;
    }

    static StateUpdateResult update(JsonObject &root, StepperSettings &settings)
    {   
        settings.enableOnStart = root["enableOnStart"];
        settings.invertDirection = root["invertDirection"];
        settings.maxSpeed = root["maxSpeed"];
        settings.maxAcceleration = root["maxAcceleration"];
        settings.current = root["current"];
        return StateUpdateResult::CHANGED;
    }

    // static void readState(TMC5160Controller *stepper, JsonObject &root) {
    //     root["enableOnStart"] = stepper->enabled;
    //     root["invertDirection"] = stepper->invertDirection;
    //     // root["move"] = stepper->move();
    //     root["maxSpeed"] = stepper->getSpeed();
    //     root["maxAcceleration"] = stepper->getAcceleration();
    //     root["current"] = stepper->driver.rms_current();
    // }
};

class MultiStepperSettings
{
public:
    std::vector<StepperSettings> settings;
    static void read(MultiStepperSettings &settings, JsonObject &root)
    {
        JsonArray jsonArray = root["steppers"].to<JsonArray>();
        for (StepperSettings stepper : settings.settings) {
            JsonObject obj = jsonArray.add<JsonObject>();
            stepper.read(stepper, obj);
        }
    }
    static StateUpdateResult update(JsonObject &root, MultiStepperSettings &settings)
    {
        JsonArray jsonArray = root["steppers"].as<JsonArray>();
        bool hasChanged = false;
        for (int i = 0; i < min(jsonArray.size(), settings.settings.size()); i++) {
            JsonObject obj = jsonArray[i];
            if (settings.settings[i].update(obj, settings.settings[i]) == StateUpdateResult::CHANGED) hasChanged = true;
        }
        return hasChanged ? StateUpdateResult::CHANGED : StateUpdateResult::UNCHANGED;
    }
};

class StepperSettingsService : public StatefulService<MultiStepperSettings>
{
public:
    StepperSettingsService(PsychicHttpServer *server, 
                            FS *fs,
                            SecurityManager *securityManager,
                            std::vector<TMC5160Controller*>& steppers);
    void begin();
    int32_t getMaxSpeed();
    int32_t getMaxAccel();
    MultiStepperSettings getState();

private:
    HttpEndpoint<MultiStepperSettings> _httpEndpoint;
    FSPersistence<MultiStepperSettings> _fsPersistence;
    std::vector<TMC5160Controller*>& _steppers;

    void onConfigUpdated();
};

#endif
