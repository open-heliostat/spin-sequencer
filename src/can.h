#ifndef CAN_ISO_TP_H
#define CAN_ISO_TP_H

#include <Arduino.h>
#include <CanIsoTp.hpp>

typedef struct {
    uint32_t counter;
    uint32_t counter1;
    uint32_t counter2;
    uint32_t counter3;
    uint32_t counter4;
    uint32_t counter5;
    uint32_t counter6;
    uint32_t counter7;
    uint32_t counter8;
    uint32_t counter9;
    uint32_t counter10;
    uint32_t counter11;

} MessageData;

template <typename T>
class CanIsoTPController
{
    uint8_t pinTX = D6;
    uint8_t pinRX = D7;

    CanIsoTp isoTpReceiver;
    T rxData, txData;
    pdu_t rxPdu, txPdu;

    T messageHistory[10]; // Array to store the last 10 messages
    int messageIndex = 0; // Index for the next message to be stored

public:
    uint32_t txId = 0x123;
    uint32_t rxId = 0x456;
    
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
        rxPdu.separationTimeMin = 0;
    
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
        int result = isoTpReceiver.receive(&rxPdu);
        if (result == 0 && rxPdu.cantpState == CANTP_END) {
            ESP_LOGI("CAN", "Receiver: Received message : %s", rxData);
            // Store the received message in history
            messageHistory[messageIndex] = rxData;
            messageIndex = (messageIndex + 1) % 10; // Wrap around the index
        }
    }

    bool sendMessage(const T& message) {
        if (!started) {
            ESP_LOGI("CAN", "CAN controller not started");
            return false;
        }

        // Prepare message for transmission
        txData = message;
        txPdu.data = (uint8_t*)&txData;
        txPdu.len = sizeof(txData);
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

    T* getMessageHistory() {
        return messageHistory;
    }

private:
    bool started = false;
};

#endif