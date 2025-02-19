#include <SpinSequencerService.h>

JsonRouter<SpinSequencerController> SpinSequencerControllerJsonRouter::router = JsonRouter<SpinSequencerController>(
{
    {"controller", [&](JsonVariant content, SpinSequencerController &controller) {
        return ClosedLoopControllerJsonRouter::router.parse(content, controller.motorController);
    }}
},
{
    {"controller", [&](SpinSequencerController &controller, JsonVariant content) {
        if (content.is<JsonObject>()) ClosedLoopControllerJsonRouter::router.serialize(controller.motorController, content);
    }}
});


void SpinSequencerService::begin() 
{
    _httpRouterEndpoint.begin();
    _fsPersistence.readFromFS();
    _state.init();
}
void SpinSequencerService::loop() 
{
    _state.run();

}