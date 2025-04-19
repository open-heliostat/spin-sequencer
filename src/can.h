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

    uint32_t txId = 0x123;
    uint32_t rxId = 0x456;

    CanIsoTp isoTpReceiver;
    T rxData, txData;
    pdu_t rxPdu, txPdu;

public:
    CanIsoTPController(uint32_t txId = 0x123, uint32_t rxId = 0x456) : txId(txId), rxId(rxId) {
        // Constructor
    }

    void CanIsoTPController::begin() {
        if (!isoTpReceiver.begin(500, pinTX, pinRX)) {
            Serial.println("Failed to start TWAI");
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
    }
    void CanIsoTPController::loop() {
        int result = isoTpReceiver.receive(&rxPdu);
        if (result == 0 && rxPdu.cantpState == CANTP_END) {
            ESP_LOGI("CAN", "Receiver: Received counter = %i", rxData.counter);
    
            // Prepare response
            txData.counter = rxData.counter + 100; // Just an example modification
            txPdu.data = (uint8_t*)&txData;
            txPdu.len = sizeof(txData);
    
            // Send response
            if (isoTpReceiver.send(&txPdu) == 0) {
                ESP_LOGI("CAN", "Receiver: Sent response counter = %i", txData.counter);
            } else {
                ESP_LOGI("CAN", "Receiver: Error sending response");
            }
        }
    }
};

#endif