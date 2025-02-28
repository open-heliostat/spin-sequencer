#include <JseqService.h>

// Define the main router
JsonRouter<JsonSeq> JsonSeqJsonRouter::router = JsonRouter<JsonSeq>(
{
    {"config", [](JsonVariant content, JsonSeq &sequencer) {
        return configRouter.parse(content, sequencer);
    }},
    {"control", [](JsonVariant content, JsonSeq &sequencer) {
        return controlRouter.parse(content, sequencer);
    }}
},
{
    {"status", [](JsonSeq &sequencer, const JsonVariant target) {
        target["isRunning"] = sequencer.isRunning;
        target["selectedCommand"] = sequencer.selectedCommand;
        target["commandRunning"] = sequencer.commandRunning;
        target["nextCommand"] = sequencer.nextCommand;
        target["lostCommands"] = sequencer.lostCommands;
    }},
    {"config", [](JsonSeq &sequencer, const JsonVariant target) {
        // target["commands"] = sequencer.commands;
        target["selectedCommand"] = sequencer.selectedCommand;
        target["isRunning"] = sequencer.isRunning;
    }}
});

// Define the control router
JsonEventRouter<JsonSeq> JsonSeqJsonRouter::controlRouter = JsonEventRouter<JsonSeq>({
    {"select", [](JsonVariant content, JsonSeq &sequencer) {
        if (content.is<int>()) {
            sequencer.selectCommand(content.as<int>());
            return true;
        }
        return false;
    }},
    {"run", [](JsonVariant content, JsonSeq &sequencer) {
        if (content.is<bool>()) {
            sequencer.isRunning = content.as<bool>();
            if (sequencer.isRunning) sequencer.readCommand();
            return true;
        }
        return false;
    }},
    {"execute", [](JsonVariant content, JsonSeq &sequencer) {
        if (content.is<String>()) {
            sequencer.readCommand(content.as<String>());
        }
        return true;
    }}
});

// Define the config router
JsonEventRouter<JsonSeq> JsonSeqJsonRouter::configRouter = JsonEventRouter<JsonSeq>({
    // {"save", [](JsonVariant content, JsonSeq &sequencer) {
    //     sequencer.saveJson();
    //     return true;
    // }},
    // {"load", [](JsonVariant content, JsonSeq &sequencer) {
    //     sequencer.loadJson();
    //     return true;
    // }},
    {"command", [](JsonVariant content, JsonSeq &sequencer) {
        if (content.is<JsonObject>() && content["index"].is<int>()) {
            int index = content["index"].as<int>();
            sequencer.selectCommand(index);
            if (content["data"].is<JsonVariant>()) {
                deserializeJson(sequencer.commands[index], content["data"]);
            }
            return true;
        }
        return false;
    }}
});