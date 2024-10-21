#ifndef StepperControlService_h
#define StepperControlService_h

#include <StepperSettingsService.h>

#include <EventEndpoint.h>
#include <FeaturesService.h>

#include <tmcdriver.h>

#define STEPPER_CONTROL_EVENT "steppercontrol"

class StepperControl
{
public:
    bool isEnabled;
    bool direction;
    float move;
    float newMove;
    float speed;
    float acceleration;
    uint32_t status;
    uint8_t version;
    // StepperSettings settings;

    static void read(StepperControl &settings, JsonObject &root)
    {
        root["isEnabled"] = settings.isEnabled;
        root["direction"] = settings.direction;
        root["move"] = settings.move;
        root["speed"] = settings.speed;
        root["acceleration"] = settings.acceleration;
        root["status"] = settings.status;
        root["version"] = settings.version;
        // Serial.print("Read : ");
        // Serial.println(settings.status);
    }

    static StateUpdateResult update(JsonObject &root, StepperControl &settings)
    {
        bool hasChanged = false;
        if (root["isEnabled"].is<bool>() && settings.isEnabled != root["isEnabled"]) {
            settings.isEnabled = root["isEnabled"];
            hasChanged = true;
        }
        if (root["direction"].is<bool>() && settings.direction != root["direction"]) {
            settings.direction = root["direction"];
            hasChanged = true;
        }
        if (root["move"].is<float>() && abs(settings.move - float(root["move"])) > 0.01) {
            settings.newMove = settings.move - float(root["move"]);
            settings.move = root["move"];
            hasChanged = true;
        }
        if (root["speed"].is<float>() && abs(settings.speed - float(root["speed"])) > 0.01) {
            settings.speed = root["speed"];
            hasChanged = true;
        }
        if (root["acceleration"].is<float>() &&  abs(settings.acceleration - float(root["acceleration"])) > 0.01) {
            settings.acceleration = root["acceleration"];
            hasChanged = true;
        }
        if (root["status"].is<uint32_t>() && settings.status != root["status"]) {
            settings.status = root["status"];
            hasChanged = true;
        }
        if (root["version"].is<uint8_t>() && settings.version != root["version"]) {
            settings.version = root["version"];
            hasChanged = true;
        }
        // Serial.print("Update : ");
        // Serial.println(float(root["speed"]));
        if (hasChanged) return StateUpdateResult::CHANGED;
        else return StateUpdateResult::UNCHANGED;
    }

    static void readState(TMC5160Controller *stepper, StepperSettingsService *stepperSettingsService, JsonObject &root) {
        root["isEnabled"] = stepper->enabled;
        if (abs(stepper->getSpeed()) > 0.001) root["direction"] = stepper->getSpeed() >= 0;
        // root["move"] = stepper->move();
        root["speed"] = abs(stepper->getSpeed());
        // Serial.print("Read State : ");
        // Serial.println(float(root["speed"]));
        root["acceleration"] = stepper->getAcceleration();
        root["status"] = stepper->getStatus();
        root["version"] = stepper->driver.version();
        // Serial.println(stepper->getStatus());
    }
};

class StepperControlService : public StatefulService<StepperControl>
{
public:
    StepperControlService(EventSocket *socket,
                          StepperSettingsService *stepperSettingsService,
                          TMC5160Controller *stepper,
                          FeaturesService *featuresService);
    void begin();
    void loop();

private:
    EventEndpoint<StepperControl> _eventEndpoint;
    StepperSettingsService *_stepperSettingsService;
    TMC5160Controller *_stepper;
    FeaturesService *_featuresService;

    unsigned long lastUpdate = 0;

    void registerConfig();
    void configureArtNet();
    void onConfigUpdated(const String &originId);
    void updateState();
};

#endif
