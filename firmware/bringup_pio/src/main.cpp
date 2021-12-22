#include "imu/imu.h"
#include "USRF.h"
#include "touchpad.h"
#include "LedMouth.h"
#include "sound/sound.h"
#include "motion.h"
#include "SdFatHelper.h"

#include <bluefruit.h>
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

Motion motion = Motion();
timed_task_t servo_task = {.tick_interval = 30, .tick_count = 0};

// *********************************************************************
// IMU
// *********************************************************************

//const int IMU_INT_PIN = 0;
//const int IMU_ADDR = 0x00;
//const int IMU_BUS = 0x00;

Imu imu = Imu();
timed_task_t imu_task = {.tick_interval = 50, .tick_count = 0};

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

LedMouth ledm = LedMouth();
timed_task_t ledm_task = {.tick_interval = 1000, .tick_count = 0};

// *********************************************************************
// Printer
// *********************************************************************

void print_stuff()
{
  imu.printStateUni();
  usrf.printState();
  touch.printState();
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
// SD/FAT file stuff
// *********************************************************************

#include <ArduinoJson.h>
StaticJsonDocument<512> calibJSON;

SdFatHelper sdfat;

bool printFile(const char *filename) 
{

  File file;
  bool success = sdfat.open_read(filename, file);
  if (!file) {
    Serial.println(F("Failed to read file"));
    return false;
  }

  Serial.println("------------");
  while (file.available()) {
    Serial.write(file.read());
  }
  Serial.println("\n------------");
  sdfat.close(file);
  yield();
  return true;
}

/**! XYZ vector of offsets for zero-g, in m/s^2 */
float accel_zerog[3] = {0, 0, 0};

/**! XYZ vector of offsets for zero-rate, in rad/s */
float gyro_zerorate[3] = {0, 0, 0};

/**! XYZ vector of offsets for hard iron calibration (in uT) */
float mag_hardiron[3] = {0, 0, 0};

/**! The 3x3 matrix for soft-iron calibration (unitless) */
float mag_softiron[9] = {1, 0, 0, 0, 1, 0, 0, 0, 1};

/**! The magnetic field magnitude in uTesla */
float mag_field = 50;

bool saveFile(const char *filename)
{
  File file;
  bool success = sdfat.open_new(filename, file);
  if (!file) {
    Serial.println(F("Failed to create file"));
    return false;
  }

  JsonObject root = calibJSON.to<JsonObject>();
  JsonArray mag_hard_data = root.createNestedArray("mag_hardiron");
  for (int i = 0; i < 3; i++) {
    mag_hard_data.add(mag_hardiron[i]);
  }
  JsonArray mag_soft_data = root.createNestedArray("mag_softiron");
  for (int i = 0; i < 9; i++) {
    mag_soft_data.add(mag_softiron[i]);
  }
  root["mag_field"] = mag_field;
  JsonArray gyro_zerorate_data = root.createNestedArray("gyro_zerorate");
  for (int i = 0; i < 3; i++) {
    gyro_zerorate_data.add(gyro_zerorate[i]);
  }
  JsonArray accel_zerog_data = root.createNestedArray("accel_zerog");
  for (int i = 0; i < 3; i++) {
    accel_zerog_data.add(accel_zerog[i]);
  }
  // serializeJsonPretty(root, Serial);

  // Serialize JSON to file
  if (serializeJson(calibJSON, file) == 0) {
    Serial.println(F("Failed to write to file"));
    return false;
  }
  
  sdfat.close(file);

  return true;
}

// *********************************************************************
// Bluetooth stuff
// *********************************************************************

//BLEDfu bledfu;  // OTA DFU service
BLEUart bleuart;

void startAdv(void)
{
  // Advertising packet
  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
  Bluefruit.Advertising.addTxPower();

  // Include the BLE UART (AKA 'NUS') 128-bit UUID
  Bluefruit.Advertising.addService(bleuart);

  // Secondary Scan Response packet (optional)
  // Since there is no room for 'Name' in Advertising packet
  Bluefruit.ScanResponse.addName();

  /* Start Advertising
   * - Enable auto advertising if disconnected
   * - Interval:  fast mode = 20 ms, slow mode = 152.5 ms
   * - Timeout for fast mode is 30 seconds
   * - Start(timeout) with timeout = 0 will advertise forever (until connected)
   * 
   * For recommended advertising interval
   * https://developer.apple.com/library/content/qa/qa1931/_index.html   
   */
  Bluefruit.Advertising.restartOnDisconnect(true);
  Bluefruit.Advertising.setInterval(32, 244); // in unit of 0.625 ms
  Bluefruit.Advertising.setFastTimeout(30);   // number of seconds in fast mode
  Bluefruit.Advertising.start(0);             // 0 = Don't stop advertising after n seconds
}

void BLE_init()
{
  Bluefruit.begin();
  Bluefruit.setTxPower(4); // Check bluefruit.h for supported values
  Bluefruit.setName("OttoRemix");

  // To be consistent OTA DFU should be added first if it exists
  //bledfu.begin();

  // Configure and start the BLE Uart service
  bleuart.begin();

  // Set up and start advertising
  startAdv();
}

#define READ_BUFSIZE (20)
uint8_t packetbuffer[READ_BUFSIZE + 1];

uint8_t readPacket(BLEUart *ble_uart, uint16_t timeout)
{
  uint16_t origtimeout = timeout, replyidx = 0;

  memset(packetbuffer, 0, READ_BUFSIZE);

  while (timeout--)
  {
    if (replyidx >= 20)
      break;

    while (ble_uart->available())
    {
      char c = ble_uart->read();
      if (c == '!')
      {
        replyidx = 0;
      }
      packetbuffer[replyidx] = c;
      replyidx++;
      timeout = origtimeout;
    }

    if (timeout == 0)
      break;
    delay(1);
  }

  packetbuffer[replyidx] = 0; // null term

  if (!replyidx) // no data or timeout
    return 0;
  if (packetbuffer[0] != '!') // doesn't start with '!' packet beginning
    return 0;

  // check checksum!
  uint8_t xsum = 0;
  uint8_t checksum = packetbuffer[replyidx - 1];

  for (uint8_t i = 0; i < replyidx - 1; i++)
  {
    xsum += packetbuffer[i];
  }
  xsum = ~xsum;

  // Throw an error message if the checksum's don't match
  if (xsum != checksum)
  {
    Serial.print("Checksum mismatch in packet : ");
    //printHex(packetbuffer, replyidx + 1);
    return 0;
  }

  // checksum passed!
  return replyidx;
}

void BLE_update()
{
  uint8_t len = readPacket(&bleuart, 500);
  if (len == 0)
    return;
}

// *********************************************************************
// app tasks
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
    imu.update();
  }

  if (task_ready(&usrf_task))
  {
    usrf.getEvent(&usrf_dist);
  }

  if (task_ready(&touch_task))
  {
    touch.update();
  }

  if (task_ready(&ledm_task))
  {
    ledm.update();
  }

  if (task_ready(&servo_task))
  {
    motion.update();
  }

  if (task_ready(&print_task))
  {
    print_stuff();
  }

  count++;
}

// *********************************************************************
// setup and loop
// *********************************************************************

SheetMusic music;

void setup()
{
  pinMode(HB_PIN, OUTPUT);
  digitalWrite(HB_PIN, LOW);

  Serial.begin(115200);
  while (!Serial) delay(50);
  delay(100);
  Serial.println("Starting...");
  delay(100);

  sdfat.begin();
  sdfat.printFS();
  //saveFile("sensor_calib.json");
  printFile("sensor_calib.json");


  imu.begin();

#if 0
  usrf.begin();
  touch.begin();
  ledm.begin();
  motion.begin();

  // SheetMusicCatalog::LoadMarioTheme(&music, 5, 90);
  music.Restart();

  // task_timer.begin(task_timer_interval_ms, update_tasks);
  task_timer.start();

#endif
}

uint32_t loop_interval_us = 100;

void loop()
{
  static uint32_t loop_count = 0;
  static uint32_t loop_curr_us = 0;
  static uint32_t loop_prev_us = 0;

  imu.update();
  //imu.printStateUni();
  //imu.printStateRaw();
  delay(10);

#if 0

  // attempt to run loop at specific interval
  loop_prev_us = loop_curr_us;
  loop_curr_us = micros();
  int32_t loop_delay_us = loop_interval_us - (loop_curr_us - loop_prev_us);
  if (loop_delay_us > 0 && loop_delay_us < loop_interval_us)
  {
    delayMicroseconds(loop_delay_us);
  }

  // music update "thread"
  static uint32_t music_next_us = 0;
  if (loop_curr_us >= music_next_us)
  {
    MusicalNote note;
    if (music.GetNextNote(&note))
    {
      speaker.play(note.freq_Hz, note.duration_ms);
      music_next_us = loop_curr_us + (note.duration_ms * 1000);
    }
    else
    {
      music.Restart();
      music_next_us = loop_curr_us + (5 * 1000 * 1000);
    }
  }

#endif

  loop_count++;
}
