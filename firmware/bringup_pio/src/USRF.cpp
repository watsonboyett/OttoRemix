#include "USRF.h"
#include <Arduino.h>

bool USRF::begin()
{
    pinMode(trig_pin, OUTPUT);
    pinMode(echo_pin, INPUT);

    return true;
}

// TODO: use interrupt to prevent from waiting here?
bool USRF::getEvent(sensors_event_t *dist)
{
    // pulse trigger output
    digitalWrite(trig_pin, LOW);
    delayMicroseconds(2);
    digitalWrite(trig_pin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trig_pin, LOW);

    // read the echo input (returns the sound wave travel time in microseconds)
    long duration = pulseIn(echo_pin, HIGH, timeout_us);
    long distance = (float)duration * conv_factor;
    latest_event.distance = distance;
    dist->distance = latest_event.distance;

    return true;
}

void USRF::printState()
{
    Serial.print("Dist: ");
    Serial.print(latest_event.distance);
    Serial.println(" cm");
}

void USRF::getSensor(sensor_t *sensor)
{
    //strncpy(sensor->name, "HC-SR04", sizeof(sensor->name) - 1);
    sensor->version = 1;
    sensor->sensor_id = -1;
    sensor->type = SENSOR_TYPE_PROXIMITY;
    sensor->min_delay = 0.01F;
    sensor->max_value = 400;
    sensor->min_value = 2;
    sensor->resolution = 0.03F;
}
