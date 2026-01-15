#ifndef TMCDRIVER_h
#define TMCDRIVER_h

#define R_SENSE 0.075f

#include <TMCStepper.h>
#include "FastAccelStepper.h"

struct TMC5160Controller {
    TMC5160Stepper &driver;
    FastAccelStepperEngine &engine;
    FastAccelStepper *stepper = NULL;
    bool enabled = false;
    bool direction = false;
    bool running = false;
    uint16_t stepsPerRotation = 200;
    uint16_t microsteps = 256;
    uint16_t current = 30;
    uint32_t maxSpeed = 40;
    uint32_t maxAccel = 20;
    static constexpr uint8_t VIRTUAL_DIR_PIN = PIN_EXTERNAL_FLAG | 1;
    inline static TMC5160Controller* externalDirOwner = nullptr;
    const char* msteps;
    const char* pwmfr;
    const char* freewh;
    const int DIR;
    const int STEP;

    TMC5160Controller(TMC5160Stepper &driver, FastAccelStepperEngine &engine, const int STEP, const int DIR) : driver {driver}, engine {engine}, STEP {STEP}, DIR {DIR} {}

    static bool externalDirCallback(uint8_t pin, uint8_t value) {
        if (!externalDirOwner) return false;
        if (pin != VIRTUAL_DIR_PIN) return value;
        externalDirOwner->driver.shaft(value != 0);
        return value != 0; // return requested state to avoid blocking on readback
    }

    void init() {
        pinMode(STEP, OUTPUT);
        driver.begin();                 //  SPI: Init CS pins and possible SW SPI pins
        if (!isConnected()) Serial.println("Driver communication error");
        Serial.print("Driver firmware version: ");
        Serial.println(driver.version());
        if (driver.sd_mode()) Serial.println("Driver is hardware configured for Step & Dir mode");
        if (driver.drv_enn()) Serial.println("Driver is not hardware enabled");


        Serial.print("DRV_STATUS=0b");
        Serial.println(driver.DRV_STATUS(), BIN);
        initDriver();
        pinMode(DIR, OUTPUT);
        digitalWrite(DIR, HIGH);

        stepper = engine.stepperConnectToPin(STEP);
        if (stepper) {
            externalDirOwner = this;
            engine.setExternalCallForPin(externalDirCallback);
            stepper->setDirectionPin(VIRTUAL_DIR_PIN);
            stepper->setSpeedInHz(maxSpeed*microsteps);       // 200 steps/s
            stepper->setAcceleration(maxAccel*microsteps);    // 40 steps/s²
            ESP_LOGI("TMC", "Init: Stepper configured");
        }
        else {
            ESP_LOGE("TMC", "Init: Stepper connection failed");
        }
    }

    bool isConnected() {
        return !(driver.version() == 0xFF || driver.version() == 0);
    }

    void initDriver() {
        driver.GSTAT(1);
        // driver.defaults();
        // driver.toff(3);
        // enable();
        driver.rms_current(500);
        driver.en_pwm_mode(true);
        driver.s2g_level(9);
        driver.s2vs_level(9);
        driver.bbmclks(2);
        // driver.shaft(true);
        disable();
        // stepper->attachToPulseCounter(6, -200*microsteps, 200*microsteps);
    }
    void setMaxSpeed() {
        stepper->setSpeedInHz(maxSpeed*microsteps);
    }

    void setMaxSpeed(uint32_t sp) {
        stepper->setSpeedInHz(sp*microsteps);
    }

    uint32_t getMaxSpeed() {
        return stepper->getSpeedInMilliHz()/(1000*microsteps);
    }

    // void setSpeed(double sp) {
    //     sp = min(max(-1., sp), 1.);
    //     // Serial.print("Set : ");
    //     // Serial.println(sp);
    //     if (sp == 0) stepper->stopMove();
    //     else {
    //         setMaxSpeed(abs(sp*maxSpeed));
    //         if (sp < 0) stepper->runBackward();
    //         else stepper->runForward();
    //     }
    // }

    void setSpeed(int32_t sp) {
        if (sp == 0) stepper->stopMove();
        else {
            setMaxSpeed(abs(sp));
            if (sp < 0) stepper->runBackward();
            else stepper->runForward();
        }
    }

    int32_t getSpeed() {
        if (!stepper) return 0;
        return stepper->getCurrentSpeedInMilliHz()/(1000*microsteps);
    }

    void stop() {
        setSpeed(int32_t(0));
    }

    int32_t move() {
        if (!stepper) return 0;
        return (stepper->targetPos()-stepper->getCurrentPosition())/microsteps;
    }

    void moveR(int32_t move) {
        if (!stepper) return;
        stepper->moveTo(stepper->getCurrentPosition() + move * microsteps);
    }

    void moveR(double angle) {
        if (!stepper) return;
        stepper->moveTo(stepper->getCurrentPosition() + angle*stepsPerRotation/360. * microsteps);
        // ESP_LOGI("Driver", "MoveR %f", angle);
        // Serial.println(driver.XACTUAL());
        // driver.RAMPMODE(0);
        // driver.VSTART(100);
        // driver.AMAX(stepper->getAcceleration());
        // driver.a1(stepper->getAcceleration());
        // driver.VMAX(stepper->getMaxSpeedInHz());
        // driver.v1(stepper->getMaxSpeedInHz());
        // driver.DMAX(stepper->getMaxSpeedInHz());
        // driver.d1(stepper->getMaxSpeedInHz());
        // driver.VSTOP(100);
        // driver.XTARGET(driver.XACTUAL() + angle*stepsPerRotation/360. * microsteps);
    }

    void move(double angle) {
        if (!stepper) return;
        stepper->move((angle*stepsPerRotation/360.-double(this->move()))*microsteps);
    }

    void move(int32_t move) {
        if (!stepper) return;
        stepper->move((move-this->move())*microsteps);
    }

    double mod(double a, double N) {return a - N*floor(a/N);}

    void setAngle(double angle) {
        double dist = angle - getAngle();
        moveR(dist);
    }

    double getAngle() {
        if (!stepper) return 0.0;
        return mod(stepper->getCurrentPosition()*360./double(microsteps*stepsPerRotation), 360.);
    }

    double getTargetAngle() {
        if (!stepper) return 0.0;
        return mod(stepper->targetPos()*360./double(microsteps), 360.);
    }

    uint32_t getStatus() {
        return driver.DRV_STATUS();
    }

    void enable() {
        driver.toff(3);
        enabled = true;
    }

    bool isEnabled() {
        return driver.isEnabled();
    }

    void disable() {
        driver.toff(0);
        enabled = false;
    }

    void setMicroSteps(uint16_t ms) {
        if (ms > 128) ms = 256;
        else if (ms > 64) ms = 128;
        else if (ms > 32) ms = 64;
        else if (ms > 16) ms = 32;
        else if (ms > 8) ms = 16;
        else if (ms > 4) ms = 8;
        else if (ms > 2) ms = 4;
        else if (ms > 1) ms = 2;
        else ms = 0;
        microsteps = ms ? ms : 1;
        ESP_LOGI("Driver", "Microsteps : %d", microsteps);
        driver.microsteps(ms);
    }

    uint16_t getMicroSteps() {
        uint16_t ms = driver.microsteps();
        microsteps = ms ? ms : 1;
        return microsteps;
    }

    // const char* getMicroSteps() {
    //     uint16_t m = driver.microsteps();
    //     microsteps = m ? m : 1;
    //     if (msteps != NULL) free((char*)msteps);
    //     msteps = strdup(String(microsteps).c_str());
    //     return msteps;
    // }

    void setAcceleration(uint32_t acc) {
        if (!stepper) return;
        stepper->setAcceleration(acc*microsteps);
    }

    uint32_t getAcceleration() {
        if (!stepper) return 0;
        return uint32_t(stepper->getAcceleration())/uint32_t(microsteps);
    }

    uint32_t getStepsToStop() {
        return getSpeed() * getSpeed() / (getAcceleration() * 2);
    }

    const char* pwm_freq() {
        if (pwmfr != NULL) free((char*)pwmfr);
        pwmfr = strdup(String(driver.pwm_freq()).c_str());
        return pwmfr;
    }

    const char* getFreeWheel() {
        if (freewh != NULL) free((char*)freewh);
        freewh = strdup(String(driver.freewheel()).c_str());
        return freewh;
    }
};

#endif