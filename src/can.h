#ifndef CAN_ISO_TP_H
#define CAN_ISO_TP_H

#include <Arduino.h>
#include <CanIsoTp.hpp>

template <typename T>
class CanIsoTPController
{
    uint8_t pinTX = D6;
    uint8_t pinRX = D7;

    CanIsoTp isoTpReceiver;
    T rxData, txData;
    pdu_t rxPdu, txPdu;

    std::vector<String> messageHistory;
    const int messageHistorySize = 10;

public:
    uint32_t txId = 0x123;
    uint32_t rxId = 0x456;
    bool enabled = false;
    
    CanIsoTPController(uint32_t txId = 0x123, uint32_t rxId = 0x456) : txId(txId), rxId(rxId) {
        // Constructor
    }

    void begin() {
        if (!isoTpReceiver.begin(500, pinTX, pinRX)) {
            ESP_LOGI("CAN", "Failed to start TWAI");
            while (1);
        }
    
        // Setup Rx PDU for incoming data
        rxPdu.rxId = rxId; 
        rxPdu.txId = txId;
        rxPdu.data = (uint8_t*)&rxData;
        rxPdu.len = sizeof(rxData);
        rxPdu.cantpState = CANTP_IDLE;  // Start in idle state
        rxPdu.blockSize = 0;
        rxPdu.separationTimeMin = 5;
    
        // Setup Tx PDU for responses
        txPdu.txId = txId;
        txPdu.rxId = rxId;
        txPdu.data = (uint8_t*)&txData;
        txPdu.len = sizeof(txData);
        txPdu.cantpState = CANTP_IDLE;
        txPdu.blockSize = 0;
        txPdu.separationTimeMin = 5;

        started = true;
    }
    void loop() {
        if (enabled && started) {
            int result = isoTpReceiver.receive(&rxPdu);
            if (result == 0 && rxPdu.cantpState == CANTP_END) {
                ESP_LOGI("CAN", "Receiver: Received message : %s", (char*)rxData.message);
                // Store the received message in history
                if (messageHistory.size() < messageHistorySize) {
                    messageHistory.push_back(String(rxData.message));
                } 
                else {
                    messageHistory.erase(messageHistory.begin());
                    messageHistory.push_back(String(rxData.message));
                }
                rxPdu.cantpState = CANTP_IDLE; // Reset state for next message
                rxPdu.data = (uint8_t*)&rxData;
            }
            else if (rxPdu.cantpState == CANTP_ERROR) {
                ESP_LOGI("CAN", "Receiver: Error in receiving message");
                rxPdu.cantpState = CANTP_IDLE; // Reset state for next message
                rxPdu.data = (uint8_t*)&rxData;
            }
        }
    }

    bool sendMessage(const T& message, uint32_t txId) {
        txId = txId;
        return sendMessage(message);
    }

    bool sendMessage(const T& message) {
        if (!started) {
            ESP_LOGI("CAN", "CAN controller not started");
            return false;
        }

        // Prepare message for transmission
        txData = message;
        txPdu.txId = txId;
        txPdu.data = (uint8_t*)&txData;
        txPdu.len = String(txData.message).length() + 1;
        txPdu.cantpState = CANTP_IDLE;

        // Attempt to send the message
        int result = isoTpReceiver.send(&txPdu);
        if (result == 0) {
            ESP_LOGI("CAN", "Sender: Message sent successfully");
            return true;
        } else {
            ESP_LOGI("CAN", "Sender: Failed to send message (error: %d)", result);
            return false;
        }
    }

    std::vector<String> getMessageHistory() {
        return messageHistory;
    }

private:
    bool started = false;
};

#endif