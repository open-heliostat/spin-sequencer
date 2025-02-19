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

#define SERIAL_BAUD_RATE 115200

PsychicHttpServer server;

ESP32SvelteKit esp32sveltekit(&server, 120);

FastAccelStepperEngine engine = FastAccelStepperEngine();

TMC5160Stepper driver1(D3, R_SENSE, D10, D9, D8);

TMC5160Controller stepper1 = {driver1, engine, D1, D0};

Encoder encoder1 = Encoder(D4, D5);

ClosedLoopController closedLoopController1 = {stepper1, encoder1};

SpinSequencerController spinSequencer = SpinSequencerController{closedLoopController1};

SpinSequencerService spinSequencerService = SpinSequencerService{&server, 
                                                                 &esp32sveltekit, 
                                                                 spinSequencer};
void setup()
{
    // start serial and filesystem
    Serial.begin(SERIAL_BAUD_RATE);

    // increase httpd stack for HttpJsonRouter
    server.config.stack_size = 8192;

    // start ESP32-SvelteKit
    esp32sveltekit.begin();

    engine.init();
    stepper1.init();
    spinSequencerService.begin();
}

unsigned long lastTick = 0;

void loop()
{
    // Delete Arduino loop task, as it is not needed in this example
    // vTaskDelete(NULL);
    spinSequencerService.loop();
    unsigned long now = millis();
    // if (now - lastTick > 1000) {
    //     lastTick = now;
    //     if (WiFi.status() == WL_CONNECTED) {
    //         lightStateService.updateState(LightState{true, 0, 0.2, 0.1});
    //     }
    //     else {
    //         lightStateService.updateState(LightState{true, 0.2, 0.1, 0});
    //     }
    // }
}
