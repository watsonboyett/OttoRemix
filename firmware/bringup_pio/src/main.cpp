#include "imu.h"
#include "USRF.h"
#include "touchpad.h"
#include "LedMatrix.h"
#include "sound/sound.h"

#include <Servo.h>
#include <Arduino.h>

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

  if (true)
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
      servos[i]->write(pos_cur);
    }
  }
}

// *********************************************************************
// Update timer
// *********************************************************************

typedef struct
{
  uint32_t task_period;
  uint32_t tick_count;
} timer_task_t;

SoftwareTimer t1 = SoftwareTimer();

// *********************************************************************
// IMU
// *********************************************************************

//const int IMU_INT_PIN = 0;
//const int IMU_ADDR = 0x00;
//const int IMU_BUS = 0x00;

Imu imu = Imu();
timer_task_t imu_task = {.task_period = 10, .tick_count = 0};

// *********************************************************************
// Rangefinder
// *********************************************************************

const int USRF_ECHO_PIN = 18;
const int USRF_TRIG_PIN = 19;

USRF usrf = USRF(USRF_TRIG_PIN, USRF_ECHO_PIN);

// *********************************************************************
// Touchpad
// *********************************************************************

const int TOUCHPAD_RDY_PIN = 7;
const int TOUCHPAD_ADDR = 0x74;

Touchpad touch = Touchpad(TOUCHPAD_ADDR, TOUCHPAD_RDY_PIN);

// *********************************************************************
// main app
// *********************************************************************

const int HB_PIN = 3;

SheetMusic music;

void setup()
{
  pinMode(HB_PIN, OUTPUT);
  digitalWrite(HB_PIN, LOW);

  Serial.begin(38400);
  delay(100);
  Serial.println("Starting...");
  delay(100);

  //t1.begin(100, test, )

  //I2C_Setup();
  //imu.Setup();
  //usrf.begin();
  //touch.Setup();
  ledm_init();
  //servo_init();
}

void loop()
{
  static int loop_count = 0;
  static int hb_state = 0;

  if (loop_count % (1UL << 5) == 0)
  {
    digitalWrite(HB_PIN, hb_state);
    hb_state = !hb_state;
  }

  if (false && loop_count % (1UL << 8) == 0)
  {
    SheetMusicCatalog::LoadMarioTheme(&music, 5, 90);

    music.Restart();
    MusicalNote note;
    while (music.GetNextNote(&note))
    {
      speaker.play(note.freq, note.duration);
    }
  }

  //imu.Update();

  // sensors_event_t dist;
  // usrf.getEvent(&dist);

  // touch.Update();

  if (loop_count % (1 << 8) == 0)
  {
    //servo_update();
  }

  if (loop_count % (1 << 7) == 0)
  {
    ledm_update();
  }

  if (loop_count % (1UL << 6) == 0)
  {
    //imu.PrintStateRaw();
    // usrf.printState();
    // touch.PrintState();
  }

  delay(10);
  loop_count++;
}
