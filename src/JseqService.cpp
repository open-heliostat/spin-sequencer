#include <JseqService.h>

// Convert vector of JsonDocument to JsonArray of String
JsonArray jsonDocumentVectorToStringsArray(std::vector<JsonDocument> commands)
{
    JsonDocument doc;
    JsonArray result = doc.to<JsonArray>();
    for (auto& command : commands) {
        String commandStr;
        serializeJson(command, commandStr);
        result.add(commandStr);
        Serial.println(commandStr);
    }
    Serial.println(doc.as<String>());
    return result;
}
// Convert String array to JsonDocument vector
std::vector<JsonDocument> stringArrayToJsonDocumentVector(JsonArray array) {
    std::vector<JsonDocument> result;
    for (JsonVariant value : array) {
        if (value.is<const char*>() || value.is<String>()) {
            JsonDocument doc;
            DeserializationError error = deserializeJson(doc, value.as<String>());
            if (!error) {
                result.push_back(doc);
            }
            else Serial.println(error.c_str());
        }
    }
    return result;
}

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
        JsonArray commands = target["commands"].to<JsonArray>();
        for (auto& command : sequencer.commands) {
            String commandStr;
            serializeJson(command, commandStr);
            commands.add(commandStr);
        }
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
    }},
    {"commands", [](JsonVariant content, JsonSeq &sequencer) {
        if (content.is<JsonArray>()) {
            sequencer.commands = stringArrayToJsonDocumentVector(content);
            return true;
        }
        Serial.println("Failed to parse : " + content.as<String>());
        return false;
    }}
});