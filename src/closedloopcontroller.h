#ifndef CLOSED_LOOP_CONTROLLER_H
#define CLOSED_LOOP_CONTROLLER_H

#include <Arduino.h>
#include <tmcdriver.h>
#include <encoder.h>

class ClosedLoopController
{
public:
    TMC5160Controller &stepper;
    Encoder &encoder;
    uint32_t maxPollInterval = 100;
    bool enabled;
    double targetAngle;
    double tolerance = 0.1;
    ClosedLoopController(TMC5160Controller &stepper, Encoder &encoder) : stepper(stepper), encoder(encoder) {}
    void setAngle(double angle) {
        targetAngle = angle;
        double curAngle = encoder.getAngle();
        double toGo = targetAngle - curAngle;
        if (abs(toGo) > tolerance) stepper.moveR(toGo);
    }
    double getAngle(){
        return encoder.getAngle();
    }
    void run() {
        if (enabled && millis() - lastPoll >= maxPollInterval) {
            setAngle(targetAngle);
            lastPoll = millis();
        }
    }
private:
    uint32_t lastPoll = 0;
};
#endif