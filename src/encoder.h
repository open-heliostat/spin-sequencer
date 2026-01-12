#ifndef ENCODER_H
#define ENCODER_H

#include <Arduino.h>
#include <Wire.h>

class Encoder
{
public:
    double angle;
    double offset = 0;
    bool invert = false;
    bool error = false;
    bool devicePresent = true;
    Encoder(int _SDA = SDA, int _SCL = SCL, TwoWire &I2C_ = Wire) : I2C(I2C_), SDA_pin(_SDA), SCL_pin(_SCL) {}
    void begin() {
        if (initialized) return;
        initialized = true;
        I2C.begin(SDA_pin, SCL_pin);
        I2C.setClock(100000);       // keep I2C conservative on newer cores
        I2C.setTimeOut(50);
    }
    double getAngle() {
        if (!initialized) begin();
        update();
        return angle;
    }
    bool hasNewData() {
        return newData && millis() - lastPoll <= maxPollInterval;
    }
    bool update() {
        uint32_t now = millis();
        uint32_t interval = devicePresent ? maxPollInterval : missingBackoff;
        if (now - lastPoll >= interval) {
            int value = readEncoder();
            lastPoll = now;
            if (value > 0) {
                angle = value*360./16384.;
                if (invert) angle = 360. - angle;
                angle = mod(angle + offset, 360.);
                newData = true;
                error = false;
                devicePresent = true;
                missingBackoff = initialMissingBackoff;
                return true;
            }
            else {
                if (now - lastErrorLog >= errorLogInterval) {
                    Serial.printf("Bad I2C Data : %d, Delay : %d\n", value, millis()-now);
                    lastErrorLog = now;
                }
                newData = false;
                error = true;
                devicePresent = false;
                if (missingBackoff < maxMissingBackoff) {
                    missingBackoff = min(missingBackoff * 2, maxMissingBackoff);
                }
            }
        }
        return newData;
    }
    int readEncoder() {
        if (!initialized) begin();
        byte buff[3];

        // First try: repeated start (preferred on newer cores)
        I2C.beginTransmission(0x06);
        I2C.write(0x02);
        if (I2C.endTransmission(false) == 0) {
            int available = I2C.requestFrom(0x06, 3, (uint8_t)true);
            if (available == 3) {
                I2C.readBytes(buff, 3);
                return (256 * buff[1] + buff[2]) / 4;
            }
        }

        // Fallback: issue a stop then a fresh read
        I2C.beginTransmission(0x06);
        I2C.write(0x02);
        if (I2C.endTransmission(true) != 0) return -1;

        int available = I2C.requestFrom(0x06, 3, (uint8_t)true);
        if (available == 3) {
            I2C.readBytes(buff, 3);
            return (256 * buff[1] + buff[2]) / 4;
        }

        return -1;
    }
private:
    TwoWire &I2C;
    int SDA_pin;
    int SCL_pin;
    bool initialized = false;
    uint32_t maxPollInterval = 20;
    const uint32_t initialMissingBackoff = 200;
    uint32_t missingBackoff = initialMissingBackoff;
    const uint32_t maxMissingBackoff = 2000;
    const uint32_t errorLogInterval = 1000;
    uint32_t lastErrorLog = 0;
    uint32_t lastPoll = 0;
    bool newData = false;
    double mod(double a, double N) {return a - N*floor(a/N);}
};
#endif