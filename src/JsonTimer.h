#ifndef JsonTimer_h
#define JsonTimer_h

#include "jseq.h"
#include <ArduinoJson.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <vector>
#include <algorithm>

// Forward declare JsonTimer class for use in timerTask
class JsonTimer;

// FreeRTOS task function declaration 
void timerTask(void* parameter);

// Add days of week bitfield definition
typedef uint8_t DaysOfWeek;
#define DOW_SUNDAY    0x01
#define DOW_MONDAY    0x02
#define DOW_TUESDAY   0x04
#define DOW_WEDNESDAY 0x08
#define DOW_THURSDAY  0x10
#define DOW_FRIDAY    0x20
#define DOW_SATURDAY  0x40
#define DOW_ALL       0x7F

struct JsonDailyTimer {
    uint8_t hour;      // 0-23
    uint8_t minute;    // 0-59
    DaysOfWeek days;   // Bitfield for days of week
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

    void addJsonDailyTimer(uint8_t hour, uint8_t minute, DaysOfWeek days, const String& jsonCommand) {
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
            .days = days,
            .jsonCommand = jsonCommand,
            .executed = false
        };
        
        _timers.push_back(timer);
    }

    // Overload to accept JsonDocument directly
    void addJsonDailyTimer(uint8_t hour, uint8_t minute, DaysOfWeek days, const JsonDocument& command) {
        String jsonString;
        serializeJson(command, jsonString);
        addJsonDailyTimer(hour, minute, days, jsonString);
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

    // Remove timer by time and days
    bool removeDailyTimer(uint8_t hour, uint8_t minute, DaysOfWeek days) {
        auto it = std::find_if(
            _timers.begin(),
            _timers.end(),
            [hour, minute, days](const JsonDailyTimer& timer) {
                return timer.hour == hour && timer.minute == minute && timer.days == days;
            }
        );
        
        if (it != _timers.end()) {
            _timers.erase(it);
            return true;
        }
        return false;
    }

    void checkTimers();
    
    // Execute the latest timer that should have been executed (useful after reboot)
    bool executeLatestMissedTimer();

    bool executeLatestOnStart = false; // Flag to execute latest timer on start

private:
    JsonSeq& _sequencer;
    std::vector<JsonDailyTimer> _timers;
    TaskHandle_t _timerTaskHandle;
    friend void timerTask(void* parameter);
    bool latestExecuted = false; // Track if the latest timer was executed
};

#endif