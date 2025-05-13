#ifndef JsonTimer_h
#define JsonTimer_h

#include "jseq.h"
#include <ArduinoJson.h>

struct JsonDailyTimer {
    uint8_t hour;      // 0-23
    uint8_t minute;    // 0-59
    String jsonCommand; // Store the actual JSON command
    bool executed;     // Track if timer was executed today
};

class JsonTimer {
public:
    JsonTimer(JsonSeq& sequencer) : _sequencer(sequencer) {}

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

private:
    JsonSeq& _sequencer;
    std::vector<JsonDailyTimer> _timers;

    void checkTimers() {
        time_t now;
        struct tm timeinfo;
        time(&now);
        localtime_r(&now, &timeinfo);

        // Only process if we have valid time
        if (timeinfo.tm_year > (2023 - 1900)) {
            for (auto& timer : _timers) {
                // Check if it's time to execute and hasn't been executed today
                if (timeinfo.tm_hour == timer.hour && 
                    timeinfo.tm_min == timer.minute && 
                    !timer.executed) {
                    _sequencer.readCommand(timer.jsonCommand);
                    timer.executed = true;
                }
                // Reset executed flag at midnight
                else if (timeinfo.tm_hour == 0 && timeinfo.tm_min == 0) {
                    timer.executed = false;
                }
            }
        }
    }

    friend void timerTask(void* parameter);
};

// FreeRTOS task function
void timerTask(void* parameter) {
    JsonTimer* timer = static_cast<JsonTimer*>(parameter);
    while (true) {
        timer->checkTimers();
        vTaskDelay(pdMS_TO_TICKS(60000)); // Check every minute
    }
}

#endif