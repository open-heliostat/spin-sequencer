#ifndef JsonTimer_h
#define JsonTimer_h

#include "jseq.h"
#include <ArduinoJson.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// Forward declare JsonTimer class for use in timerTask
class JsonTimer;

// FreeRTOS task function declaration 
void timerTask(void* parameter);

struct JsonDailyTimer {
    uint8_t hour;      // 0-23
    uint8_t minute;    // 0-59
    String jsonCommand; // Store the actual JSON command
    bool executed;     // Track if timer was executed today
};

class JsonTimer {
public:
    JsonTimer(JsonSeq& sequencer) : _sequencer(sequencer), _timerTaskHandle(nullptr) {}
    
    void begin() {
        xTaskCreate(
            timerTask,              // Function that should be called
            "Timer Task",           // Name of the task (for debugging)
            4096,                   // Stack size (bytes)
            this,                   // Pass reference to this class instance
            1,                      // Task priority
            &_timerTaskHandle       // Task handle
        );
    }

    void end() {
        if (_timerTaskHandle != nullptr) {
            vTaskDelete(_timerTaskHandle);
            _timerTaskHandle = nullptr;
        }
    }

    void addJsonDailyTimer(uint8_t hour, uint8_t minute, const String& jsonCommand) {
        if (hour > 23 || minute > 59) return;
        
        // Create a test parse to validate JSON
        JsonDocument testDoc;
        DeserializationError error = deserializeJson(testDoc, jsonCommand);
        if (error) {
            Serial.println("Invalid JSON command");
            return;
        }

        JsonDailyTimer timer = {
            .hour = hour,
            .minute = minute,
            .jsonCommand = jsonCommand,
            .executed = false
        };
        
        _timers.push_back(timer);
    }

    // Overload to accept JsonDocument directly
    void addJsonDailyTimer(uint8_t hour, uint8_t minute, const JsonDocument& command) {
        String jsonString;
        serializeJson(command, jsonString);
        addJsonDailyTimer(hour, minute, jsonString);
    }

    // Get a list of all timers
    std::vector<JsonDailyTimer> getTimers() const {
        return _timers;
    }

    // Get timer at specific index
    const JsonDailyTimer* getTimer(size_t index) const {
        if (index < _timers.size()) {
            return &_timers[index];
        }
        return nullptr;
    }

    // Remove timer at specific index
    bool removeTimer(size_t index) {
        if (index < _timers.size()) {
            _timers.erase(_timers.begin() + index);
            return true;
        }
        return false;
    }

    // Remove timer by time
    bool removeDailyTimer(uint8_t hour, uint8_t minute) {
        auto it = std::find_if(
            _timers.begin(),
            _timers.end(),
            [hour, minute](const JsonDailyTimer& timer) {
                return timer.hour == hour && timer.minute == minute;
            }
        );
        
        if (it != _timers.end()) {
            _timers.erase(it);
            return true;
        }
        return false;
    }

    void checkTimers();

private:
    JsonSeq& _sequencer;
    std::vector<JsonDailyTimer> _timers;
    TaskHandle_t _timerTaskHandle;
    friend void timerTask(void* parameter);
};

#endif