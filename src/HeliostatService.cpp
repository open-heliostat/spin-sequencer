#include <HeliostatService.h>

JsonRouter<HeliostatController> HeliostatControllerJsonRouter::router = JsonRouter<HeliostatController>(
{
    {"azimuth", [&](JsonVariant content, HeliostatController &controller) {
        return ClosedLoopControllerJsonRouter::router.parse(content, controller.azimuthController);
    }},
    {"elevation", [&](JsonVariant content, HeliostatController &controller) {
        return ClosedLoopControllerJsonRouter::router.parse(content, controller.elevationController);
    }}
},
{
    {"azimuth", [&](HeliostatController &controller, JsonVariant content) {
        ClosedLoopControllerJsonRouter::router.serialize(controller.azimuthController, content);
    }},
    {"elevation", [&](HeliostatController &controller, JsonVariant content) {
        ClosedLoopControllerJsonRouter::router.serialize(controller.elevationController, content);
    }}
});

void HeliostatService::begin() 
{
    _stateService.begin();
    _httpRouterEndpoint.begin();
    _fsPersistence.readFromFS();
}
void HeliostatService::loop() 
{
    _state.run();
    // _stateService.updateState();
}