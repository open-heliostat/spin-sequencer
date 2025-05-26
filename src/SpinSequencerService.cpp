#include <SpinSequencerService.h>

JsonRouter<SpinSequencerController> SpinSequencerControllerJsonRouter::router = JsonRouter<SpinSequencerController>(
{
    {"controller", [&](JsonVariant content, SpinSequencerController &controller) {
        return ClosedLoopControllerJsonRouter::router.parse(content, controller.controller);
    }},
    {"sequencer", [&](JsonVariant content, SpinSequencerController &controller) {
        return JsonSeqJsonRouter::router.parse(content, controller.jsonSeq);
    }},
    {"remotes", [&](JsonVariant content, SpinSequencerController &controller) {
        return RemoteJsonRouter::router.parse(content, controller.remotesController);
    }},
    {"timers", [&](JsonVariant content, SpinSequencerController &controller) {
        return JsonTimerRouter::router.parse(content, controller.jsonTimer);
    }},
    {"welcome", [&](JsonVariant content, SpinSequencerController &controller) {
        if (content.is<JsonObject>()) {
            JsonObject obj = content.as<JsonObject>();
            if (obj["text"].is<String>()) {
                controller.welcomeText = obj["text"].as<String>();
            }
            return true;
        }
        return false;
    }},
},
{
    {"controller", [&](SpinSequencerController &controller, JsonVariant content) {
        if (content.is<JsonObject>()) ClosedLoopControllerJsonRouter::router.serialize(controller.controller, content);
    }},
    {"sequencer", [&](SpinSequencerController &controller, JsonVariant content) {
        if (content.is<JsonObject>()) JsonSeqJsonRouter::router.serialize(controller.jsonSeq, content);
    }},
    {"remotes", [&](SpinSequencerController &controller, JsonVariant content) {
        if (content.is<JsonObject>()) RemoteJsonRouter::router.serialize(controller.remotesController, content);
    }},
    {"timers", [&](SpinSequencerController &controller, JsonVariant content) {
        if (content.is<JsonObject>()) JsonTimerRouter::router.serialize(controller.jsonTimer, content);
    }},
    {"welcome", [&](SpinSequencerController &controller, JsonVariant content) {
        if (content.is<JsonObject>()) {
            JsonObject obj = content.as<JsonObject>();
            obj["text"] = controller.welcomeText;
        }
    }},
    {"diag", [&](SpinSequencerController &controller, JsonVariant content) {
        if (content.is<JsonObject>()) {
            JsonObject obj = content.as<JsonObject>();
            JsonObject stepperDiag = obj["stepper"].to<JsonObject>();
            stepperDiag["isEnabled"] = controller.controller.stepper.isEnabled();
            stepperDiag["status"] = controller.controller.stepper.getStatus();
            stepperDiag["version"] = controller.controller.stepper.driver.version();
            JsonObject mcuDiag = obj["mcu"].to<JsonObject>();
            mcuDiag["version"] = APP_VERSION;
            mcuDiag["freeHeap"] = ESP.getFreeHeap();
            mcuDiag["freeSketchSpace"] = ESP.getFreeSketchSpace();
            mcuDiag["sketchSize"] = ESP.getSketchSize();
            // mcuDiag["sketchMD5"] = ESP.getSketchMD5();
            mcuDiag["cpuFreq"] = ESP.getCpuFreqMHz();
            // mcuDiag["chipModel"] = ESP.getChipModel();
            // mcuDiag["chipRevision"] = ESP.getChipRevision();
            // mcuDiag["sdkVersion"] = ESP.getSdkVersion();
            // mcuDiag["flashChipSize"] = ESP.getFlashChipSize();
            JsonObject wifiDiag = obj["wifi"].to<JsonObject>();
            wifiDiag["ssid"] = WiFi.SSID();
            wifiDiag["rssi"] = WiFi.RSSI();
            wifiDiag["ip"] = WiFi.localIP().toString();
            wifiDiag["hostname"] = WiFi.getHostname();
            wifiDiag["apEnabled"] = WiFi.softAPgetStationNum();
            wifiDiag["channel"] = WiFi.channel();
            JsonObject encoderDiag = obj["encoder"].to<JsonObject>();
            encoderDiag["angle"] = controller.controller.encoder.getAngle();
            encoderDiag["error"] = controller.controller.encoder.error;
            encoderDiag["invert"] = controller.controller.encoder.invert;
            encoderDiag["newData"] = controller.controller.encoder.hasNewData();
            JsonObject canDiag = obj["can"].to<JsonObject>();
            canDiag["enabled"] = controller.canController.enabled;
            canDiag["messagePack"] = controller.canController.messagePack;
            canDiag["rxId"] = controller.canController.rxId;
            canDiag["speed"] = controller.canController.getSpeed();
            JsonObject httpDiag = obj["http"].to<JsonObject>();
            httpDiag["numClients"] = controller.server->count();
            httpDiag["maxClients"] = controller.server->config.max_open_sockets;
            httpDiag["maxUriHandlers"] = controller.server->config.max_uri_handlers;
            httpDiag["lruPurgeEnable"] = controller.server->config.lru_purge_enable;
            JsonObject sequencerDiag = obj["sequencer"].to<JsonObject>();
            sequencerDiag["isRunning"] = controller.jsonSeq.isRunning;
            sequencerDiag["numCommands"] = controller.jsonSeq.commands.size();
            // sequencerDiag["lostCommands"] = controller.jsonSeq.lostCommands;
            // sequencerDiag["lastCommandID"] = controller.jsonSeq.lastCommandID;
            // sequencerDiag["commandDuration"] = controller.jsonSeq.commandDuration;
            // sequencerDiag["commandStart"] = controller.jsonSeq.commandStart;
            // sequencerDiag["commandRunning"] = controller.jsonSeq.commandRunning;
            // sequencerDiag["commandID"] = controller.jsonSeq.commandID;
            sequencerDiag["selectedCommand"] = controller.jsonSeq.selectedCommand;
            sequencerDiag["nextCommand"] = controller.jsonSeq.nextCommand;
            JsonObject remotesDiag = obj["remotes"].to<JsonObject>();
            remotesDiag["isMaster"] = controller.remotesController.isMaster;
            remotesDiag["numRemotes"] = controller.remotesController.remotes.size();
            JsonObject espnowDiag = obj["espnow"].to<JsonObject>();
            espnowDiag["enabled"] = ESPNow::state.enabled;
            espnowDiag["macAddress"] = ESPNow::getMacAddress();
        }
    }},
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