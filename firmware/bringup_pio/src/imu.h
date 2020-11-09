#pragma once

#include <Arduino.h>
#include "I2CHelper.h"
#include "bmx160.h"

class Imu
{
public:
    Imu(){};
    void Setup();
    void Update();
    void PrintSettings();
    void PrintState();
    void PrintStateRaw();

private:
    static I2CHelper *i2c;
    static uint8_t RegisterRead(uint8_t addr);
    static void RegisterWrite(uint8_t addr, uint8_t value);

    BMX160 device = BMX160(0x160A, 0x160B, 0x160C);
    sensors_event_t accel;
    sensors_event_t gyro;
    sensors_event_t mag;
};
