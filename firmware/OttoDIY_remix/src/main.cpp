//--------------------------------------------------------------------------------------------------------------------------------------------------------------------
//-- Otto DIY APP Firmware Version 11 (V11) with standard baudrate of 9600 for Bluetooth BLE modules.
//-- Designed to work with the starter Otto or PLUS or other biped robots.
//-- Otto DIY invests time and resources providing open source code and hardware,  please support by purchasing kits from (https://www.ottodiy.com)
//-----------------------------------------------------------------
//-- If you wish to use this software under Open Source Licensing, you must contribute all your source code to the community and all text above must be included in any redistribution
//-- in accordance with the GPL Version 2 when your application is distributed. See http://www.gnu.org/copyleft/gpl.html
//-------------------------------------------------------------------------
// 2021/06  Nca78 - Adapted from Otto App sketch to run on Ottoky with either ESP32 bluetooth or bluetooth module

//#include <EEPROM.h>

#include "Oscillator.h"
#include "SerialCommand.h"
#include "Otto.h"
#include <bluefruit.h>

Otto otto;

// Uncomment the #define below to use internal bluetooth function of the ESP32
//  Bluetooth name will be "OTTOKY"
// /!\ WARNING this is only compatible with Android and not iOS devices, iOS compatible version will comme later...
// If line is uncommented then the sonar is replaced with bluetooth module, RX must be connected on IN2 and TX on IN3
//#define USE_INTERNAL_BT 1

#define N_SERVOS 4
#define PIN_YL 6 //Left leg
#define PIN_YR 16  //Right leg
#define PIN_RL 9 //Left foot
#define PIN_RR 15 //Right foot

#define PIN_Buzzer 1      //BUZZER pin
#define PIN_NoiseSensor 7 // IN 1
#define PIN_Button 4       // BTN B

#define PIN_Trigger 19 // Sonar Trigger
#define PIN_Echo 18    // Sonar Echo

// *********************************************************************
// BLE
// *********************************************************************

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

// *********************************************************************
// Serial Parsing
// *********************************************************************

SerialCommand SCmd(bleuart);

const char programID[] = "Otto_APP_V11";
const char name_fac = '$';
const char name_fir = '#';
int T = 1000;
int moveId = 0;
int moveSize = 15;
volatile bool buttonPushed = false;

int randomDance = 0;
int randomSteps = 0;
bool obstacleDetected = false;
unsigned long int matrix;
void obstacleDetector()
{
  int distance = 0; //otto.getDistance();
  if (distance < 15)
    obstacleDetected = true;
  else
    obstacleDetected = false;
}
void sendAck()
{
  delay(30);
  bleuart.print(F("&&"));
  bleuart.print(F("A"));
  bleuart.println(F("%%"));
  //bleuart.flush();
}
void sendFinalAck()
{
  delay(30);
  bleuart.print(F("&&"));
  bleuart.print(F("F"));
  bleuart.println(F("%%"));
  //bleuart.flush();
}
void receiveReset()
{
  sendAck();
  otto.setRestState(false);
  otto.home();
  otto.setRestState(true);
  sendFinalAck();
}
void receiveUnknown()
{
  Serial.println("Unknown command.");
}
void receiveStop()
{
  sendAck();
  otto.home();
  sendFinalAck();
}
void receiveLED()
{
  sendAck();
  otto.home();
  unsigned long int matrix;
  char *arg;
  char *endstr;
  arg = SCmd.next();
  if (arg != NULL)
  {
    matrix = strtoul(arg, &endstr, 2);
    otto.putMouth(matrix, false);
  }
  else
  {
    otto.putMouth(xMouth);
    delay(2000);
    otto.clearMouth();
  }
  sendFinalAck();
}
void recieveBuzzer()
{
  sendAck();
  otto.home();
  bool error = false;
  int frec;
  int duration;
  char *arg;
  arg = SCmd.next();
  if (arg != NULL)
    frec = atoi(arg);
  else
    error = true;
  arg = SCmd.next();
  if (arg != NULL)
    duration = atoi(arg);
  else
    error = true;
  if (error == true)
  {
    otto.putMouth(xMouth);
    delay(2000);
    otto.clearMouth();
  }
  else
    otto._tone(frec, duration, 1);
  sendFinalAck();
}
void receiveTrims()
{
  sendAck();
  otto.home();
  int trim_YL, trim_YR, trim_RL, trim_RR;
  bool error = false;
  char *arg;
  arg = SCmd.next();
  if (arg != NULL)
    trim_YL = atoi(arg);
  else
    error = true;
  arg = SCmd.next();
  if (arg != NULL)
    trim_YR = atoi(arg);
  else
    error = true;
  arg = SCmd.next();
  if (arg != NULL)
    trim_RL = atoi(arg);
  else
    error = true;
  arg = SCmd.next();
  if (arg != NULL)
    trim_RR = atoi(arg);
  else
    error = true;
  if (error == true)
  {
    otto.putMouth(xMouth);
    delay(2000);
    otto.clearMouth();
  }
  else
  {
    otto.setTrims(trim_YL, trim_YR, trim_RL, trim_RR);
    otto.saveTrimsOnEEPROM();
  }
  sendFinalAck();
}
void receiveServo()
{
  sendAck();
  moveId = 30;
  bool error = false;
  char *arg;
  int servo_YL, servo_YR, servo_RL, servo_RR;
  arg = SCmd.next();
  if (arg != NULL)
    servo_YL = atoi(arg);
  else
    error = true;
  arg = SCmd.next();
  if (arg != NULL)
    servo_YR = atoi(arg);
  else
    error = true;
  arg = SCmd.next();
  if (arg != NULL)
    servo_RL = atoi(arg);
  else
    error = true;
  arg = SCmd.next();
  if (arg != NULL)
  {
    servo_RR = atoi(arg);
  }
  else
    error = true;
  if (error == true)
  {
    otto.putMouth(xMouth);
    delay(2000);
    otto.clearMouth();
  }
  else
  {
    int servoPos[4] = {servo_YL, servo_YR, servo_RL, servo_RR};
    otto._moveServos(200, servoPos);
  }
  sendFinalAck();
}

void move(int moveId)
{
  bool manualMode = false;
  switch (moveId)
  {
  case 0:
    otto.home();
    break;
  case 1:
    otto.walk(1, T, 1);
    break;
  case 2:
    otto.walk(1, T, -1);
    break;
  case 3:
    otto.turn(1, T, 1);
    break;
  case 4:
    otto.turn(1, T, -1);
    break;
  case 5:
    otto.updown(1, T, moveSize);
    break;
  case 6:
    otto.moonwalker(1, T, moveSize, 1);
    break;
  case 7:
    otto.moonwalker(1, T, moveSize, -1);
    break;
  case 8:
    otto.swing(1, T, moveSize);
    break;
  case 9:
    otto.crusaito(1, T, moveSize, 1);
    break;
  case 10:
    otto.crusaito(1, T, moveSize, -1);
    break;
  case 11:
    otto.jump(1, T);
    break;
  case 12:
    otto.flapping(1, T, moveSize, 1);
    break;
  case 13:
    otto.flapping(1, T, moveSize, -1);
    break;
  case 14:
    otto.tiptoeSwing(1, T, moveSize);
    break;
  case 15:
    otto.bend(1, T, 1);
    break;
  case 16:
    otto.bend(1, T, -1);
    break;
  case 17:
    otto.shakeLeg(1, T, 1);
    break;
  case 18:
    otto.shakeLeg(1, T, -1);
    break;
  case 19:
    otto.jitter(1, T, moveSize);
    break;
  case 20:
    otto.ascendingTurn(1, T, moveSize);
    break;
  default:
    manualMode = true;
    break;
  }
  if (!manualMode)
    sendFinalAck();
}
void receiveMovement()
{
  sendAck();
  if (otto.getRestState() == true)
    otto.setRestState(false);
  char *arg;
  arg = SCmd.next();
  if (arg != NULL)
    moveId = atoi(arg);
  else
  {
    otto.putMouth(xMouth);
    delay(2000);
    otto.clearMouth();
    moveId = 0;
  }
  arg = SCmd.next();
  if (arg != NULL)
    T = atoi(arg);
  else
    T = 1000;
  arg = SCmd.next();
  if (arg != NULL)
    moveSize = atoi(arg);
  else
    moveSize = 15;

    move(moveId);
}
void receiveGesture()
{
  sendAck();
  otto.home();
  int gesture = 0;
  char *arg;
  arg = SCmd.next();
  if (arg != NULL)
    gesture = atoi(arg);
  else
    delay(2000);
  switch (gesture)
  {
  case 1:
    otto.playGesture(OttoHappy);
    break;
  case 2:
    otto.playGesture(OttoSuperHappy);
    break;
  case 3:
    otto.playGesture(OttoSad);
    break;
  case 4:
    otto.playGesture(OttoSleeping);
    break;
  case 5:
    otto.playGesture(OttoFart);
    break;
  case 6:
    otto.playGesture(OttoConfused);
    break;
  case 7:
    otto.playGesture(OttoLove);
    break;
  case 8:
    otto.playGesture(OttoAngry);
    break;
  case 9:
    otto.playGesture(OttoFretful);
    break;
  case 10:
    otto.playGesture(OttoMagic);
    break;
  case 11:
    otto.playGesture(OttoWave);
    break;
  case 12:
    otto.playGesture(OttoVictory);
    break;
  case 13:
    otto.playGesture(OttoFail);
    break;
  default:
    break;
  }
  sendFinalAck();
}
void receiveSing()
{
  sendAck();
  otto.home();
  int sing = 0;
  char *arg;
  arg = SCmd.next();
  if (arg != NULL)
    sing = atoi(arg);
  else
    delay(2000);
  switch (sing)
  {
  case 1:
    otto.sing(S_connection);
    break;
  case 2:
    otto.sing(S_disconnection);
    break;
  case 3:
    otto.sing(S_surprise);
    break;
  case 4:
    otto.sing(S_OhOoh);
    break;
  case 5:
    otto.sing(S_OhOoh2);
    break;
  case 6:
    otto.sing(S_cuddly);
    break;
  case 7:
    otto.sing(S_sleeping);
    break;
  case 8:
    otto.sing(S_happy);
    break;
  case 9:
    otto.sing(S_superHappy);
    break;
  case 10:
    otto.sing(S_happy_short);
    break;
  case 11:
    otto.sing(S_sad);
    break;
  case 12:
    otto.sing(S_confused);
    break;
  case 13:
    otto.sing(S_fart1);
    break;
  case 14:
    otto.sing(S_fart2);
    break;
  case 15:
    otto.sing(S_fart3);
    break;
  case 16:
    otto.sing(S_mode1);
    break;
  case 17:
    otto.sing(S_mode2);
    break;
  case 18:
    otto.sing(S_mode3);
    break;
  case 19:
    otto.sing(S_buttonPushed);
    break;
  default:
    break;
  }
  sendFinalAck();
}
void receiveName()
{
  sendAck();
  otto.home();
  char newOttoName[11] = "";
  int eeAddress = 5;
  char *arg;
  arg = SCmd.next();
  if (arg != NULL)
  {
    int k = 0;
    while ((*arg) && (k < 11))
    {
      newOttoName[k] = *arg++;
      k++;
    }
    //EEPROM.put(eeAddress, newOttoName);
  }
  else
  {
    delay(2000);
  }
  sendFinalAck();
}
void requestName()
{
  otto.home();
  char actualOttoName[11] = "OttoName";
  int eeAddress = 5;
  //EEPROM.get(eeAddress, actualOttoName);
  bleuart.print(F("&&"));
  bleuart.print(F("E "));
  bleuart.print(actualOttoName);
  bleuart.println(F("%%"));
  bleuart.flush();
}
void requestDistance()
{
  otto.home();
  int distance = 0; //otto.getDistance();
  bleuart.print(F("&&"));
  bleuart.print(F("D "));
  bleuart.print(distance);
  bleuart.println(F("%%"));
  bleuart.flush();
}
void requestNoise()
{
  otto.home();
  int microphone = 0; //otto.getNoise();
  bleuart.print(F("&&"));
  bleuart.print(F("N "));
  bleuart.print(microphone);
  bleuart.println(F("%%"));
  bleuart.flush();
}
void requestProgramId()
{
  otto.home();
  bleuart.print(F("&&"));
  bleuart.print(F("I "));
  bleuart.print(programID);
  bleuart.println(F("%%"));
  bleuart.flush();
}

void ButtonPushed()
{
  if (!buttonPushed)
  {
    buttonPushed = true;
    otto.putMouth(smallSurprise);
  }
}


// *********************************************************************
// setup + loop
// *********************************************************************

const int HB_PIN = 3;

void toggle_heartbeat()
{
  static int hb_state = 0;

  digitalWrite(HB_PIN, hb_state);
  hb_state = !hb_state;
}

void setup()
{
  pinMode(HB_PIN, OUTPUT);
  digitalWrite(HB_PIN, LOW);

  Serial.begin(115200);
  delay(100);
  Serial.println("Starting...");
  delay(100);


  BLE_init();

  otto.init(PIN_YL, PIN_YR, PIN_RL, PIN_RR, true, PIN_Buzzer); //, PIN_NoiseSensor, PIN_Buzzer, PIN_Trigger, PIN_Echo);
  otto.initMATRIX(); //DIN_PIN, CS_PIN, CLK_PIN, LED_DIRECTION);
  otto.matrixIntensity(2);
  //randomSeed(analogRead(PIN_NoiseSensor));
  // not available on Ottoky // pinMode(PIN_ASSEMBLY, INPUT_PULLUP);
  pinMode(PIN_Button, INPUT);

  SCmd.addCommand("X", receiveReset);
  SCmd.addCommand("S", receiveStop);
  SCmd.addCommand("L", receiveLED);
  SCmd.addCommand("T", recieveBuzzer);
  SCmd.addCommand("M", receiveMovement);
  SCmd.addCommand("H", receiveGesture);
  SCmd.addCommand("K", receiveSing);
  SCmd.addCommand("C", receiveTrims);
  SCmd.addCommand("G", receiveServo);
  SCmd.addCommand("R", receiveName);
  SCmd.addCommand("E", requestName);
  SCmd.addCommand("D", requestDistance);
  SCmd.addCommand("N", requestNoise);
  SCmd.addCommand("I", requestProgramId);
  SCmd.addDefaultHandler(receiveUnknown);

  //otto.sing(S_connection);
  otto.home();
  // for (int i = 0; i < 2; i++)
  // {
  //   for (int i = 0; i < 8; i++)
  //   {
  //     otto.putAnimationMouth(littleUuh, i);
  //     delay(150);
  //   }
  // }
  otto.putMouth(zero);
  //otto.sing(S_happy);
  delay(200);

  // if (EEPROM.read(5) == name_fir)
  // {
  //   otto.jump(1, 700);
  //   delay(200);
  //   otto.shakeLeg(1, T, 1);
  //   otto.putMouth(smallSurprise);
  //   otto.swing(2, 800, 20);
  //   otto.home();
  // }
  // otto.putMouth(happyOpen);

  /* not available on Ottoky // 
while (digitalRead(PIN_ASSEMBLY) == LOW) {
  otto.home();
  otto.sing(S_happy_short);   // sing every 5 seconds so we know OTTO is still working
delay(5000);}
 not available on Ottoky */

  Serial.println("Init Done.");
}

void loop()
{
  static uint32_t loop_count = 0;
  if (loop_count % (1 << 8) == 0)
  {
    toggle_heartbeat();
  }

  SCmd.readSerial();
}
