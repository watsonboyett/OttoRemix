
#include "touchpad.h"

const int TOUCHPAD_ADDR = 0x74;

int Touchpad::request(uint16_t reg_addr, volatile uint8_t *res)
{
  Wire.beginTransmission(TOUCHPAD_ADDR);
  Wire.write((reg_addr & 0xff00) >> 8);
  Wire.write((reg_addr & 0x00ff));
  int error = Wire.endTransmission();

  Wire.requestFrom(TOUCHPAD_ADDR, 1);
  *res = Wire.read();

  return error;
}

int Touchpad::write(uint16_t reg_addr, uint8_t value)
{
  Wire.beginTransmission(TOUCHPAD_ADDR);
  Wire.write((reg_addr & 0xff00) >> 8);
  Wire.write((reg_addr & 0x00ff));
  Wire.write(value);
  return Wire.endTransmission();
}

int Touchpad::end_comm_window()
{
  return write(0xEEEE, 0x00);
}

volatile bool touchpad_has_data = 0;
volatile uint8_t touchpad_status = 0;
volatile uint8_t touchpad_ges0 = 0;
volatile uint8_t touchpad_ges1 = 0;
volatile uint8_t touchpad_sys0 = 0;

void Touchpad::ready()
{
  touchpad_has_data = 1;

  // read gesture registers from touchpad
  // TODO: we're not actually using the device address here, FIXME
  request(0x0000, &touchpad_status);
  request(0x000D, &touchpad_ges0);
  request(0x000E, &touchpad_ges1);
  request(0x000F, &touchpad_sys0);
  //touchpad_end_comm_window();
}

void Touchpad::PrintConfig()
{
  for (int i = 0; i < 0xFF; i++)
  {
    volatile uint8_t reg;
    request(i, &reg);
    Serial.print(i, HEX);
    Serial.print(": ");
    Serial.println(reg, HEX);
  }

  for (int i = 0x0431; i < 0x43E; i++)
  {
    volatile uint8_t reg;
    request(i, &reg);
    Serial.print(i, HEX);
    Serial.print(": ");
    Serial.println(reg, HEX);
  }

  for (int i = 0x056D; i < 0x0596; i++)
  {
    volatile uint8_t reg;
    request(i, &reg);
    Serial.print(i, HEX);
    Serial.print(": ");
    Serial.println(reg, HEX);
  }

  for (int i = 0x06B7; i < 0x06Cf; i++)
  {
    volatile uint8_t reg;
    request(i, &reg);
    Serial.print(i, HEX);
    Serial.print(": ");
    Serial.println(reg, HEX);
  }
}

I2CHelper *Touchpad::i2c;

void Touchpad::Setup()
{
  i2c = I2CHelper::GetInstance();
  i2c->Setup();

  //  touchpad_write(0x058E, 0xEC);
  //  touchpad_write(0x0431, 0x00);

  pinMode(int_pin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(int_pin), ready, RISING);
}

enum TOUCHPAD_REG_GES_EVNT_0
{
  SINGLE_TAP = 0x01,
  PRESS_HOLD = 0x02,
  SWIPE_X_M = 0x04,
  SWIPE_X_P = 0x08,
  SWIPE_Y_P = 0x10,
  SWIPE_Y_M = 0x20
};

enum TOUCHPAD_REG_GES_EVNT_1
{
  TWO_FING_TAP = 0x01,
  SCROLL = 0x02,
  ZOOM = 0x04
};

void Touchpad::Update()
{
  if (touchpad_has_data)
  {
    touchpad_has_data = 0;

    //    Serial.println(touchpad_ges0, HEX);
    //    Serial.println(touchpad_ges1, HEX);
    //    Serial.println(touchpad_sys0, HEX);

    switch (touchpad_ges0)
    {
    case TOUCHPAD_REG_GES_EVNT_0::SINGLE_TAP:
    {
      gesture |= gesture_t::SINGLE_TAP;
      break;
    }
    case TOUCHPAD_REG_GES_EVNT_0::PRESS_HOLD:
    {
      gesture |= gesture_t::PRESS_HOLD;
      break;
    }
    case TOUCHPAD_REG_GES_EVNT_0::SWIPE_X_M:
    {
      gesture |= gesture_t::SWIPE_X_M;
      break;
    }
    case TOUCHPAD_REG_GES_EVNT_0::SWIPE_X_P:
    {
      gesture |= gesture_t::SWIPE_X_P;
      break;
    }
    case TOUCHPAD_REG_GES_EVNT_0::SWIPE_Y_P:
    {
      gesture |= gesture_t::SWIPE_Y_P;
      break;
    }
    case TOUCHPAD_REG_GES_EVNT_0::SWIPE_Y_M:
    {
      gesture |= gesture_t::SWIPE_Y_M;
      break;
    }
    }

    switch (touchpad_ges1)
    {
    case TOUCHPAD_REG_GES_EVNT_1::TWO_FING_TAP:
    {
      gesture |= gesture_t::TWO_FING_TAP;
      break;
    }
    case TOUCHPAD_REG_GES_EVNT_1::SCROLL:
    {
      gesture |= gesture_t::SCROLL;
      break;
    }
    case TOUCHPAD_REG_GES_EVNT_1::ZOOM:
    {
      gesture |= gesture_t::ZOOM;
      break;
    }
    }

    if (touchpad_ges0 | touchpad_ges1)
    {
      Serial.println("Touch detected: " + gesture);
    }
  }
}

void Touchpad::PrintState()
{
  gesture = gesture_t::NONE;

  if (gesture == NONE)
    return;

  String gesture_str = "";
  switch (gesture)
  {
  case gesture_t::SINGLE_TAP:
  {
    gesture_str += "Single tap.";
    break;
  }
  case gesture_t::PRESS_HOLD:
  {
    gesture_str += "Press + hold.";
    break;
  }
  case gesture_t::SWIPE_X_M:
  {
    gesture_str += "Swipe X-.";
    break;
  }
  case gesture_t::SWIPE_X_P:
  {
    gesture_str += "Swipe X+.";
    break;
  }
  case gesture_t::SWIPE_Y_P:
  {
    gesture_str += "Swipe Y+.";
    break;
  }
  case gesture_t::SWIPE_Y_M:
  {
    gesture_str += "Swipe Y-.";
    break;
  }
  case gesture_t::TWO_FING_TAP:
  {
    gesture_str += "2 Finger tap.";
    break;
  }
  case gesture_t::SCROLL:
  {
    gesture_str += "Scroll.";
    break;
  }
  case gesture_t::ZOOM:
  {
    gesture_str += "Zoom.";
    break;
  }
  }

  Serial.println("Touch detected: " + gesture);
}