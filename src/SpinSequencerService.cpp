#include <SpinSequencerService.h>

JsonRouter<SpinSequencerController> SpinSequencerControllerJsonRouter::router = JsonRouter<SpinSequencerController>(
{
    {"controller", [&](JsonVariant content, SpinSequencerController &controller) {
        return ClosedLoopControllerJsonRouter::router.parse(content, controller.controller);
    }},
    {"sequencer", [&](JsonVariant content, SpinSequencerController &controller) {
        return JsonSeqJsonRouter::router.parse(content, controller.jsonSeq);
    }},
},
{
    {"controller", [&](SpinSequencerController &controller, JsonVariant content) {
        if (content.is<JsonObject>()) ClosedLoopControllerJsonRouter::router.serialize(controller.controller, content);
    }},
    {"sequencer", [&](SpinSequencerController &controller, JsonVariant content) {
        if (content.is<JsonObject>()) JsonSeqJsonRouter::router.serialize(controller.jsonSeq, content);
    }},
    {"diag", [&](SpinSequencerController &controller, JsonVariant content) {
        if (content.is<JsonObject>()) {
            JsonObject obj = content.as<JsonObject>();
            JsonObject stepperDiag = obj["stepper"].to<JsonObject>();
            stepperDiag["isEnabled"] = controller.controller.stepper.isEnabled();
            stepperDiag["status"] = controller.controller.stepper.getStatus();
            stepperDiag["version"] = controller.controller.stepper.driver.version();
            JsonObject sequencerDiag = obj["sequencer"].to<JsonObject>();
            sequencerDiag["isRunning"] = controller.jsonSeq.isRunning;
            sequencerDiag["numCommands"] = controller.jsonSeq.commands.size();
            JsonObject mcuDiag = obj["mcu"].to<JsonObject>();
            mcuDiag["version"] = APP_VERSION;
            mcuDiag["freeHeap"] = ESP.getFreeHeap();
            mcuDiag["freeSketchSpace"] = ESP.getFreeSketchSpace();
        }
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