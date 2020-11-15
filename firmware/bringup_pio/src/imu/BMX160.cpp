/*!
 * @file BMX160.cpp
 *
 * @mainpage DPEng BMX160 accel/gyro/mag sensor driver
 *
 * @section intro_sec Introduction
 *
 * This is the documentation for DPEng's BMX160 driver for the
 * Arduino platform.  It is designed specifically to work with the
 * DPEng BMX160 breakout:
 * https://www.ebay.co.uk/itm/323835783928
 * [AMAZON BMX160 BREAKOUT BOARD PRODUCT LINK IS COMING SOON!!!]
 *
 * These sensors use I2C to communicate, 2 pins (SCL+SDA) are required
 * to interface with the breakout. SPI is also possible with <2 MBIT/s speeds.
 *
 * DPEng invests time and resources providing this open source code,
 * please support DPEng and open-source hardware by purchasing
 * products from DPEng!
 *
 * @section dependencies Dependencies
 *
 * This library depends on <a href="https://github.com/adafruit/Adafruit_Sensor">
 * Adafruit_Sensor</a> being present on your system. Please make sure you have
 * installed the latest version before using this library.
 *
 * @section author Author
 *
 * Written by David Pattison for DPEng.
 *
 * @section license License
 *
 * MIT license, all text here must be included in any redistribution.
 *
 */

#include "BMX160.h"
#include <limits.h>

/* -----------------------------------------------------------------------
    REGISTERS
----------------------------------------------------------------------- */

    /*!
        Raw register addresses used to communicate with the sensor.
    */
    typedef enum
    {
    CHIP_ID     = 0x00, // Should return 0xEA
    ERR_REG         = 0x02,  // Bit 7 enable DMP, bit 3 reset DMP
    PMU_STATUS      = 0x03,  // Bit 7 enable DMP, bit 3 reset DMP
    DATA_0      = 0x04,
    DATA_1      = 0x05,
    DATA_2      = 0x06,
    DATA_3      = 0x07,
    DATA_4      = 0x08,
    DATA_5      = 0x09,
    DATA_6      = 0x0A,
    DATA_7      = 0x0B,
    DATA_8      = 0x0C,
    DATA_9      = 0x0D,
    DATA_10       = 0x0E,
    DATA_11       = 0x0F,
    DATA_12       = 0x10,
    DATA_13       = 0x11,
    DATA_14       = 0x12,
    DATA_15       = 0x13,
    DATA_16     = 0x14,
    DATA_17     = 0x15,
    DATA_18       = 0x16,
    DATA_19       = 0x17,
    SENSOERTIME_0 = 0x18, 
    SENSOERTIME_1 = 0x19, 
    SENSOERTIME_2 = 0x1A, 
    SENSOR_STATUS = 0x1B, 
    INT_STATUS_0  = 0x1C, 
    INT_STATUS_1  = 0x1D, 
    INT_STATUS_2  = 0x1E, 
    INT_STATUS_3  = 0x1F,
    TEMPERATURE_0 = 0x20,
    TEMPERATURE_1 = 0x21,
    FIFO_LENGTH_0 = 0x22,
    FIFO_LENGTH_1 = 0x23,
    FIFO_DATA   = 0x24,
    ACC_CONF    = 0x40,
    ACC_RANGE   = 0x41,
    GYR_CONF    = 0x42,
    GYR_RANGE   = 0x43,
    MAG_CONF    = 0x44,
    FIFO_DOWNS    = 0x45, 
    FIFO_CONFIG_0 = 0x46, 
    FIFO_CONFIG_1 = 0x47, 
    MAG_IF_0    = 0x4C, 
    MAG_IF_1    = 0x4D, 
    MAG_IF_2    = 0x4E, 
    MAG_IF_3    = 0x4F,
    INT_EN_0    = 0x50,
    INT_EN_1    = 0x51,
    INT_EN_2    = 0x52,
    INT_OUT_CTRL  = 0x53,
    INT_LATCH   = 0x54,
    INT_MAP_0   = 0x55,
    INT_MAP_1   = 0x56,
    INT_MAP_2   = 0x57,
    INT_DATA_0    = 0x58,
    INT_DATA_1    = 0x59,
    INT_LOWHIGH_0 = 0x5A,
    INT_LOWHIGH_1 = 0x5B,
    INT_LOWHIGH_2 = 0x5C,
    INT_LOWHIGH_3 = 0x5D,
    INT_LOWHIGH_4 = 0x5E,
    INT_MOTION_0  = 0x5F,
    INT_MOTION_1  = 0x60,
    INT_MOTION_2  = 0x61,
    INT_MOTION_3  = 0x62,
    INT_TAP_0   = 0x63,
    INT_TAP_1   = 0x64,
    INT_ORIENT_0  = 0x65,
    INT_ORIENT_1  = 0x66,
    INT_FLAT_0    = 0x67,
    INT_FLAT_1    = 0x68,
    FOC_COMF    = 0x69,
    COMF      = 0x6A,
    IF_CONF     = 0x6B,
    PMU_TRIGGER   = 0x6C,
    SELF_TEST   = 0x6D,
    NV_CONF     = 0x70,
    OFFSET_0    = 0x71,
    OFFSET_1    = 0x72,
    OFFSET_2    = 0x73,
    OFFSET_3    = 0x74,
    OFFSET_4    = 0x75,
    OFFSET_5    = 0x76,
    OFFSET_6    = 0x77,
    STEP_CNT_0    = 0x78,
    STEP_CNT_1    = 0x79,
    STEP_CONF_0   = 0x7A,
    STEP_CONF_1   = 0x7B,
    CMD       = 0x7E
    } bmx160Registers_t;
    
/***************************************************************************
 CONSTRUCTOR
 ***************************************************************************/

 /**************************************************************************/
 /*!
     @brief  Instantiates a new BMX160 class, including assigning
             a unique ID to the accel and gyroscope for logging purposes.

     @param accelSensorID The unique ID to associate with the accelerometer.
     @param gyroSensorID The unique ID to associate with the gyroscope.
	 @param magSensorID The unique ID to associate with the magnetometer.
 */
 /**************************************************************************/
BMX160::BMX160(int32_t accelSensorID, int32_t gyroSensorID, int32_t magSensorID)
{
  _accelSensorID = accelSensorID;
  _gyroSensorID = gyroSensorID;
  _magSensorID = magSensorID;
}

/***************************************************************************
 PUBLIC FUNCTIONS
 ***************************************************************************/

void BMX160::SetReadFunc(read_func_t read_fn)
{
  read8 = read_fn;
}

void BMX160::SetWriteFunc(write_func_t write_fn)
{
  write8 = write_fn;
}

 /**************************************************************************/
 /*!
     @brief  Initializes the hardware, including setting the accelerometer
             range based on bmx160AccelRange_t and gyroscope range based
			 on bmx160GyroRange_t

     @param  rngAccel
             The range to set for the accelerometer, based on bmx160AccelRange_t
	 @param  rngGyro
             The range to set for the gyroscope, based on bmx160GyroRange_t		 

     @return True if the device was successfully initialized, otherwise false.
 */
 /**************************************************************************/
bool BMX160::begin(bmx160AccelRange_t rngAccel, bmx160GyroRange_t rngGyro)
{
  // dummy read to set bus protocol to SPI if needed (does not affect I2C bus if wired for I2C)
  read8(0x7F);

  /* Set the range the an appropriate value */
  _rangeAccel = rngAccel;
  _rangeGyro = rngGyro;

  /* Clear the raw sensor data */
  accel_raw.x = 0;
  accel_raw.y = 0;
  accel_raw.z = 0;
  gyro_raw.x = 0;
  gyro_raw.y = 0;
  gyro_raw.z = 0;
  mag_raw.x = 0;
  mag_raw.y = 0;
  mag_raw.z = 0;

  /* Make sure we have the correct chip ID since this checks
     for correct address and that the IC is properly connected */
  uint8_t id = read8(CHIP_ID);
  if (id != BMX160_ID)
  {
    return false;
  }

  /* Set accel to normal mode */
  write8(CMD, 0x11);
  delay(150);
  /* Set gyro to normal mode */
  write8(CMD, 0x15);
  delay(150);
  /* Set mag to normal mode */
  write8(CMD, 0x19);
  delay(200);
  
  uint8_t reg1 = read8(ACC_CONF);
  delay(50);
  // Write new ACCEL_CONFIG register value
  write8(ACC_CONF, reg1);
  
  /* Configure the accelerometer range */
  uint8_t accel_range = 0x03;
  switch (_rangeAccel) {
      case (BMX160_ACCELRANGE_2G):
        accel_range = BMX160_ACCELRANGE_2G; // Set full scale range for the accelerometer
      break;
      case (BMX160_ACCELRANGE_4G):
        accel_range = BMX160_ACCELRANGE_4G; // Set full scale range for the accelerometer
      break;
      case (BMX160_ACCELRANGE_8G):
        accel_range = BMX160_ACCELRANGE_8G; // Set full scale range for the accelerometer
      break;
	  case (BMX160_ACCELRANGE_16G):
        accel_range = BMX160_ACCELRANGE_16G; // Set full scale range for the accelerometer
      break;
  }

  write8(ACC_RANGE, accel_range);
  
    /* Set ACCEL_CONFIG (0x14)
   ====================================================================
   BIT  Symbol    Description                                   Default
   ---  ------    --------------------------------------------- -------
   7:6  -	      Reserved                                   		  -
   5:3  DLFPFCFG  Accelerometer low pass filter                     000
				  000 = 246 Hz / 1209 Hz if FCHOICE is 0
                  001 = 246 Hz
                  010 = 111.4 Hz
                  011 = 50.4 Hz
                  100 = 23.9 Hz
                  101 = 11.5 Hz
                  110 = 5.7 Hz
                  111 = 473 Hz
   2:1  FS_SEL    Accelerometer Full Scale Select                    00
     0  READY     Standby(0)/Ready(1)                                 0
  */

  /* Set CTRL_REG0 (0x0D)  Default value 0x00
  =====================================================================
  BIT  Symbol     Description                                   Default
  7:6  BW         cut-off frequency of low-pass filter               00
    5  SPIW       SPI interface mode selection                        0
  4:3  SEL        High-pass filter cutoff frequency selection        00
    2  HPF_EN     High-pass filter enable                             0
  1:0  FS         Full-scale range selection
                  00 = +-2000 dps
                  01 = +-1000 dps
                  10 = +-500 dps
                  11 = +-250 dps
  The bit fields in CTRL_REG0 should be changed only in Standby or Ready modes.
  */


  /* Reset then switch to active mode with 100Hz output */
  uint8_t ctrlReg0 = 0x03;
  /* Configure the gyroscope */
  switch(_rangeGyro)
  {
	 case GYRO_RANGE_125DPS:
      ctrlReg0 = 0x04;
      break;
    case GYRO_RANGE_250DPS:
      ctrlReg0 = 0x03;
      break;
    case GYRO_RANGE_500DPS:
      ctrlReg0 = 0x02;
      break;
    case GYRO_RANGE_1000DPS:
      ctrlReg0 = 0x01;
      break;
    case GYRO_RANGE_2000DPS:
      ctrlReg0 = 0x00;
      break;
  }

  write8(GYR_RANGE, ctrlReg0); // Set sensitivity
  delay(100); // 60 ms + 1/ODR
  write8(GYR_CONF, 0x28); // Set sensitivity
  delay(100);


  /* Configure the magnetometer */
  write8(MAG_IF_0, 0x80);
  delay(50);
  // Sleep mode
  write8(MAG_IF_3, 0x01);
  write8(MAG_IF_2, 0x4B);
  // REPXY regular preset
  write8(MAG_IF_3, 0x04);
  write8(MAG_IF_2, 0x51);
  // REPZ regular preset
  write8(MAG_IF_3, 0x0E);
  write8(MAG_IF_2, 0x52);
  
  write8(MAG_IF_3, 0x02);  // 0x02
  write8(MAG_IF_2, 0x4C);
  write8(MAG_IF_1, 0x42);  // 0x42
  write8(MAG_CONF, 0x05);
  write8(MAG_IF_0, 0x08);
  delay(50);
  /* Hybrid Mode, Over Sampling Rate = 16 */
  
  
  // uint8_t reg2 = read8(MAG_CONF);
  // delay(50);
  // // Write new MAG_CONF register value
  // write8(MAG_CONF, reg2);
  
  return true;
}

/**************************************************************************/
/*!
    @brief  Gets the most recent sensor events.

            This function reads from both the accelerometer, gyroscope and
            magnetometer in one call, and is a deviation from the standard
            Adafruit_Sensor API, but is provided as a convenience since most
            AHRS algorithms require sensor samples to be as close in time as
            possible.

    @param    accelEvent
              A reference to the sensors_event_t instances where the
              accelerometer data should be written.
    @param    gyroEvent
              A reference to the sensors_event_t instances where the
              gyroscope data should be written.
	@param	  magEvent
			  A reference to the sensors_event_t instances where the
			  magnetometer data should be written.

    @return True if the event read was successful, otherwise false.
*/
/**************************************************************************/
bool BMX160::getEvent(sensors_event_t* accelEvent, sensors_event_t* gyroEvent, sensors_event_t* magEvent)
{
  /* Clear the event */
  memset(accelEvent, 0, sizeof(sensors_event_t));
  memset(gyroEvent, 0, sizeof(sensors_event_t));
  memset(magEvent, 0, sizeof(sensors_event_t));

  /* Clear the raw data placeholder */
  accel_raw.x = 0;
  accel_raw.y = 0;
  accel_raw.z = 0;
  gyro_raw.x = 0;
  gyro_raw.y = 0;
  gyro_raw.z = 0;
  mag_raw.x = 0;
  mag_raw.y = 0;
  mag_raw.z = 0;

  /* Set the static metadata */
  accelEvent->version   = sizeof(sensors_event_t);
  accelEvent->sensor_id = _accelSensorID;
  accelEvent->type      = SENSOR_TYPE_ACCELEROMETER;

  gyroEvent->version   = sizeof(sensors_event_t);
  gyroEvent->sensor_id = _gyroSensorID;
  gyroEvent->type      = SENSOR_TYPE_GYROSCOPE;
  
  magEvent->version   = sizeof(sensors_event_t);
  magEvent->sensor_id = _magSensorID;
  magEvent->type      = SENSOR_TYPE_MAGNETIC_FIELD;


  uint8_t mxlo = read8(DATA_0);
  uint8_t mxhi = read8(DATA_1);
  uint8_t mylo = read8(DATA_2);
  uint8_t myhi = read8(DATA_3);
  uint8_t mzlo = read8(DATA_4);
  uint8_t mzhi = read8(DATA_5);
	uint8_t rhalllo = read8(DATA_6);
	uint8_t rhallhi = read8(DATA_7);
	uint8_t gxlo = read8(DATA_8);
	uint8_t gxhi = read8(DATA_9);
  uint8_t gylo = read8(DATA_10);
  uint8_t gyhi = read8(DATA_11);
  uint8_t gzlo = read8(DATA_12);
  uint8_t gzhi = read8(DATA_13);
  uint8_t axlo = read8(DATA_14);
	uint8_t axhi = read8(DATA_15);
  uint8_t aylo = read8(DATA_16);
  uint8_t ayhi = read8(DATA_17);
  uint8_t azlo = read8(DATA_18);
  uint8_t azhi = read8(DATA_19);
	uint8_t stlo = read8(SENSOERTIME_0);
  uint8_t stmi = read8(SENSOERTIME_1);
  uint8_t sthi = read8(SENSOERTIME_2);


	

  /* Set the timestamps */
  accelEvent->timestamp = millis();
  gyroEvent->timestamp = accelEvent->timestamp;
  magEvent->timestamp = ((int32_t)((sthi << 16) | (stmi << 8) | stlo) * 0.039) ;

  /* Shift values to create properly formed integers */
  /* Note, accel data is 14-bit and left-aligned, so we shift two bit right */
  accelEvent->acceleration.x = (int16_t)((axhi << 8) | axlo);
  accelEvent->acceleration.y = (int16_t)((ayhi << 8) | aylo);
  accelEvent->acceleration.z = (int16_t)((azhi << 8) | azlo);
  gyroEvent->gyro.x = (int16_t)((gxhi << 8) | gxlo);
  gyroEvent->gyro.y = (int16_t)((gyhi << 8) | gylo);
  gyroEvent->gyro.z = (int16_t)((gzhi << 8) | gzlo);
  magEvent->magnetic.x = (int16_t)((mxhi << 8) | mxlo);
  magEvent->magnetic.y = (int16_t)((myhi << 8) | mylo);
  magEvent->magnetic.z = (int16_t)((mzhi << 8) | mzlo);

  /* Assign raw values in case someone needs them */
  accel_raw.x = accelEvent->acceleration.x;
  accel_raw.y = accelEvent->acceleration.y;
  accel_raw.z = accelEvent->acceleration.z;
  gyro_raw.x = gyroEvent->gyro.x;
  gyro_raw.y = gyroEvent->gyro.y;
  gyro_raw.z = gyroEvent->gyro.z;
  mag_raw.x = magEvent->magnetic.x;
  mag_raw.y = magEvent->magnetic.y;
  mag_raw.z = magEvent->magnetic.z;

  /* Convert accel values to m/s^2 */
  switch (_rangeAccel) {
      case (BMX160_ACCELRANGE_2G):
          accelEvent->acceleration.x *= ACCEL_MG_LSB_2G * SENSORS_GRAVITY_STANDARD;
          accelEvent->acceleration.y *= ACCEL_MG_LSB_2G * SENSORS_GRAVITY_STANDARD;
          accelEvent->acceleration.z *= ACCEL_MG_LSB_2G * SENSORS_GRAVITY_STANDARD;
      break;
      case (BMX160_ACCELRANGE_4G):
          accelEvent->acceleration.x *= ACCEL_MG_LSB_4G * SENSORS_GRAVITY_STANDARD;
          accelEvent->acceleration.y *= ACCEL_MG_LSB_4G * SENSORS_GRAVITY_STANDARD;
          accelEvent->acceleration.z *= ACCEL_MG_LSB_4G * SENSORS_GRAVITY_STANDARD;
      break;
      case (BMX160_ACCELRANGE_8G):
          accelEvent->acceleration.x *= ACCEL_MG_LSB_8G * SENSORS_GRAVITY_STANDARD;
          accelEvent->acceleration.y *= ACCEL_MG_LSB_8G * SENSORS_GRAVITY_STANDARD;
          accelEvent->acceleration.z *= ACCEL_MG_LSB_8G * SENSORS_GRAVITY_STANDARD;
      break;
	  case (BMX160_ACCELRANGE_16G):
          accelEvent->acceleration.x *= ACCEL_MG_LSB_16G * SENSORS_GRAVITY_STANDARD;
          accelEvent->acceleration.y *= ACCEL_MG_LSB_16G * SENSORS_GRAVITY_STANDARD;
          accelEvent->acceleration.z *= ACCEL_MG_LSB_16G * SENSORS_GRAVITY_STANDARD;
      break;
  }
  
  /* Compensate values depending on the resolution */
  switch(_rangeGyro)
  {
	case GYRO_RANGE_125DPS:
      gyroEvent->gyro.x *= GYRO_SENSITIVITY_125DPS;
      gyroEvent->gyro.y *= GYRO_SENSITIVITY_125DPS;
      gyroEvent->gyro.z *= GYRO_SENSITIVITY_125DPS;
      break;
    case GYRO_RANGE_250DPS:
      gyroEvent->gyro.x *= GYRO_SENSITIVITY_250DPS;
      gyroEvent->gyro.y *= GYRO_SENSITIVITY_250DPS;
      gyroEvent->gyro.z *= GYRO_SENSITIVITY_250DPS;
      break;
    case GYRO_RANGE_500DPS:
      gyroEvent->gyro.x *= GYRO_SENSITIVITY_500DPS;
      gyroEvent->gyro.y *= GYRO_SENSITIVITY_500DPS;
      gyroEvent->gyro.z *= GYRO_SENSITIVITY_500DPS;
      break;
    case GYRO_RANGE_1000DPS:
      gyroEvent->gyro.x *= GYRO_SENSITIVITY_1000DPS;
      gyroEvent->gyro.y *= GYRO_SENSITIVITY_1000DPS;
      gyroEvent->gyro.z *= GYRO_SENSITIVITY_1000DPS;
      break;
    case GYRO_RANGE_2000DPS:
      gyroEvent->gyro.x *= GYRO_SENSITIVITY_2000DPS;
      gyroEvent->gyro.y *= GYRO_SENSITIVITY_2000DPS;
      gyroEvent->gyro.z *= GYRO_SENSITIVITY_2000DPS;
      break;
  }
  
  /* Convert mag values to uTesla */
  magEvent->magnetic.x *= MAG_UT_LSB;
  magEvent->magnetic.y *= MAG_UT_LSB;
  magEvent->magnetic.z *= MAG_UT_LSB;

  return true;
}

/**************************************************************************/
/*!
    @brief  Gets sensor_t data for both the accel, gyro and mag in one operation.

    @param    accelSensor
              A reference to the sensor_t instances where the
              accelerometer sensor info should be written.
    @param    gyroSensor
              A reference to the sensor_t instances where the
              gyroscope sensor info should be written.
	@param	  magSensor
			  A reference to the sensor_t instances where the
			  magnetometer sensor info should be written.
*/
/**************************************************************************/
void  BMX160::getSensor(sensor_t* accelSensor, sensor_t* gyroSensor, sensor_t* magSensor)
{
  /* Clear the sensor_t object */
  memset(accelSensor, 0, sizeof(sensor_t));
  memset(gyroSensor, 0, sizeof(sensor_t));
  memset(magSensor, 0, sizeof(sensor_t));

  /* Insert the sensor name in the fixed length char array */
  strncpy (accelSensor->name, "BMX160", sizeof(accelSensor->name) - 1);
  accelSensor->name[sizeof(accelSensor->name) - 1] = 0;
  accelSensor->version     = 1;
  accelSensor->sensor_id   = _accelSensorID;
  accelSensor->type        = SENSOR_TYPE_ACCELEROMETER;
  accelSensor->min_delay   = 0.01F; // 100Hz
  switch (_rangeAccel) {
      case (BMX160_ACCELRANGE_2G):
          accelSensor->max_value   = 2.0F * SENSORS_GRAVITY_STANDARD;
          accelSensor->min_value   = -1.999F * SENSORS_GRAVITY_STANDARD;
          accelSensor->resolution  = ACCEL_MG_LSB_2G * SENSORS_GRAVITY_STANDARD;
      break;
      case (BMX160_ACCELRANGE_4G):
          accelSensor->max_value   = 4.0F * SENSORS_GRAVITY_STANDARD;
          accelSensor->min_value   = -3.998F * SENSORS_GRAVITY_STANDARD;
          accelSensor->resolution  = ACCEL_MG_LSB_4G * SENSORS_GRAVITY_STANDARD;
      break;
      case (BMX160_ACCELRANGE_8G):
          accelSensor->max_value   = 8.0F * SENSORS_GRAVITY_STANDARD;
          accelSensor->min_value   = -7.996F * SENSORS_GRAVITY_STANDARD;
          accelSensor->resolution  = ACCEL_MG_LSB_8G * SENSORS_GRAVITY_STANDARD;
      break;
	  case (BMX160_ACCELRANGE_16G):
          accelSensor->max_value   = 16.0F * SENSORS_GRAVITY_STANDARD;
          accelSensor->min_value   = -15.996F * SENSORS_GRAVITY_STANDARD;
          accelSensor->resolution  = ACCEL_MG_LSB_16G * SENSORS_GRAVITY_STANDARD;
      break;
  }

  strncpy (gyroSensor->name, "BMX160", sizeof(gyroSensor->name) - 1);
  gyroSensor->name[sizeof(gyroSensor->name) - 1] = 0;
  gyroSensor->version     = 1;
  gyroSensor->sensor_id   = _gyroSensorID;
  gyroSensor->type        = SENSOR_TYPE_GYROSCOPE;
  gyroSensor->min_delay   = 0;
  gyroSensor->max_value   = (float)this->_rangeGyro;
  gyroSensor->min_value   = (this->_rangeGyro * -1.0);
  gyroSensor->resolution  = 0.0F; // TBD
  
  strncpy (magSensor->name, "BMX160", sizeof(magSensor->name) - 1);
  magSensor->name[sizeof(magSensor->name)- 1] = 0;
  magSensor->version     = 1;
  magSensor->sensor_id   = _magSensorID;
  magSensor->type        = SENSOR_TYPE_MAGNETIC_FIELD;
  magSensor->min_delay   = 0.01F;
  magSensor->max_value   = 2500.0F;
  magSensor->min_value   = -2500.0F;
  magSensor->resolution  = 0.1F; // TBD
}

/**************************************************************************/
/*!
    @brief    Reads a single sensor event from the accelerometer.

    @attention

    This function exists to keep Adafruit_Sensor happy since we
    need a single sensor in the canonical .getEvent call. The
    non-standard .getEvent call with two parameters should
    generally be used with this sensor.

    @param    accelEvent
              A reference to the sensors_event_t instances where the
              accelerometer data should be written.

    @return True if the event read was successful, otherwise false.
*/
/**************************************************************************/
bool BMX160::getEvent(sensors_event_t* accelEvent)
{
    sensors_event_t accel;

    return getEvent(accelEvent, &accel, &accel);
}

/**************************************************************************/
/*!
    @brief   Gets sensor details about the accelerometer.

    @attention

    This function exists to keep Adafruit_Sensor happy since we
    need a single sensor in the canonical .getSensor call. The
    non-standard .getSensor call with two parameters should
    generally be used with this sensor.

    @param   accelSensor
             A reference to the sensor_t instances where the
             accelerometer sensor info should be written.
*/
/**************************************************************************/
void  BMX160::getSensor(sensor_t* accelSensor)
{
    sensor_t accel;

    return getSensor(accelSensor, &accel, &accel);
}


/**************************************************************************/
/*!
    @brief  Puts device into/out of standby mode

    @param standby Set this to a non-zero value to enter standy mode.
*/
/**************************************************************************/
void BMX160::standby(boolean standby)
{
  uint8_t reg1 = read8(PMU_STATUS);
  if (standby) {
	  if(reg1 != 0) {
	    /* Set accel to suspend mode */
		write8(CMD, 0x11);
		delay(150);
		/* Set gyro to suspend mode */
		write8(CMD, 0x14);
		delay(150);
		/* Set mag to suspend mode */
		write8(CMD, 0x18);
		delay(150);
	  }
  } else {
	  if(reg1 == 0) {
	    /* Set accel to normal mode */
		write8(CMD, 0x10);
		delay(150);
		/* Set gyro to normal mode */
		write8(CMD, 0x15);
		delay(150);
		/* Set mag to normal mode */
		write8(CMD, 0x19);
		delay(150);
	  }
  }
  
  if (! standby) {
    delay(100);
  }
}
