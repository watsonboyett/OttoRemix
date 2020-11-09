#include "I2CHelper.h"

I2CHelper i2c0;

I2CHelper *I2CHelper::GetInstance()
{
  return &i2c0;
}

void I2CHelper::Setup()
{
  // TODO: allow for using different ports
  port = &Wire;

  if (!isConfigured)
  {
    port->begin();
    isConfigured = true;
  }
}

uint8_t I2CHelper::WriteAddr(uint8_t slaveAddr)
{
  port->beginTransmission(slaveAddr);
  uint8_t result = port->endTransmission();
  return result;
}

uint8_t I2CHelper::WriteCmd(uint8_t slaveAddr, uint8_t cmd)
{
  port->beginTransmission(slaveAddr);
  port->write(cmd);
  uint8_t result = port->endTransmission();
  return result;
}

uint8_t I2CHelper::WriteByte(uint8_t slaveAddr, uint8_t regAddr, uint8_t cmd)
{
  port->beginTransmission(slaveAddr);
  port->write(regAddr);
  port->write(cmd);
  uint8_t result = port->endTransmission();
  return result;
}

// return True if ACK is retruned from slave, false otherwise
bool I2CHelper::ReadAddr(uint8_t slaveAddr, uint8_t *data, uint8_t byteCount)
{
  uint8_t bytes_rx = port->requestFrom(slaveAddr, byteCount);
  //Serial.printf("received %d bytes\n", bytes_rx);

  if (bytes_rx < byteCount && port->available() < byteCount)
  {
    return false;
  }
  else
  {
    for (int i = 0; i < byteCount; i++)
    {
      data[i] = port->read();
    }
    return true;
  }
}

bool I2CHelper::ReadByte(uint8_t slaveAddr, uint8_t regAddr, uint8_t *data)
{
  return ReadBytes(slaveAddr, regAddr, data, 1);
}

bool I2CHelper::ReadBytes(uint8_t slaveAddr, uint8_t regAddr, uint8_t *data, uint8_t byteCount)
{
  WriteCmd(slaveAddr, regAddr);
  delay(1);
  return ReadAddr(slaveAddr, data, byteCount);
}
