#ifndef SPIN_SEQ_CLASS_H
#define SPIN_SEQ_CLASS_H
#include <Arduino.h>
#include <closedloopcontroller.h>
#include <controller.h>
#include <jseq.h>
#include <ArduinoJson.h>
#include <deque>
// #include <CanControllerService.h>
#include <RemoteService.h>
#include <JsonTimerService.h>
#include <ESPNowService.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <ETH.h>
#include <esp_netif.h>
#include <UdpRouterEndpoint.h>

#ifndef FACTORY_SEQ_START_BUTTON_PIN
#define FACTORY_SEQ_START_BUTTON_PIN -1
#endif
#ifndef FACTORY_SEQ_START_BUTTON_ACTIVE_LOW
#define FACTORY_SEQ_START_BUTTON_ACTIVE_LOW 1
#endif
#ifndef FACTORY_SEQ_START_BUTTON_DEBOUNCE_MS
#define FACTORY_SEQ_START_BUTTON_DEBOUNCE_MS 50
#endif
#ifndef FACTORY_SEQ_STATUS_LED_PIN
#define FACTORY_SEQ_STATUS_LED_PIN -1
#endif
#ifndef FACTORY_SEQ_STATUS_LED_ACTIVE_HIGH
#define FACTORY_SEQ_STATUS_LED_ACTIVE_HIGH 1
#endif

struct SequencerHardwareConfig
{
    int startButtonPin = -1;
    bool startButtonActiveLow = true;
    uint16_t startButtonDebounceMs = 50;
    int statusLedPin = -1;
    bool statusLedActiveHigh = true;
};

struct SequencerHardwareState
{
    bool startButtonReading = false;
    bool startButtonStable = false;
    unsigned long startButtonChangeMs = 0;
    bool statusLedState = false;
};

class SpinSequencerController
{
public:
    MotorController &motorController;
    ClosedLoopController &controller;
    JsonSeq jsonSeq;
    // CanIsoTPController<CanIsoTPMessage> &canController;
    PsychicHttpServer *server;
    RemotesController remotesController = {};
    JsonTimer jsonTimer = {jsonSeq};
    String welcomeText = "";
    SequencerHardwareConfig hardwareConfig = {};
    SequencerHardwareState hardwareState = {};
    std::deque<String> udpMessageHistory = {};

    SpinSequencerController(MotorController &motorController, ClosedLoopController &controller, PsychicHttpServer *server) :
            motorController(motorController), controller(controller), jsonSeq(motorController), server(server) 
    {
        hardwareConfig.startButtonPin = FACTORY_SEQ_START_BUTTON_PIN;
        hardwareConfig.startButtonActiveLow = FACTORY_SEQ_START_BUTTON_ACTIVE_LOW;
        hardwareConfig.startButtonDebounceMs = FACTORY_SEQ_START_BUTTON_DEBOUNCE_MS;
        hardwareConfig.statusLedPin = FACTORY_SEQ_STATUS_LED_PIN;
        hardwareConfig.statusLedActiveHigh = FACTORY_SEQ_STATUS_LED_ACTIVE_HIGH;

        // canController.messageCallback = [this](String message) {
        //     jsonSeq.readCommand(message);
        // };
        // canController.clientMappingCallback = [this](uint32_t rxId) {
        //     remotesController.addRemote(rxId);
        // };
        jsonSeq.broadcastMessage = [&](String message) {
            // canController.sendMessage(message, uint32_t(0));
            ESPNow::broadcast(message);
        };
        jsonSeq.broadcastMessageWithRetry = [&](String message, int numRetries) {
            ESPNow::broadcast(message, numRetries);
        };
        jsonSeq.udpBroadcastMessage = [&](String message) {
            sendUdpBroadcast(message);
        };
        // jsonSeq.sendMessage = [&](String message, uint32_t address) {
        //     canController.sendMessage(message, address);
        // };
        ESPNow::setMessageCallback([&](String message) {
            ESP_LOGI("ESPNow", "Message: %s", message.c_str());
            jsonSeq.readCommand(message);
        });
    }
    
    void init() 
    {
        jsonTimer.begin();
        // add all remotes to espnow hosts list
        for (auto &remote : remotesController.remotes) {
            ESPNow::addPeer(remote.macAddress);
        }
        configureHardwarePins();
    }

    void run()
    {
        controller.run();
        motorController.tick();
        handleHardwareControls();
        jsonSeq.tick();
        ESPNow::update(millis());
    }

    void appendUdpMessage(const String &message);

private:
    friend class SpinSequencerControllerJsonRouter;
    void configureHardwarePins();
    void handleHardwareControls();
    void handleStartButton();
    void updateStatusLed();
    bool readStartButtonPressed();
    void sendUdpBroadcast(const String &message);
};

inline void SpinSequencerController::configureHardwarePins()
{
    if (hardwareConfig.startButtonPin >= 0) {
        pinMode(hardwareConfig.startButtonPin, hardwareConfig.startButtonActiveLow ? INPUT_PULLUP : INPUT);
        bool pressed = readStartButtonPressed();
        hardwareState.startButtonReading = pressed;
        hardwareState.startButtonStable = pressed;
        hardwareState.startButtonChangeMs = millis();
    }

    if (hardwareConfig.statusLedPin >= 0) {
        pinMode(hardwareConfig.statusLedPin, OUTPUT);
        updateStatusLed();
    }
}

inline bool SpinSequencerController::readStartButtonPressed()
{
    if (hardwareConfig.startButtonPin < 0) return false;
    int level = digitalRead(hardwareConfig.startButtonPin);
    return hardwareConfig.startButtonActiveLow ? level == LOW : level == HIGH;
}

inline void SpinSequencerController::handleStartButton()
{
    if (hardwareConfig.startButtonPin < 0) return;

    unsigned long now = millis();
    bool pressed = readStartButtonPressed();

    if (pressed != hardwareState.startButtonReading) {
        hardwareState.startButtonChangeMs = now;
        hardwareState.startButtonReading = pressed;
    }

    if ((now - hardwareState.startButtonChangeMs) >= hardwareConfig.startButtonDebounceMs) {
        if (hardwareState.startButtonStable != hardwareState.startButtonReading) {
            hardwareState.startButtonStable = hardwareState.startButtonReading;
            if (hardwareState.startButtonStable && !jsonSeq.isRunning) {
                // Mirror the web UI start behavior: always launch command 1 and mark running
                jsonSeq.selectCommand(1);
                jsonSeq.isRunning = true;
                jsonSeq.readCommand(1);
            }
        }
    }
}

inline void SpinSequencerController::updateStatusLed()
{
    if (hardwareConfig.statusLedPin < 0) return;
    bool desired = hardwareConfig.statusLedActiveHigh ? jsonSeq.isRunning : !jsonSeq.isRunning;
    if (hardwareState.statusLedState != desired) {
        digitalWrite(hardwareConfig.statusLedPin, desired ? HIGH : LOW);
        hardwareState.statusLedState = desired;
    }
}

inline void SpinSequencerController::handleHardwareControls()
{
    handleStartButton();
    updateStatusLed();
}

inline void SpinSequencerController::sendUdpBroadcast(const String &message)
{
    int iface = ETH.linkUp() ? 2 : 0;

    const char *ifkey = iface == 2 ? "ETH_DEF" : "WIFI_STA_DEF";
    if (ifkey)
    {
        if (esp_netif_t *netif = esp_netif_get_handle_from_ifkey(ifkey))
        {
            esp_netif_set_default_netif(netif);
        }
    }

    IPAddress ip = (iface == 2) ? ETH.localIP() : WiFi.localIP();
    IPAddress mask = (iface == 2) ? ETH.subnetMask() : WiFi.subnetMask();
    IPAddress dest(ip[0] | ~mask[0], ip[1] | ~mask[1], ip[2] | ~mask[2], ip[3] | ~mask[3]);

    JsonDocument outbound;
    JsonObject root = outbound.to<JsonObject>();
    root["method"] = "POST";
    root["path"] = "/rest/spin-seq/sequencer/control/execute/";
    root["body"]["execute"] = message;

    String payload;
    serializeJson(outbound, payload);

    WiFiUDP udp;
    if (!udp.begin(0)) return;
    if (udp.beginPacket(dest, JSON_ROUTER_UDP_PORT) != 1) { udp.stop(); return; }
    udp.write(reinterpret_cast<const uint8_t *>(payload.c_str()), payload.length());
    udp.endPacket();
    udp.stop();
}

inline void SpinSequencerController::appendUdpMessage(const String &message)
{
    udpMessageHistory.push_back(message);
    while (udpMessageHistory.size() > 50) {
        udpMessageHistory.pop_front();
    }
}
#endif