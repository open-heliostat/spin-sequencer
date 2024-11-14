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
    uint32_t maxPollInterval = 50;
    bool enabled;
    double targetAngle;
    double tolerance = 0.1;
    bool hasLimits = false;
    double limitA = 0.;
    double limitB = 360.;
    double calibrationDecay = 0.1;
    int calibrationSpeed = 5;
    bool hasCalibration = false;
    bool calibrationRunning = false;
    static const int calibrationSteps = 128;
    float calibrationOffsets[calibrationSteps];
    double calibrationStepperStartOffset = 0.;
    ClosedLoopController(TMC5160Controller &stepper, Encoder &encoder) : stepper(stepper), encoder(encoder) {}
    double mod(double a, double N) {return a - N*floor(a/N);}
    void setAngle(double angle) {
        targetAngle = angle;
        double curAngle = getAngle();
        if (encoder.hasNewData()) {
            double toGo = 0.;
            if (hasLimits) {
                double middle = mod((limitA + limitB) * 0.5, 360.);
                double interval = limitB - limitA;
                if (limitB < limitA) {
                    middle = mod(middle + 180., 360.);
                    interval = mod(interval, 360.);
                }
                double t = mod(targetAngle - middle + 180., 360.) - 180.;
                targetAngle = mod(max(min(t, interval*0.5), -interval*0.5) + middle, 360.);
                toGo = mod(targetAngle - middle + 180., 360.) - mod(curAngle - middle + 180., 360.);
                // Serial.printf("Middle : %f`, Interval: %f, t: %f, Target: %f, To Go: %f\n", middle, interval, t, targetAngle, toGo);
            }
            else toGo = mod(targetAngle - curAngle + 180., 360.) - 180.;
            // Serial.printf("Command : %f, Target: %f, Current: %f, To Go: %f\n", angle, targetAngle, curAngle, toGo);
            if (abs(toGo) > tolerance) stepper.moveR(toGo);
        }
    }
    double getAngle(){
        if (hasCalibration) return getCalibratedAngle();
        else return encoder.getAngle();
    }
    double lerp(double a, double b, double t) {
        return b * t + a * (1. - t);
    }
    double getCalibratedAngle() {
        double readAngle = encoder.getAngle();
        double index = readAngle*360./calibrationSteps;
        double current = calibrationOffsets[int(floor(index)) % calibrationSteps];
        double next = calibrationOffsets[int(ceil(index)) % calibrationSteps];
        double offset = lerp(current, next, mod(index, 1.));
        return mod(readAngle + offset, 360.);
    }
    void run() {
        if (calibrationRunning) runCalibration();
        else if (enabled && millis() - lastPoll >= maxPollInterval) {
            setAngle(targetAngle);
            lastPoll = millis();
        }
    }
    void startCalibration() {
        if (!calibrationRunning) {
            calibrationStepperStartOffset = mod(stepper.getAngle() - encoder.getAngle() + 180., 360.) - 180.;
            stepper.setSpeed(calibrationSpeed);
            calibrationRunning = true;
        }
    }
    void stopCalibration() {
        if (calibrationRunning) {
            stepper.stop();
            calibrationRunning = false;
        }
    }
    void resetCalibration() {
        for (int i = 0; i < calibrationSteps; i++) {
            calibrationOffsets[i] = 0.;
        }
    }
    void setCalibrationSpeed(int speed) {
        calibrationSpeed = speed;
        if (calibrationRunning) stepper.setSpeed(calibrationSpeed);
    }
    void runCalibration() {
        double readAngle = encoder.getAngle();
        if (encoder.hasNewData()) {
            double stepperAngle = stepper.getAngle();
            double offset = mod(stepperAngle - readAngle - calibrationStepperStartOffset + 180., 360.) - 180.;
            ESP_LOGI("Calibration", "offset %f", offset);
            float index = readAngle*calibrationSteps/360.;
            int current = int(floor(index)) % calibrationSteps;
            if (calibrationOffsets[current] == 0.) {
                calibrationOffsets[current] = offset;
                ESP_LOGI("Calibration", "current %d", current);
            }
            else {
                int next = int(ceil(index)) % calibrationSteps;
                double fract = mod(index, 1.);
                calibrationOffsets[current] = lerp(calibrationOffsets[current], offset, calibrationDecay - fract * calibrationDecay);
                if (calibrationOffsets[next] != 0.) calibrationOffsets[next] = lerp(calibrationOffsets[next], offset, fract * calibrationDecay);
            }
        }
    }
private:
    uint32_t lastPoll = 0;
};
#endif