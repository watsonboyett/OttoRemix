#pragma once

#include <Adafruit_Sensor.h>
#include <Arduino.h>

class USRF : public Adafruit_Sensor
{
public:
    USRF(int trig_pin, int echo_pin) : trig_pin(trig_pin), echo_pin(echo_pin) {}

    bool begin();
    bool getEvent(sensors_event_t *dist);
    void getSensor(sensor_t *dist);
    void printState();

private:
    const float conv_factor = 0.01724f; // 1/58, per datasheet (assumes speed of sound is 340m/s and accounts for reflection distance)
    const int timeout_us = 23200;       // 400 cm * 58

    int trig_pin = -1;
    int echo_pin = -1;

    sensors_event_t latest_event;
};
