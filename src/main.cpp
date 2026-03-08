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
// #include <CanControllerService.h>
#include <ESPNowService.h>
// UDP sender for JSON router
#include <UdpSenderService.h>
// #include <lora.h>

#define SERIAL_BAUD_RATE 115200

PsychicHttpServer server;

ESP32SvelteKit esp32sveltekit(&server, 200);

FastAccelStepperEngine engine = FastAccelStepperEngine();

// TMC driver and Ethernet share the same SPI bus with different CS pins
// Ethernet: CS=D1, TMC: CS=D3
// Both use the same SPI pins: SCK=D8, MISO=D9, MOSI=D10
// TMC Stepper uses STEP=D2, DIR=D0
TMC5160Stepper driver1(STEPPER_CS_PIN, R_SENSE, -1, &SPI);

TMC5160Controller stepper1 = {driver1, engine, STEPPER_STEP_PIN, STEPPER_DIR_PIN};

Encoder encoder1 = Encoder(D4, D5);

ClosedLoopController closedLoopController1 = {stepper1, encoder1};
MotorController motorController = {stepper1, encoder1};

// CanIsoTPController<CanIsoTPMessage> canController = CanIsoTPController<CanIsoTPMessage>();
// CanControllerService canControllerService = CanControllerService{&server, 
//                                                                     &esp32sveltekit, 
//                                                                     canController};

SpinSequencerController spinSequencer = SpinSequencerController{motorController, closedLoopController1, &server};

SpinSequencerService spinSequencerService = SpinSequencerService{&server, 
                                                                 &esp32sveltekit, 
                                                                 spinSequencer};

ESPNowService espnowService = ESPNowService(&server, &esp32sveltekit, ESPNow::state);

UdpSenderState udpSenderState = UdpSenderState{};
UdpSenderService udpSenderService = UdpSenderService(&server, &esp32sveltekit, udpSenderState);

// LoRaController loraController = LoRaController();

void setup()
{
    // start serial and filesystem
    Serial.begin(SERIAL_BAUD_RATE);

    // init I2C encoder after heap/RTOS are up
    encoder1.begin();

    // increase httpd stack for HttpJsonRouter
    server.config.stack_size = 8192;
    server.config.max_resp_headers = 12;
    server.config.max_open_sockets = 11;
    server.config.lru_purge_enable = true;

    // start ESP32-SvelteKit (Ethernet will initialize shared SPI bus)
    esp32sveltekit.begin();

    engine.init();
    stepper1.init();
    spinSequencerService.begin();
    stepper1.disable();

    pinMode(STEPPER_ENABLE_PIN, OUTPUT);
    digitalWrite(STEPPER_ENABLE_PIN, LOW); // Enable stepper driver (active low)

    udpSenderService.begin();
    
    // start CAN controller
    // canControllerService.begin();

    // Pull pin 42 high to disable SPI on the LORA module
    // pinMode(41, OUTPUT);
    // digitalWrite(41, HIGH);

    // start LoRa controller
    // loraController.begin();

    espnowService.begin();

}

unsigned long lastTick = 0;

void loop()
{
    // Delete Arduino loop task, as it is not needed in this example
    // vTaskDelete(NULL);
    spinSequencerService.loop();
    // unsigned long now = millis();

    // canControllerService.loop();

    // if (now - lastTick > 1000) {
    //     ESP_LOGI("CAN", "Sending packet ... ");
    //     lastTick = now;
    // }
}
