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

    static void readController(HeliostatController &controller, JsonObject &root) {
        root["azimuth"] = controller.azimuthController.targetAngle;
        root["elevation"] = controller.elevationController.targetAngle;
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
        _fsPersistence(HeliostatControllerState::read, HeliostatControllerState::update, this, fs, "heliostat-control.json"),
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
    void updateController(const String &originId) 
    {
        if (originId != "stateUpdate") {
            HeliostatControllerState::updateController(_state, controller);
        }
    }

private:
    EventEndpoint<HeliostatControllerState> _eventEndpoint;
    FSPersistence<HeliostatControllerState> _fsPersistence;
    HeliostatController &controller;
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

class HeliostatDiagnosticService : public StatefulService<HeliostatDiagnostic>
{

};

class HeliostatService
{
public:
    HeliostatService(EventSocket *socket, FS *fs, HeliostatController &controller) :
        stateService(socket, fs, controller),
        controller(controller) {}
    void begin() 
    {
        stateService.begin();
    }
    void loop() 
    {
        controller.run();
    }

private:
    HeliostatControllerStateService stateService;
    HeliostatController &controller;
};

#endif
