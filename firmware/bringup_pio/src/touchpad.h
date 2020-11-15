#pragma once

#include "I2CHelper.h"
#include <Arduino.h>

class Touchpad
{
public:
    typedef enum
    {
        NONE        = 0x0000,
        SINGLE_TAP  = 0x0001,
        PRESS_HOLD  = 0x0002,
        SWIPE_X_M   = 0x0004,
        SWIPE_X_P   = 0x0008,
        SWIPE_Y_P   = 0x0010,
        SWIPE_Y_M   = 0x0020,
        TWO_FING_TAP= 0x0040,
        SCROLL      = 0x0080,
        ZOOM        = 0x0100
    } gesture_t;

    Touchpad(uint8_t addr, uint8_t int_pin) : device_addr(addr), int_pin(int_pin) {}
    void begin();
    void update();
    void printState();
    void printConfig();

    uint32_t gesture;

private:
    static I2CHelper *i2c;
    uint8_t device_addr = 0;
    uint8_t int_pin;

    static int request(uint16_t reg_addr, volatile uint8_t *res);
    static int write(uint16_t reg_addr, uint8_t value);
    static int end_comm_window();
    static void ready();
};
