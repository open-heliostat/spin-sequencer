/**
 *   ESP32 SvelteKit
 *
 *   A simple, secure and extensible framework for IoT projects for ESP32 platforms
 *   with responsive Sveltekit front-end built with TailwindCSS and DaisyUI.
 *   https://github.com/theelims/ESP32-sveltekit
 *
 *   Copyright (C) 2018 - 2023 rjwats
 *   Copyright (C) 2023 - 2024 theelims
 *
 *   All Rights Reserved. This software may be modified and distributed under
 *   the terms of the LGPL v3 license. See the LICENSE file for details.
 **/

#include <ESP32SvelteKit.h>
#include <PsychicHttpServer.h>
#include <SpinSequencerService.h>
#include <controller.h>
// #include <ESP32-TWAI-CAN.hpp>
// #include <CAN.h>        // https://github.com/adafruit/arduino-CAN
// #include <CanIsoTp.hpp> // https://github.com/MLeganes/arduino-can-iso-tp
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
// CanIsoTp isoTpSender;
CanIsoTp isoTpReceiver;
MessageData txData, rxData;
pdu_t txPdu, rxPdu;

// CanIsoTp
// CanIsoTp cantp;  // CAN IsoTp protocol to send and receive the pdus
// pdu_t cantp_pdu; // Pdu used in CAN IsoTp to transmit the data.

// uint8_t buff[128]; // Buffer used in the pdu.data.
// uint8_t msgSF[] = { 0x00, 0x01, 0x02, 0x03, 0x04};
// uint8_t msgCF[] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15};

#define SERIAL_BAUD_RATE 115200

PsychicHttpServer server;

ESP32SvelteKit esp32sveltekit(&server, 120);

FastAccelStepperEngine engine = FastAccelStepperEngine();

TMC5160Stepper driver1(D3, R_SENSE, D10, D9, D8);

TMC5160Controller stepper1 = {driver1, engine, D1, D0};

Encoder encoder1 = Encoder(D4, D5);

ClosedLoopController closedLoopController1 = {stepper1, encoder1};
MotorController motorController = {stepper1, encoder1};

SpinSequencerController spinSequencer = SpinSequencerController{motorController, closedLoopController1};

SpinSequencerService spinSequencerService = SpinSequencerService{&server, 
                                                                 &esp32sveltekit, 
                                                                 spinSequencer};
void setup()
{
    // start serial and filesystem
    // Serial.begin(SERIAL_BAUD_RATE);

    // increase httpd stack for HttpJsonRouter
    server.config.stack_size = 8192;

    // start ESP32-SvelteKit
    esp32sveltekit.begin();

    engine.init();
    stepper1.init();
    spinSequencerService.begin();

    if (!isoTpReceiver.begin(500, D6, D7)) {
        Serial.println("Failed to start TWAI");
        while (1);
    }

    // Setup Rx PDU for incoming data
    rxPdu.rxId = 0x123; 
    rxPdu.txId = 0x456;
    rxPdu.data = (uint8_t*)&rxData;
    rxPdu.len = sizeof(rxData);
    rxPdu.cantpState = CANTP_IDLE;  // Start in idle state
    rxPdu.blockSize = 0;
    rxPdu.separationTimeMin = 0;

    // Setup Tx PDU for responses
    txPdu.txId = 0x456;
    txPdu.rxId = 0x123;
    txPdu.data = (uint8_t*)&txData;
    txPdu.len = sizeof(txData);
    txPdu.cantpState = CANTP_IDLE;
    txPdu.blockSize = 0;
    txPdu.separationTimeMin = 5;

    // if (!isoTpSender.begin(500, D6, D7)) {
    //     Serial.println("Failed to start TWAI");
    //     while (1);
    // }

    // // Initialize data
    // txData.counter = 0;

    // // Setup Tx PDU
    // txPdu.txId = 0x123;  
    // txPdu.rxId = 0x456;
    // txPdu.data = (uint8_t*)&txData;
    // txPdu.len = sizeof(txData);
    // txPdu.cantpState = CANTP_IDLE;
    // txPdu.blockSize = 0;        
    // txPdu.separationTimeMin = 5;

    // // Setup Rx PDU for responses
    // rxPdu.txId = 0x456;
    // rxPdu.rxId = 0x123;
    // rxPdu.data = (uint8_t*)&rxData;
    // rxPdu.len = sizeof(rxData);
    // rxPdu.cantpState = CANTP_IDLE;
    // rxPdu.blockSize = 0;
    // rxPdu.separationTimeMin = 0;

    // CAN.setPins(D6, D7);

    // // // start the CAN bus at 500 kbps
    // if (!CAN.begin(500E3)) {
    //     ESP_LOGI("CAN", "Starting CAN failed!");
    //     while (1);
    // }
    // else {
    //     ESP_LOGI("CAN", "CAN started");
    // }
    // if(ESP32Can.begin(ESP32Can.convertSpeed(500), D6, D7, 5, 5)) {
    //     ESP_LOGI("CAN", "CAN bus started!");
    // } else {
    //     ESP_LOGI("CAN", "CAN bus failed!");
    // }

    // CanIsoTp setup
    // CAN.setPins(D6, D7);
    // cantp.begin(100000);    // Baud Rate
    // cantp_pdu.rxId = 0x100; // CanId rx.
    // cantp_pdu.txId = 0x200; // CanId tx.
}

// void receiveCanPacket() {
//     // try to parse packet
//     int packetSize = CAN.parsePacket();
  
//     if (packetSize || CAN.packetId() != -1) {
//         // received a packet
//         ESP_LOGI("CAN", "Received ");
    
//         if (CAN.packetExtended()) {
//             ESP_LOGI("CAN", "extended ");
//         }
    
//         if (CAN.packetRtr()) {
//             // Remote transmission request, packet contains no data
//             ESP_LOGI("CAN", "RTR ");
//         }
    
//         ESP_LOGI("CAN", "packet with id 0x");
    
//         if (CAN.packetRtr()) {
//             // Serial.print(" and requested length ");
//             // ESP_LOGI("CAN", CAN.packetDlc());
//         } else {
//             // Serial.print(" and length ");
//             // ESP_LOGI("CAN", packetSize);
    
//             // only print packet data for non-RTR packets
//             while (CAN.available()) {
//                 ESP_LOGI("CAN", "%c", (char)CAN.read());

//             }
//             // ESP_LOGI("CAN", "");
//         }
    
//         // ESP_LOGI("CAN", );
//     }
// }

// void sendCanPacket() {
//     // send packet: id is 11 bits, packet can contain up to 8 bytes of data
//     ESP_LOGI("CAN", "Sending packet ... ");
  
//     CAN.beginPacket(0x12);
//     CAN.write('h');
//     CAN.write('e');
//     CAN.write('l');
//     CAN.write('l');
//     CAN.write('o');
//     CAN.endPacket();
  
//     ESP_LOGI("CAN", "Packet sent");
// }

// CanFrame rxFrame;

// void sendObdFrame(uint8_t obdId) {
// 	CanFrame obdFrame = { 0 };
// 	obdFrame.identifier = 0x7DF; // Default OBD2 address;
// 	obdFrame.extd = 0;
// 	obdFrame.data_length_code = 8;
// 	obdFrame.data[0] = 2;
// 	obdFrame.data[1] = 1;
// 	obdFrame.data[2] = obdId;
// 	obdFrame.data[3] = 0xAA;    // Best to use 0xAA (0b10101010) instead of 0
// 	obdFrame.data[4] = 0xAA;    // CAN works better this way as it needs
// 	obdFrame.data[5] = 0xAA;    // to avoid bit-stuffing
// 	obdFrame.data[6] = 0xAA;
// 	obdFrame.data[7] = 0xAA;
//     // Accepts both pointers and references 
//     ESP32Can.writeFrame(obdFrame);  // timeout defaults to 1 ms
// }


unsigned long lastTick = 0;

void loop()
{
    // Delete Arduino loop task, as it is not needed in this example
    // vTaskDelete(NULL);
    spinSequencerService.loop();
    unsigned long now = millis();

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
    // cantp_pdu.data = buff;
    // if (cantp.receive(&cantp_pdu) == 0) {
    //     // convert msgCF uint8_t array into String
    //     String msg = "";
    //     for (int i = 0; i < cantp_pdu.len; i++) {
    //         msg += String(cantp_pdu.data[i], HEX);
    //         msg += " ";
    //     }
    //     ESP_LOGI("CAN", "Received packet ... %s", msg.c_str());
    // }
    // if(ESP32Can.readFrame(rxFrame, 1000)) {
    //     // Comment out if too many requests 
    //     ESP_LOGI("CAN", "Received frame: %03X \r\n", rxFrame.identifier);
    //     if(rxFrame.identifier == 0x7E8) {   // Standard OBD2 frame responce ID
    //         ESP_LOGI("CAN", "Collant temp: %3d°C \r\n", rxFrame.data[3] - 40); // Convert to °C
    //     }
    // }
    // receiveCanPacket();
    if (now - lastTick > 1000) {
        // ESP_LOGI("CAN", "Sending packet ... ");
        // lastTick = now;
        // txData.counter++;
        // txPdu.data = (uint8_t*)&txData;
        // txPdu.len = sizeof(txData);
        // if (isoTpSender.send(&txPdu) == 0) {
        //     ESP_LOGI("CAN", "Sender: Sent counter = %i", txData.counter);
        // } else {
        //     ESP_LOGI("CAN", "Sender: Error sending");
        // }

        // // Attempt to receive response
        // int result = isoTpSender.receive(&rxPdu);
        // if (result == 0 && rxPdu.cantpState == CANTP_IDLE) {
        //     ESP_LOGI("CAN", "Sender: Received response counter = %i", rxData.counter);
        // } else {
        //     ESP_LOGI("CAN", "Sender: Error receiving");
        // }

        // Multiframe
        // cantp_pdu.data = msgCF;
        // cantp_pdu.len = 22;
        // cantp.send(&cantp_pdu);
        // sendCanPacket();
        // sendObdFrame(5); // For coolant temperature
    //     if (WiFi.status() == WL_CONNECTED) {
    //         lightStateService.updateState(LightState{true, 0, 0.2, 0.1});
    //     }
    //     else {
    //         lightStateService.updateState(LightState{true, 0.2, 0.1, 0});
    //     }
    }
}
