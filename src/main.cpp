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
#include <StepperSettingsService.h>
#include <gpsneo.h>

#define SERIAL_BAUD_RATE 115200

PsychicHttpServer server;

ESP32SvelteKit esp32sveltekit(&server, 120);

FastAccelStepperEngine engine = FastAccelStepperEngine();

TMC5160Stepper driver1(5, R_SENSE, MOSI, MISO, SCK);
// TMC5160Stepper driver2(16, R_SENSE, MOSI, MISO, SCK);

TMC5160Controller stepper1 = {driver1, engine, 21, 17, EN_PIN};
// TMC5160Controller stepper2 = {driver2, engine, STEP_PIN, DIR_PIN, EN_PIN};

LightMqttSettingsService lightMqttSettingsService = LightMqttSettingsService(
    &server,
    esp32sveltekit.getFS(),
    esp32sveltekit.getSecurityManager());

LightStateService lightStateService = LightStateService(
    &server,
    esp32sveltekit.getSocket(),
    esp32sveltekit.getSecurityManager(),
    esp32sveltekit.getMqttClient(),
    &lightMqttSettingsService);

StepperSettingsService stepperSettingsService = StepperSettingsService(
    &server, 
    esp32sveltekit.getFS(), 
    esp32sveltekit.getSecurityManager(), 
    &stepper1);

StepperControlService stepperControlService = StepperControlService(
    esp32sveltekit.getSocket(),
    &stepperSettingsService,
    &stepper1);


void setup()
{
    // start serial and filesystem
    Serial.begin(SERIAL_BAUD_RATE);

    esp32sveltekit.getFeatureService()->addFeature("stepper", true);

    // start ESP32-SvelteKit
    esp32sveltekit.begin();

    // load the initial light settings
    lightStateService.begin();
    // start the light service
    lightMqttSettingsService.begin();


    stepper1.init();

    stepperSettingsService.begin();
    stepperControlService.begin();

    setConfigGPS();
}

void loop()
{
    // Delete Arduino loop task, as it is not needed in this example
    // vTaskDelete(NULL);
    updateGPS();
}
