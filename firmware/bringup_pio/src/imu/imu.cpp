#include "imu.h"

I2CHelper * Imu::i2c;

uint8_t Imu::RegisterRead(uint8_t addr)
{
  uint8_t value;

  i2c->WriteCmd(BMX160_ADDRESS, addr);
  delay(1);
  i2c->ReadAddr(BMX160_ADDRESS, &value, 1);

  return value;
}

void Imu::RegisterWrite(uint8_t addr, uint8_t value)
{
  i2c->WriteByte(BMX160_ADDRESS, addr, value);
}

void Imu::begin()
{
  i2c = I2CHelper::GetInstance();
  i2c->Setup();

  device.SetReadFunc(RegisterRead);
  device.SetWriteFunc(RegisterWrite);

  const int int_pin = 0;
  pinMode(int_pin, INPUT_PULLUP);

  if (!device.begin(BMX160_ACCELRANGE_4G, GYRO_RANGE_250DPS))
  {
    Serial.println("No BMX160 detected.");
    //while (1);
  }
}

void Imu::update()
{
  device.getEvent(&accel, &gyro, &mag);
}

#define ser Serial

void Imu::printState()
{
  /* Display the accel results (acceleration is measured in m/s^2) */
  ser.print("Accel ");
  ser.print("X: "); ser.print(accel.acceleration.x, 4); ser.print("  ");
  ser.print("Y: "); ser.print(accel.acceleration.y, 4); ser.print("  ");
  ser.print("Z: "); ser.print(accel.acceleration.z, 4); ser.print("  ");
  ser.println("m/s^2");

  /* Display the gyro results (gyro data is in g) */
  ser.print("Gyro ");
  ser.print("X: "); ser.print(gyro.gyro.x, 3); ser.print("  ");
  ser.print("Y: "); ser.print(gyro.gyro.y, 3); ser.print("  ");
  ser.print("Z: "); ser.print(gyro.gyro.z, 3); ser.print("  ");
  ser.println("g");

  /* Display the mag results (mag data is in uTesla) */
  ser.print("Mag ");
  ser.print("X: "); ser.print(mag.magnetic.x, 4); ser.print("  ");
  ser.print("Y: "); ser.print(mag.magnetic.y, 4); ser.print("  ");
  ser.print("Z: "); ser.print(mag.magnetic.z, 4); ser.print("  ");
  ser.println("uT");
}

void Imu::printStateRaw()
{
  // NOTE: This format is used by MotionCal (so don't change it)
  ser.print("Raw:");
  ser.print(device.accel_raw.x); ser.print(",");
  ser.print(device.accel_raw.y); ser.print(",");
  ser.print(device.accel_raw.z); ser.print(",");
  ser.print(device.gyro_raw.x); ser.print(",");
  ser.print(device.gyro_raw.y); ser.print(",");
  ser.print(device.gyro_raw.z); ser.print(",");
  ser.print(device.mag_raw.x); ser.print(",");
  ser.print(device.mag_raw.y); ser.print(",");
  ser.print(device.mag_raw.z); 
  ser.println();
}

void Imu::printSettings()
{
  sensor_t accel, gyro, mag;
  device.getSensor(&accel, &gyro, &mag);

  ser.println("------------------------------------");
  ser.println("ACCELEROMETER");
  ser.println("------------------------------------");
  ser.print  ("Sensor:       "); ser.println(accel.name);
  ser.print  ("Driver Ver:   "); ser.println(accel.version);
  ser.print  ("Unique ID:    0x"); ser.println(accel.sensor_id, HEX);
  ser.print  ("Min Delay:    "); ser.print(accel.min_delay); ser.println(" s");
  ser.print  ("Max Value:    "); ser.print(accel.max_value, 4); ser.println(" m/s^2");
  ser.print  ("Min Value:    "); ser.print(accel.min_value, 4); ser.println(" m/s^2");
  ser.print  ("Resolution:   "); ser.print(accel.resolution, 8); ser.println(" m/s^2");
  ser.println("------------------------------------");
  ser.println("");
  ser.println("------------------------------------");
  ser.println("GYROSCOPE");
  ser.println("------------------------------------");
  ser.print  ("Sensor:       "); ser.println(gyro.name);
  ser.print  ("Driver Ver:   "); ser.println(gyro.version);
  ser.print  ("Unique ID:    0x"); ser.println(gyro.sensor_id, HEX);
  ser.print  ("Min Delay:    "); ser.print(accel.min_delay); ser.println(" s");
  ser.print  ("Max Value:    "); ser.print(gyro.max_value); ser.println(" g");
  ser.print  ("Min Value:    "); ser.print(gyro.min_value); ser.println(" g");
  ser.print  ("Resolution:   "); ser.print(gyro.resolution); ser.println(" g");
  ser.println("------------------------------------");
  ser.println("");
  ser.println("------------------------------------");
  ser.println("MAGNETOMETER");
  ser.println("------------------------------------");
  ser.print  ("Sensor:       "); ser.println(mag.name);
  ser.print  ("Driver Ver:   "); ser.println(mag.version);
  ser.print  ("Unique ID:    0x"); ser.println(mag.sensor_id, HEX);
  ser.print  ("Min Delay:    "); ser.print(accel.min_delay); ser.println(" s");
  ser.print  ("Max Value:    "); ser.print(mag.max_value); ser.println(" uTesla");
  ser.print  ("Min Value:    "); ser.print(mag.min_value); ser.println(" uTesla");
  ser.print  ("Resolution:   "); ser.print(mag.resolution); ser.println(" uTesla");
  ser.println("------------------------------------");
  ser.println("");
}
