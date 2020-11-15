#include "imu/imu.h"
#include "USRF.h"
#include "touchpad.h"
#include "LedMatrix.h"
#include "sound/sound.h"

#include <Servo.h>
#include <Arduino.h>

// *********************************************************************
// Task timer
// *********************************************************************

typedef struct
{
  uint32_t tick_interval;
  uint32_t tick_count;
} timed_task_t;

SoftwareTimer task_timer = SoftwareTimer();

const uint32_t task_timer_interval_ms = 1;

bool task_ready(timed_task_t *task)
{
  bool run_task = false;
  if (++task->tick_count >= task->tick_interval)
  {
    run_task = true;
    task->tick_count = 0;
  }

  return run_task;
}

// *********************************************************************
// sound output
// *********************************************************************

const int BUZZER_PIN = 4;
const int SPEAKER_PIN = 1;

Sound speaker = Sound(SPEAKER_PIN);
Sound buzzer = Sound(BUZZER_PIN);

// *********************************************************************
// servo motors
// *********************************************************************

const int SERVO1_PIN = 17;
const int SERVO2_PIN = 16;
const int SERVO3_PIN = 15;
const int SERVO4_PIN = 5;
const int SERVO5_PIN = 6;
const int SERVO6_PIN = 8;

Servo servo1;
Servo servo2;
Servo servo3;
Servo servo4;
Servo servo5;
Servo servo6;

const int servo_count = 6;
Servo *servos[servo_count] = {&servo1, &servo2, &servo3, &servo4, &servo5, &servo6};

void servo_init()
{
  servo1.attach(SERVO1_PIN);
  servo2.attach(SERVO2_PIN);
  servo3.attach(SERVO3_PIN);
  servo4.attach(SERVO4_PIN);
  servo5.attach(SERVO5_PIN);
  servo6.attach(SERVO6_PIN);
}

void servo_update()
{
  const int pos_delta = 50;
  const int pos_min = 450;
  const int pos_max = 2550;
  static int pos_cur = 0;

  if (false)
  {
    pos_cur = pos_cur >= pos_max ? pos_min : pos_max;

    for (int i = 0; i < servo_count; i++)
    {
      servos[i]->writeMicroseconds(pos_cur);
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
      servos[i]->writeMicroseconds(pos_cur);
    }
  }
}

timed_task_t servo_task = {.tick_interval = 30, .tick_count = 0};

// *********************************************************************
// IMU
// *********************************************************************

//const int IMU_INT_PIN = 0;
//const int IMU_ADDR = 0x00;
//const int IMU_BUS = 0x00;

Imu imu = Imu();
timed_task_t imu_task = {.tick_interval = 20, .tick_count = 0};

// *********************************************************************
// Rangefinder
// *********************************************************************

const int USRF_ECHO_PIN = 18;
const int USRF_TRIG_PIN = 19;

USRF usrf = USRF(USRF_TRIG_PIN, USRF_ECHO_PIN);
timed_task_t usrf_task = {.tick_interval = 200, .tick_count = 0};

// *********************************************************************
// Touchpad
// *********************************************************************

const int TOUCHPAD_RDY_PIN = 7;
const int TOUCHPAD_ADDR = 0x74;

Touchpad touch = Touchpad(TOUCHPAD_ADDR, TOUCHPAD_RDY_PIN);
timed_task_t touch_task = {.tick_interval = 100, .tick_count = 0};

// *********************************************************************
// LED matrix
// *********************************************************************

timed_task_t ledm_task = {.tick_interval = 1000, .tick_count = 0};

// *********************************************************************
// Printer
// *********************************************************************

void print_stuff()
{
  imu.PrintStateRaw();
  usrf.printState();
  touch.PrintState();
}

timed_task_t print_task = {.tick_interval = 500, .tick_count = 0};

// *********************************************************************
// Heartbeat
// *********************************************************************

const int HB_PIN = 3;

void toggle_heartbeat()
{
  static int hb_state = 0;

  digitalWrite(HB_PIN, hb_state);
  hb_state = !hb_state;
}

timed_task_t heatbeat_task = {.tick_interval = 500, .tick_count = 0};

// *********************************************************************
// main app
// *********************************************************************

sensors_event_t usrf_dist;

void update_tasks(TimerHandle_t timerHandle)
{
  static int count = 0;

  if (task_ready(&heatbeat_task))
  {
    toggle_heartbeat();
  }

  if (task_ready(&imu_task))
  {
    //imu.Update();
  }

  if (task_ready(&usrf_task))
  {
    usrf.getEvent(&usrf_dist);
  }

  if (task_ready(&touch_task))
  {
    //touch.Update();
  }

  if (task_ready(&ledm_task))
  {
    ledm_update();
  }

  if (task_ready(&servo_task))
  {
    servo_update();
  }

  if (task_ready(&print_task))
  {
    print_stuff();
  }

  count++;
}

SheetMusic music;

void setup()
{
  pinMode(HB_PIN, OUTPUT);
  digitalWrite(HB_PIN, LOW);

  Serial.begin(38400);
  delay(100);
  Serial.println("Starting...");
  delay(100);

  //I2C_Setup();
  //imu.Setup();
  usrf.begin();
  //touch.Setup();
  ledm_init();
  servo_init();

  SheetMusicCatalog::LoadMarioTheme(&music, 5, 90);
  music.Restart();

  task_timer.begin(task_timer_interval_ms, update_tasks);
  task_timer.start();
}

uint32_t loop_interval_us = 100;

void loop()
{
  static uint32_t loop_count = 0;
  static uint32_t loop_start_us = 0;
  loop_start_us = micros();

  // music update "thread"
  static uint32_t music_next_us = 0;
  if (loop_start_us >= music_next_us)
  {
    MusicalNote note;
    if (music.GetNextNote(&note))
    {
      speaker.play(note.freq_Hz, note.duration_ms);
      music_next_us = loop_start_us + (note.duration_ms * 1000);
    }
    else
    {
      music.Restart();
      music_next_us = loop_start_us + (5 * 1000 * 1000);
    }
  }

  // attempt to run loop at specific interval
  uint32_t loop_end_us = micros();
  int32_t loop_delay_us = loop_interval_us - (loop_end_us - loop_start_us);
  if (loop_delay_us > 0)
  {
    delayMicroseconds(loop_delay_us);
  }

  loop_count++;
}
