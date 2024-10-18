#ifndef ENCODER_H
#define ENCODER_H

#include <Arduino.h>
#include <Wire.h>

class Encoder
{
public:
    double angle;
    Encoder(int SDA = SDA, int SCL = SCL, TwoWire &I2C_ = Wire) : I2C(I2C_) {
        I2C.begin(SDA, SCL);
    }
    bool update() {
        uint32_t now = millis();
        if (now - lastPoll >= maxPollInterval) {
            int value = readEncoder();
            lastPoll = now;
            if (value) {
                angle = value*360./4096.;
                return true;
            }
        }
        return false;
    }
    int readEncoder() {
        int available = I2C.requestFrom(0x06, 3);
        if (available > 2) {
            byte buff[3];
            I2C.beginTransmission(0x06);  
            I2C.write(0x02);  // set register for read
            I2C.endTransmission();
            I2C.readBytes(buff, 3);
            int value = ((256 * buff[1] + buff[2]))/4;
            return value;
        }
        else return -1;
    }
private:
    TwoWire &I2C;
    uint32_t maxPollInterval = 1000;
    uint32_t lastPoll = 0;
};
#endif