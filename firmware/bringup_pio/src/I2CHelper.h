#pragma once

#include <Arduino.h>
#include <Wire.h>

class I2CHelper
{

public:
    static I2CHelper *GetInstance();

    void Setup();

    uint8_t WriteAddr(uint8_t slaveAddr);

    uint8_t WriteCmd(uint8_t slaveAddr, uint8_t cmd);

    uint8_t WriteByte(uint8_t slaveAddr, uint8_t regAddr, uint8_t cmd);

    bool ReadAddr(uint8_t slaveAddr, uint8_t *data, uint8_t byteCount);

    bool ReadByte(uint8_t slaveAddr, uint8_t regAddr, uint8_t *data);

    bool ReadBytes(uint8_t slaveAddr, uint8_t regAddr, uint8_t *data, uint8_t byteCount);

private:
    TwoWire *port;
    bool isConfigured = false;
};
