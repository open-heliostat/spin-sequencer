#ifndef SPIN_SEQ_CLASS_H
#define SPIN_SEQ_CLASS_H
#include <Arduino.h>
#include <closedloopcontroller.h>
#include <controller.h>
#include <jseq.h>
// #include <CanControllerService.h>
#include <RemoteService.h>
#include <JsonTimerService.h>
#include <ESPNowService.h>

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

private:
    friend class SpinSequencerControllerJsonRouter;
    void configureHardwarePins();
    void handleHardwareControls();
    void handleStartButton();
    void updateStatusLed();
    bool readStartButtonPressed();
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
                jsonSeq.isRunning = true;
                jsonSeq.readCommand();
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
#endif