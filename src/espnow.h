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

struct ESPNowPeer {
    esp_now_peer_info_t peerInfo;
    uint32_t pingTimestamp;
    uint32_t numPings;
    uint32_t numSent;
    uint32_t numLost;
    double pingMeanTime;
};

struct ESPNowState {
    bool enabled = false;
    int channel = 0;
};

namespace ESPNow {
    // Static instance to hold callback state
    class ESPNowInstance {
    public:
        static ESPNowInstance& getInstance() {
            static ESPNowInstance instance;
            return instance;
        }
        std::function<void(String)> messageCallback;
    private:
        ESPNowInstance() {}
    };

    extern const uint8_t broadcastAddress[6];
    extern uint8_t lastAddress[6];
    extern int maxSendRetries;
    extern int retryDelay;
    extern wifi_interface_t interface;
    extern ESPNowState state;
    extern std::vector<String> messageHistory;
    extern std::vector<ESPNowPeer> peerList;
    extern int messageHistorySize;

    void formatMacAddress(const uint8_t *macAddr, char *buffer, int maxLength);
    void printAddress(const uint8_t *macAddr);
    void deletePeer(String peerName);
    bool sendMessage(const String &message, const uint8_t *macAddr);
    void broadcast(const String &message);
    void reply(const String &message);
    void ping(const uint8_t *macAddr);
    void staticReceiveCallback(const uint8_t *macAddr, const uint8_t *data, int dataLen);
    void sentCallback(const uint8_t *macAddr, esp_now_send_status_t status);
    void setup();
    void update(unsigned long now);
    void getMacAddress(uint8_t *macAddr);
    String getMacAddress();
    void setMessageCallback(std::function<void(String)> callback);
}

#endif