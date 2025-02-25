#ifndef MOTORCONTROLLER
#define MOTORCONTROLLER
#include "tmcdriver.h"
#include "encoder.h"
struct MotorController {
    TMC5160Controller &controller;
    Encoder &sensor;
    bool pingPong = false;
    bool isMoving = false;
    double stopPosition = 180;
    bool ping = false;
    bool hasChangedState = false;
    double dist = 200;
    double speed = 0;
    MotorController(TMC5160Controller &controller, Encoder &sensor) : controller {controller} , sensor {sensor} {}
    void tick() {
        hasChangedState = false;
        if (isMoving != (controller.move() != 0)) {
            isMoving = controller.move() != 0;
            hasChangedState = true;
        }
        if (hasChangedState && pingPong && !isMoving) {
            double offset = mod(sensor.getAngle() - stopPosition + 180. + (ping ? dist/2. : -dist/2.), 360.) - 180.;
            move((ping ? dist : -dist) - offset);
            ping = !ping;
        }
    }
    void setPingPong(bool p) {
        pingPong = p;
        if (p) {
            speed = 0;
            setPosition(stopPosition - (ping ? dist/2. : -dist/2.));
        }
        else {
            speed = 0;
            setPosition(stopPosition);
        }
    }
    void setPosition(double angle) {
        double curPos = sensor.getAngle();
        // double nextPos = getDistanceToStop() + curPos;
        double toGo = angle - curPos;
        if (speed == 0) toGo = mod(toGo+180., 360.) - 180.;
        else if (speed > 0) toGo = mod(toGo, 360.);
        else toGo = -mod(-toGo, 360.);
        move(toGo);
    }
    double getPosition() {
        return sensor.getAngle();
    }
    void move(double toGo) {
        controller.moveR(toGo*200./360.);
    }
    double getDistanceToStop() {
        double toStop = controller.getStepsToStop() * 360. / 200.;
        return toStop * (controller.getSpeed() > 0 ? 1. : -1.);
    }
    void stop() {
        // controller.stop();
        if (pingPong) {
            speed = 0;
            setPosition(stopPosition);
            pingPong = false;
        }
        else {
            double curPos = sensor.getAngle();
            bool direction = controller.getSpeed() > 0;
            double toGo = direction ? stopPosition - curPos : curPos - stopPosition;
            toGo = mod(toGo, 360.);
            double toStop = controller.getStepsToStop() * 360. / 200.;
            double diff = toStop - toGo;
            if (diff > 0.) {
                toGo += ceil(diff/360.)*360.;
            }
            move(toGo*(direction ? 1. : -1.));
        }
    }
    float mod(float a, float N) {return a - N*floor(a/N);}
};
#endif