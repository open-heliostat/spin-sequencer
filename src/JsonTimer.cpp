#include "JsonTimer.h"

void JsonTimer::checkTimers() {
    time_t now;
    struct tm timeinfo;
    time(&now);
    localtime_r(&now, &timeinfo);

    // Only process if we have valid time
    if (timeinfo.tm_year > (2023 - 1900)) {
        // Convert tm_wday (0-6, Sunday = 0) to our DOW bitmask
        DaysOfWeek currentDay = (1 << timeinfo.tm_wday);

        if (executeLatestOnStart && !latestExecuted) {
            // Execute the latest missed timer if the flag is set
            executeLatestMissedTimer();
            latestExecuted = true; // Mark as executed to avoid re-executing
        }
        
        for (auto& timer : _timers) {
            // Check if it's time to execute, matches day of week, and hasn't been executed today
            if (timeinfo.tm_hour == timer.hour && 
                timeinfo.tm_min == timer.minute && 
                (timer.days & currentDay) && 
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

bool JsonTimer::executeLatestMissedTimer() {
    time_t now;
    struct tm timeinfo;
    time(&now);
    localtime_r(&now, &timeinfo);

    // Only process if we have valid time
    if (timeinfo.tm_year <= (2023 - 1900)) {
        return false;
    }

    // Convert tm_wday (0-6, Sunday = 0) to our DOW bitmask
    DaysOfWeek currentDay = (1 << timeinfo.tm_wday);
    
    // Current time in minutes since midnight
    int currentTimeMinutes = timeinfo.tm_hour * 60 + timeinfo.tm_min;
    
    // Find all timers that should have been executed today
    std::vector<std::pair<int, size_t>> eligibleTimers; // pair of (time_in_minutes, timer_index)
    
    for (size_t i = 0; i < _timers.size(); i++) {
        const auto& timer = _timers[i];
        
        // Check if timer is scheduled for today
        if (timer.days & currentDay) {
            int timerTimeMinutes = timer.hour * 60 + timer.minute;
            
            // Only include timers that should have already executed today
            if (timerTimeMinutes <= currentTimeMinutes && !timer.executed) {
                eligibleTimers.push_back(std::make_pair(timerTimeMinutes, i));
            }
        }
    }
    
    // If no eligible timers found, return false
    if (eligibleTimers.empty()) {
        return false;
    }
    
    // Find the latest timer (closest to current time)
    auto latestTimer = std::max_element(eligibleTimers.begin(), eligibleTimers.end());
    size_t timerIndex = latestTimer->second;
    
    // Execute the timer
    _sequencer.readCommand(_timers[timerIndex].jsonCommand);
    _timers[timerIndex].executed = true;
    
    Serial.printf("Executed missed timer: %02d:%02d\n", 
                  _timers[timerIndex].hour, 
                  _timers[timerIndex].minute);
    
    return true;
}

void timerTask(void* parameter) {
    JsonTimer* timer = static_cast<JsonTimer*>(parameter);
    while (true) {
        timer->checkTimers();
        vTaskDelay(pdMS_TO_TICKS(30000)); // Check every 30 sec
    }
}