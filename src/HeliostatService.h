#ifndef HeliostatService_h
#define HeliostatService_h

#include <EventEndpoint.h>
#include <FSPersistence.h>

#include <heliostat.h>

class HeliostatControllerState
{
public:
    double azimuth;
    double elevation;
    
    static void read(HeliostatControllerState &state, JsonObject &root) {
        root["azimuth"] = state.azimuth;
        root["elevation"] = state.elevation;
    }

    static void readState(HeliostatController &controller, JsonObject &root) {
        root["azimuth"] = controller.azimuthController.getAngle();
        root["elevation"] = controller.elevationController.getAngle();
    }

    static StateUpdateResult update(JsonObject &root, HeliostatControllerState &state) {
        state.azimuth = root["azimuth"] | 0.;
        state.elevation = root["elevation"] | 45.;
        return StateUpdateResult::CHANGED;
    }

    static void updateController(HeliostatControllerState &state, HeliostatController &controller) {
        controller.azimuthController.setAngle(state.azimuth);
        controller.elevationController.setAngle(state.elevation);
    }
};

class HeliostatControllerStateService : public StatefulService<HeliostatControllerState>
{
public:
    HeliostatControllerStateService(EventSocket *socket, FS *fs, HeliostatController &controller) :
        _eventEndpoint(HeliostatControllerState::read, HeliostatControllerState::update, this, socket, "heliostat-control"),
        _fsPersistence(HeliostatControllerState::read, HeliostatControllerState::update, this, fs, "/config/heliostat-control.json"),
        controller(controller) 
    {
        addUpdateHandler([&](const String &originId) { updateController(originId); }, false);
    }
    void begin() 
    {
        _eventEndpoint.begin();
        _fsPersistence.readFromFS();
        updateController("begin");
    }
    void updateState()
    {
        JsonDocument json;
        JsonObject jsonObject = json.to<JsonObject>();
        _state.readState(controller, jsonObject);
        update(jsonObject, _state.update, "stateUpdate");
    }

private:
    EventEndpoint<HeliostatControllerState> _eventEndpoint;
    FSPersistence<HeliostatControllerState> _fsPersistence;
    HeliostatController &controller;

    void updateController(const String &originId) 
    {
        if (originId != "stateUpdate") {
            HeliostatControllerState::updateController(_state, controller);
        }
    }
};

class HeliostatControllerSettings
{

};

class HeliostatControllerSettingsService : public StatefulService<HeliostatControllerSettings>
{

};

class HeliostatDiagnostic
{

};

class HeliostatStatelessService
{
public:
    HeliostatStatelessService(EventSocket *socket, HeliostatController &controller) :
        socket(socket),
        controller(controller) {}
    void begin() 
    {
        socket->registerEvent(eventName);
        socket->onEvent(eventName, [&](JsonObject &root, int originID) { onEvent(root, originID); });
        ESP_LOGI("Heliostat Service", "Registered Json Event : %s", eventName);
    }
    void onEvent(JsonObject &root, int originId) 
    {
        String json;
        serializeJson(root, json);
        ESP_LOGI("Heliostat Service", "Received Json Event : %s", json.c_str());
        for (auto const& e : eventMap) {
            if (root[e.first].is<JsonVariant>()) e.second(root[e.first]);
        }
    }
private:
    HeliostatController &controller;
    EventSocket *socket;
    const char* eventName = "heliostat-service";
    const std::map<String, std::function<void (JsonVariant content)>> eventMap = {
        {"hello", [&](JsonVariant content) {
            ESP_LOGI("Heliostat Service", "Received Json Event : %s", content.as<String>().c_str());
        }}
    };
};

class HeliostatService
{
public:
    HeliostatService(EventSocket *socket, FS *fs, HeliostatController &controller) :
        stateService(socket, fs, controller),
        diagnosticService(socket, controller),
        controller(controller) {}
    void begin() 
    {
        stateService.begin();
        diagnosticService.begin();
    }
    void loop() 
    {
        controller.run();
        // stateService.updateState();
    }

private:
    HeliostatControllerStateService stateService;
    HeliostatStatelessService diagnosticService;
    HeliostatController &controller;
};

#endif
