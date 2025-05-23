#ifndef JSONSEQ
#define JSONSEQ
#include <vector>
#include <ArduinoJson.h>
#include <Preferences.h>
#include "controller.h"
#include <esp_wifi.h>
#include <esp_now.h>
extern Preferences preferences;
// namespace ESPNow{
//     void broadcast(const String &message);
//     void reply(const String &message);
//     bool sendMessage(const String &message, const uint8_t *macAddr);
//     void sendMessage(const String &message, String name);
//     void printAddress(const uint8_t *macAddr);
//     wifi_interface_t interface;
// }
class JsonSeq {
public:
    int selectedCommand = -1;
    int nextCommand = -1;
    std::vector<JsonDocument> commands{};
    MotorController &controller;
    uint32_t commandDuration = 0;
    uint32_t commandStart = 0;
    uint32_t commandID = -1;
    uint32_t lastCommandID = 0;
    uint32_t lostCommands = 0;
    bool isRunning = false;
    bool commandRunning = false;

    std::function<void(String)> broadcastMessage = nullptr;
    std::function<void(String, uint32_t)> sendMessage = nullptr;

    JsonSeq(MotorController &controller):
        controller(controller) {}

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
            if (parameters["pos"].is<float>()) controller.stopPosition = parameters["pos"].as<float>();
            if (parameters["dist"].is<float>()) controller.dist = parameters["dist"];
            if (parameters["speed"].is<int32_t>()) {
                controller.speed = parameters["speed"].as<int32_t>();
                controller.controller.setMaxSpeed(abs(int32_t(parameters["speed"].as<int32_t>())));
            }
            if (parameters["acc"].is<uint32_t>()) controller.controller.setAcceleration(parameters["acc"].as<uint32_t>());
            if (parameters["en"].is<int>()) parameters["en"].as<int>() > 0 ? controller.controller.enable() : controller.controller.disable();
            if (parameters["seq"].is<int>()) isRunning = parameters["seq"].as<int>() > 0;
        }
        if (command["c"].is<String>()) {
            String name = command["c"];
            if (name == "stop") controller.stop();
            else if (name == "run") {
                controller.controller.setSpeed(controller.speed);
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
        else if (command["n"].is<String>()) {
            String next = command["n"].as<String>();
            if (next.startsWith("+")) nextCommand = selectedCommand + next.substring(1).toInt();
            if (next.startsWith("-")) nextCommand = selectedCommand - next.substring(1).toInt();
        }
        else nextCommand = -1;
        if (command["trig"].is<int>()) {
            int commandNum = command["trig"];
            readCommand(commandNum);
        }
        if (command["t"].is<int>()) {
            int commandNum = command["t"];
            readCommand(commandNum);
        }
        if (command["b"].is<String>() && broadcastMessage) {
            broadcastMessage(command["b"].as<String>());
        }
        if (command["s"].is<String>() && command["a"].is<uint32_t>() && sendMessage) {
            uint32_t address = command["a"];
            String message = command["s"];
            // String res;
            // JsonDocument cmd;
            // deserializeJson(cmd, command["s"]);
            // cmd["id"] = commandID++;
            // serializeJson(cmd, res);
            sendMessage(message, address);
            // ESP_LOGI("Sequencer", "Send Message: %s, ID: %d", message.c_str(), address);
        }
        // if (command["sall"].is<String>()) {
        //     String res;
        //     JsonDocument cmd;
        //     deserializeJson(cmd, command["sall"]);
        //     cmd["id"] = commandID++;
        //     serializeJson(cmd, res);
        //     ESPNow::sendMessage(res, "all");
        // }
        // if (command["r"].is<int>()) {
        //     String res;
        //     JsonObject command;
        //     command["x"] = commands[command["r"]];
        //     command["i"] = command["r"];
        //     serializeJson(command, res);
        //     ESPNow::reply(res);
        // }
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
            if (controller.isMoving || controller.pingPong) controller.stop();
            commandRunning = false;
        }
    }
};
#endif