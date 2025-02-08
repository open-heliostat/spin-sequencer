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
#include <LightMqttSettingsService.h>
#include <LightStateService.h>
#include <PsychicHttpServer.h>
#include <StepperControlService.h>
#include <GPSService.h>
#include <EncoderService.h>
#include <ClosedLoopControllerService.h>
#include <HeliostatService.h>

#define SERIAL_BAUD_RATE 115200

PsychicHttpServer server;

ESP32SvelteKit esp32sveltekit(&server, 120);

FastAccelStepperEngine engine = FastAccelStepperEngine();
LightMqttSettingsService lightMqttSettingsService = LightMqttSettingsService(&server,
                                                                             &esp32sveltekit);

TMC5160Stepper driver1(10, R_SENSE, 13, 11, 12);
TMC5160Stepper driver2(7, R_SENSE, 13, 11, 12);

TMC5160Controller stepper1 = {driver1, engine, 9, 8};
TMC5160Controller stepper2 = {driver2, engine, 6, 5};

Encoder encoder1 = Encoder(2, 1);
Encoder encoder2 = Encoder(4, 3, Wire1);

ClosedLoopController closedLoopController1 = {stepper1, encoder1};
ClosedLoopController closedLoopController2 = {stepper2, encoder2};

SerialGPS gpsneo = SerialGPS(Serial1, TX, RX);

HeliostatController heliostatController = {closedLoopController1, closedLoopController2, gpsneo};

HeliostatService heliostatService = HeliostatService(
    &server,
    esp32sveltekit.getSocket(),
    esp32sveltekit.getFS(),
    esp32sveltekit.getSecurityManager(),
    heliostatController);

GPSSettingsService gpsSettingsService = GPSSettingsService(
    &server,
    esp32sveltekit.getFS(),
    esp32sveltekit.getSecurityManager(),
    &gpsneo);

GPSStateService gpsStateService =  GPSStateService(
    esp32sveltekit.getSocket(),
    &gpsSettingsService,
    &gpsneo,
    esp32sveltekit.getFeatureService());

// std::vector<TMC5160Controller*> steppers = {&stepper1, &stepper2};
// std::vector<ClosedLoopController*> closedLoopControllers = {&closedLoopController1, &closedLoopController2};

// StepperSettingsService stepperSettingsService = StepperSettingsService(
//     esp32sveltekit.getSocket(),
//     esp32sveltekit.getFS(),
//     steppers);

// StepperControlService stepperControlService = StepperControlService(
//     esp32sveltekit.getSocket(),
//     &stepperSettingsService,
//     steppers,
//     esp32sveltekit.getFeatureService());

// EncoderStateService encoderService = EncoderStateService(
//     esp32sveltekit.getSocket(),
//     &encoder1);

// ClosedLoopControllerService closedLoopControllerService = ClosedLoopControllerService(
//     &server,
//     esp32sveltekit.getSocket(),
//     esp32sveltekit.getFS(),
//     esp32sveltekit.getSecurityManager(),
//     closedLoopController1);
LightStateService lightStateService = LightStateService(&server,
                                                        &esp32sveltekit,
                                                        &lightMqttSettingsService);

void setup()
{
    // start serial and filesystem
    Serial.begin(SERIAL_BAUD_RATE);

    // increase httpd stack for HttpJsonRouter
    server.config.stack_size = 8192;

    // start ESP32-SvelteKit
    esp32sveltekit.begin();

    // // load the initial light settings
    // lightStateService.begin();
    // // start the light service
    // lightMqttSettingsService.begin();

    engine.init();
    stepper1.init();
    stepper2.init();

    gpsneo.init();
    gpsSettingsService.begin();
    gpsStateService.begin();

    heliostatService.begin();
    
    // closedLoopControllerService.begin();
}

unsigned long lastTick = 0;

void loop()
{
    // Delete Arduino loop task, as it is not needed in this example
    // vTaskDelete(NULL);
    heliostatService.loop();
    unsigned long now = millis();
    if (now - lastTick > 1000) {
        lastTick = now;
        gpsStateService.loop();
        if (WiFi.status() == WL_CONNECTED) {
            lightStateService.updateState(LightState{true, 0, 0.2, 0.1});
        }
        else {
            lightStateService.updateState(LightState{true, 0.2, 0.1, 0});
        }
        // if (encoder1.hasNewData()) Serial.println(encoder1.angle);
        // if (encoder2.hasNewData()) Serial.println(encoder2.angle);
    }
}
