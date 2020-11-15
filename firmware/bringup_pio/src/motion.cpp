#include "motion.h"
#include <Arduino.h>
#include <Servo.h>

typedef struct
{
    int output_pin;
    Servo servo;
    int min_us;
    int max_us;
} ServoConfig_t;

const int pos_min_us = 450;
const int pos_max_us = 2550;

const int servo_count = 6;
ServoConfig_t servos[servo_count] = {
    {.output_pin = 17, .servo = Servo(), .min_us = pos_min_us, .max_us = pos_max_us},
    {.output_pin = 16, .servo = Servo(), .min_us = pos_min_us, .max_us = pos_max_us},
    {.output_pin = 15, .servo = Servo(), .min_us = pos_min_us, .max_us = pos_max_us},
    {.output_pin = 5, .servo = Servo(), .min_us = pos_min_us, .max_us = pos_max_us},
    {.output_pin = 6, .servo = Servo(), .min_us = pos_min_us, .max_us = pos_max_us},
    {.output_pin = 8, .servo = Servo(), .min_us = pos_min_us, .max_us = pos_max_us},
};

void write(ServoConfig_t *cfg, int value)
{
    if (value < 0)
    {
        value = 0;
    }
    else if (value > 180)
    {
        value = 180;
    }
    value = map(value, 0, 180, cfg->min_us, cfg->max_us);

    cfg->servo.writeMicroseconds(value);
}

void Motion::begin()
{
    for (int i = 0; i < servo_count; i++)
    {
        ServoConfig_t *cfg = &servos[i];
        cfg->servo.attach(cfg->output_pin, cfg->min_us, cfg->max_us);
        write(cfg, 90);
    }
}

void Motion::update()
{
    const int pos_delta = 2;
    const int pos_min = 0;
    const int pos_max = 180;
    static int pos_cur = 90;

    if (false)
    {
        pos_cur = pos_cur >= pos_max ? pos_min : pos_max;

        for (int i = 0; i < servo_count; i++)
        {
            write(&servos[i], pos_cur);
        }
    }
    else
    {
        static bool pos_dir = 1;

        pos_cur = pos_dir ? pos_cur + pos_delta : pos_cur - pos_delta;

        if (pos_cur >= pos_max)
        {
            pos_cur = pos_max;
            pos_dir = 0;
        }
        if (pos_cur <= pos_min)
        {
            pos_cur = pos_min;
            pos_dir = 1;
        }

        for (int i = 0; i < servo_count; i++)
        {
            write(&servos[i], pos_cur);
        }
    }
}
