#include "JsonTimer.h"

void JsonTimer::checkTimers() {
    time_t now;
    struct tm timeinfo;
    time(&now);
    localtime_r(&now, &timeinfo);

    // Only process if we have valid time
    if (timeinfo.tm_year > (2023 - 1900)) {
        // Check if NTP just became available and we need to check for missed timers
        if (!_ntpSynced) {
            _ntpSynced = true;
            checkMissedTimers();
        }
        
        // Convert tm_wday (0-6, Sunday = 0) to our DOW bitmask
        DaysOfWeek currentDay = (1 << timeinfo.tm_wday);
        
        for (auto& timer : _timers) {
            // Check if it's time to execute, matches day of week, and hasn't been executed today
            if (timeinfo.tm_hour == timer.hour && 
                timeinfo.tm_min == timer.minute && 
                (timer.days & currentDay) && 
                !timer.executed) {
                _sequencer.readCommand(timer.jsonCommand);
                timer.executed = true;
                timer.lastExecuted = now;
            }
            // Reset executed flag at midnight
            else if (timeinfo.tm_hour == 0 && timeinfo.tm_min == 0) {
                timer.executed = false;
            }
        }
        
        _lastCheckTime = now;
    } else {
        // Time not yet synced, mark as not synced
        _ntpSynced = false;
    }
}

void JsonTimer::checkMissedTimers() {
    time_t now;
    struct tm timeinfo;
    time(&now);
    localtime_r(&now, &timeinfo);
    
    // Only proceed if we have valid time
    if (timeinfo.tm_year <= (2023 - 1900)) {
        return;
    }
    
    // Convert current day to DOW bitmask
    DaysOfWeek currentDay = (1 << timeinfo.tm_wday);
    
    // Find the latest timer that should have been executed today but wasn't
    JsonDailyTimer* latestMissedTimer = nullptr;
    int latestMissedMinutes = -1;
    
    // Calculate current time in minutes since midnight
    int currentMinutes = timeinfo.tm_hour * 60 + timeinfo.tm_min;
    
    for (auto& timer : _timers) {
        // Check if this timer should run today
        if (timer.days & currentDay) {
            int timerMinutes = timer.hour * 60 + timer.minute;
            
            // Check if timer time has already passed today
            if (timerMinutes < currentMinutes) {
                // Check if this timer was executed today
                bool executedToday = false;
                if (timer.lastExecuted > 0) {
                    struct tm lastExecTime;
                    localtime_r(&timer.lastExecuted, &lastExecTime);
                    
                    // Check if last execution was today
                    executedToday = (lastExecTime.tm_year == timeinfo.tm_year &&
                                   lastExecTime.tm_yday == timeinfo.tm_yday);
                }
                
                // If not executed today and this is the latest missed timer
                if (!executedToday && timerMinutes > latestMissedMinutes) {
                    latestMissedTimer = &timer;
                    latestMissedMinutes = timerMinutes;
                }
            }
        }
    }
    
    // Execute the latest missed timer if found
    if (latestMissedTimer != nullptr) {
        Serial.printf("Executing missed timer: %02d:%02d\n", 
                     latestMissedTimer->hour, latestMissedTimer->minute);
        _sequencer.readCommand(latestMissedTimer->jsonCommand);
        latestMissedTimer->executed = true;
        latestMissedTimer->lastExecuted = now;
    }
}

void timerTask(void* parameter) {
    JsonTimer* timer = static_cast<JsonTimer*>(parameter);
    while (true) {
        timer->checkTimers();
        vTaskDelay(pdMS_TO_TICKS(30000)); // Check every 30 sec
    }
}