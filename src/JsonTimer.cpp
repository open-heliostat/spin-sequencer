#include "JsonTimer.h"

void JsonTimer::checkTimers() {
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

void timerTask(void* parameter) {
    JsonTimer* timer = static_cast<JsonTimer*>(parameter);
    while (true) {
        timer->checkTimers();
        vTaskDelay(pdMS_TO_TICKS(30000)); // Check every 30 sec
    }
}