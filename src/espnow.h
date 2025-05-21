#ifndef ESPNOW_UTILS
#define ESPNOW_UTILS
#include <Arduino.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include <esp_now.h>
#include <map>
#include "jseq.h"

struct ESPNowEvent {
    const char *value;
    esp_now_peer_info_t peerInfo;
    unsigned long timestamp;
    bool isRegistered;
};

struct ESPNowState {
    bool enabled = false;
};

namespace ESPNow {
    extern const uint8_t broadcastAddress[6];
    extern uint8_t lastAddress[6];
    extern int maxSendRetries;
    extern int retryDelay;
    extern wifi_interface_t interface;
    extern ESPNowState state;
    extern std::function<void(String)> messageCallback;
    extern std::vector<String> messageHistory;
    extern int messageHistorySize;

    void formatMacAddress(const uint8_t *macAddr, char *buffer, int maxLength);
    void printAddress(const uint8_t *macAddr);
    void deletePeer(String peerName);
    bool sendMessage(const String &message, const uint8_t *macAddr);
    void broadcast(const String &message);
    void reply(const String &message);
    void ping(const uint8_t *macAddr);
    void receiveCallback(const uint8_t *macAddr, const uint8_t *data, int dataLen);
    void sentCallback(const uint8_t *macAddr, esp_now_send_status_t status);
    void setup();
    void update(unsigned long now);
    void getMacAddress(uint8_t *macAddr);
    String getMacAddress();
    void setMessageCallback(std::function<void(String)> callback);
}

#endif