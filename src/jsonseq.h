#ifndef JSONSEQ_h
#define JSONSEQ_h
#include <vector>
#include <ArduinoJson.h>
#include <Preferences.h>
#include <esp_wifi.h>
#include <esp_now.h>
#include <closedloopcontroller.h>
extern Preferences preferences;
namespace ESPNow{
    void broadcast(const String &message);
    void reply(const String &message);
    bool sendMessage(const String &message, const uint8_t *macAddr);
    void sendMessage(const String &message, String name);
    void printAddress(const uint8_t *macAddr);
    wifi_interface_t interface;
}
class JsonSeq 
{
public:
    int selectedCommand = -1;
    int nextCommand = -1;
    std::vector<JsonDocument> commands{};
    ClosedLoopController &controller;
    uint32_t commandDuration = 0;
    uint32_t commandStart = 0;
    uint32_t commandID = -1;
    uint32_t lastCommandID = 0;
    uint32_t lostCommands = 0;
    bool isRunning = false;
    bool commandRunning = false;
    JsonSeq(ClosedLoopController &controller):
        controller(controller) {}
    void loadJson() {
        preferences.begin("json");
        int commandsNum = preferences.getInt("commandsNum", 1);
        for (int i = 0; i < commandsNum; i++) {
            String command = preferences.getString((String("seq")+String(i)).c_str(), String("null"));
            // if (command == String("null") || i > 100) break;
            if (commands.size() <= i) commands.push_back(JsonDocument());
            deserializeJson(commands[i], command);
        }
        preferences.end();
        loadSettings();
    }
    void saveJson() {
        preferences.begin("json");
        int commandsNum = min(int(commands.size()), 32);
        preferences.putInt("commandsNum", commandsNum);
        for (int i = 0; i < commandsNum; i++) {
            String data;
            serializeJson(commands[i], data);
            preferences.putString((String("seq")+String(i)).c_str(), data);
        }
        preferences.end();
        saveSettings();
    }
    void selectCommand(int select) {
        while (select >= commands.size()) commands.push_back(JsonDocument());
        selectedCommand = select;
    }
    void readCommand(JsonDocument &command) {
        if (command["id"].is<int>()) {
            if (command["id"] == lastCommandID) {
                Serial.println("Duplicate command ID, command ignored.");
                return;
            }
            else {
                if (lastCommandID != 0) {
                    lostCommands += max(int32_t(command["id"]) - int32_t(lastCommandID) - 1, 0);
                }
                lastCommandID = command["id"];
            }
        }
        if (command["p"].is<JsonObject>()) {
            JsonObject parameters = command["p"];
            if (parameters["pos"].is<float>()) controller.setAngle(parameters["pos"]);
            if (parameters["dist"].is<float>()) controller.dist = parameters["dist"];
            if (parameters["speed"].is<float>()) controller.stepper.setSpeed(parameters["speed"].as<double>());
            if (parameters["acc"].is<float>()) controller.stepper.setAcceleration(parameters["acc"]);
            if (parameters["seq"].is<int>()) isRunning = parameters["seq"].as<int>() > 0;
        }
        if (command["c"].is<String>()) {
            String name = command["c"];
            if (name == "stop") controller.stepper.setSpeed(0);
            else if (name == "run") {
                controller.stepper.setSpeed(controller.speed);
                controller.pingPong = false;
            }
            else if (name == "pp") {
                controller.setPingPong(true);
            }
            else if (name == "move") {
                controller.move(controller.dist);
                controller.pingPong = false;
            }
            else if (name == "moveTo") {
                controller.setPosition(controller.stopPosition);
                controller.pingPong = false;
            }
        }
        if (command["d"].is<int>()) {
            commandDuration = command["d"];
            commandStart = millis();
        }
        else commandDuration = 0;
        if (command["n"].is<int>()) {
            nextCommand = command["n"];
        }
        else nextCommand = -1;
        if (command["trig"].is<int>()) {
            int commandNum = command["trig"];
            readCommand(commandNum);
        }
        if (command["b"].is<String>()) {
            ESPNow::broadcast(command["b"]);
        }
        if (command["s"].is<String>() && command["a"].is<String>()) {
            String name = command["a"];
            String res;
            JsonDocument cmd;
            deserializeJson(cmd, command["s"]);
            cmd["id"] = commandID++;
            serializeJson(cmd, res);
            ESPNow::sendMessage(res, name);
        }
        if (command["sall"].is<String>()) {
            String res;
            JsonDocument cmd;
            deserializeJson(cmd, command["sall"]);
            cmd["id"] = commandID++;
            serializeJson(cmd, res);
            ESPNow::sendMessage(res, "all");
        }
        if (command["w"].is<JsonArray>() && command["i"].is<int>()) {
            if (command["i"].size()) {
                for (int i = 0; i < command["i"].size(); i++) {
                    int j = command["i"][i];
                    while (j >= commands.size()) commands.push_back(JsonDocument());
                    if (command["w"].size()) {
                        deserializeJson(commands[j], command["w"][i]);
                    }
                    else deserializeJson(commands[j], command["w"]);
                }
            }
            else {
                while (command["i"] >= commands.size()) commands.push_back(JsonDocument());
                deserializeJson(commands[command["i"]], command["w"]);
            }
        }
        if (command["r"].is<int>()) {
            String res;
            JsonObject command;
            command["x"] = commands[command["r"]];
            command["i"] = command["r"];
            serializeJson(command, res);
            ESPNow::reply(res);
        }
        if (command["reboot"].is<JsonVariant>()) ESP.restart();

        // commandStart = millis();
        commandRunning = true;
    }
    void readCommand(String cmd) {
        JsonDocument command;
        deserializeJson(command, cmd);
        readCommand(command);
    }
    void readCommand() {
        if (selectedCommand > -1 && selectedCommand < commands.size()) {
            JsonDocument &command = commands[selectedCommand];
            readCommand(command);
        }
        else {
            Serial.println("Command Not Found");
        }
    }
    void readCommand(int commandNum) {
        selectCommand(commandNum);
        readCommand();
    }
    void tick() {
        if (isRunning) {
            if ((commandDuration > 0 && millis() - commandStart > commandDuration) || (commandDuration == 0 && !controller.isMoving && controller.hasChangedState)) {
                if (nextCommand > -1) readCommand(nextCommand);
                else isRunning = false;
            }
        }
        else if (commandRunning && commandDuration > 0 && millis() - commandStart > commandDuration) {
            if (controller.isMoving) controller.stop();
            commandRunning = false;
        }
    }
    void saveSettings() {
        preferences.begin("jseq");
        preferences.putInt("selCmd", selectedCommand);
        preferences.putBool("running", isRunning);
        preferences.end();
    }
    void loadSettings() {
        preferences.begin("jseq");
        selectCommand(preferences.getInt("selCmd", 0));
        isRunning = preferences.getBool("running", isRunning);
        if (isRunning) readCommand();
        preferences.end();
    }
};
#endif