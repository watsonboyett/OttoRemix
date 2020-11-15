/*!
 * @file BMX160_AK09916.h
 *
 * This header file is part of DPEng's BMX160 driver for the Arduino platform.  It is
 * designed specifically to work with the DPEng BMX160 breakout board:
 * https://www.ebay.co.uk/itm/323835783928
 * [AMAZON BMX160 BREAKOUT BOARD PRODUCT LINK IS COMING SOON!!!]
 *
 * These sensors use I2C to communicate, 2 pins (SCX+SDX) are required
 * to interface with the breakout. SPI is also possible with <2 MBIT/s speeds.
 *
 * DP Engineering invests time and resources providing this open source code,
 * please support DPEng by purchasing this breakout board from DPEng
 *
 * Written by David Pattison for DP Engineering Ltd.
 *
 * MIT license, all text here must be included in any redistribution.
 *
 */
/** \file BMX160.h */
#ifndef __BMX160_H__
#define __BMX160_H__

#include <Arduino.h>
#include <Adafruit_Sensor.h>

/*=========================================================================
    I2C ADDRESS/BITS AND SETTINGS
    -----------------------------------------------------------------------*/
    /** 7-bit I2C address for this sensor */
    #define BMX160_ADDRESS           (0x68)     // 0110 1001
    /** Device ID for this sensor (used as sanity check during init) */
    #define BMX160_ID                (0xD8)     // 1110 1010
	
	/** Macro for mg per LSB at +/- 2g sensitivity (1 LSB = 0.000061035mg) */
	#define ACCEL_MG_LSB_2G (0.000061035F)
	/** Macro for mg per LSB at +/- 4g sensitivity (1 LSB = 0.000122070mg) */
	#define ACCEL_MG_LSB_4G (0.000122070F)
	/** Macro for mg per LSB at +/- 8g sensitivity (1 LSB = 0.000244141mg) */
	#define ACCEL_MG_LSB_8G (0.000244141F)
	/** Macro for mg per LSB at +/- 16g sensitivity (1 LSB = 0.000488281mg) */
	#define ACCEL_MG_LSB_16G (0.000488281F)
	
	/** Gyroscope sensitivity at 125dps */
    #define GYRO_SENSITIVITY_125DPS  (0.0038110F) // Table 1 of datasheet
	/** Gyroscope sensitivity at 250dps */
    #define GYRO_SENSITIVITY_250DPS  (0.0076220F) // Table 1 of datasheet
    /** Gyroscope sensitivity at 500dps */
    #define GYRO_SENSITIVITY_500DPS  (0.0152439F)
    /** Gyroscope sensitivity at 1000dps */
    #define GYRO_SENSITIVITY_1000DPS (0.0304878F)
    /** Gyroscope sensitivity at 2000dps */
    #define GYRO_SENSITIVITY_2000DPS (0.0609756F)
	
	/** Macro for micro tesla (uT) per LSB (1 LSB = 0.1uT) */
	#define MAG_UT_LSB      (0.3F)
	
/*=========================================================================*/


/*=========================================================================
    OPTIONAL SPEED SETTINGS
    -----------------------------------------------------------------------*/
    /*!
        Range settings for the accelerometer sensor.
    */
    typedef enum
    {
      BMX160_ACCELRANGE_2G                = (0b0011),
      BMX160_ACCELRANGE_4G                = (0b0101),
      BMX160_ACCELRANGE_8G                = (0b1000),
	  BMX160_ACCELRANGE_16G               = (0b1100)
    } bmx160AccelRange_t;
	
	/*!
        Enum to define valid gyroscope range values
    */
    typedef enum
    {
	  GYRO_RANGE_125DPS  = 125,     /**< 125dps */
      GYRO_RANGE_250DPS  = 250,     /**< 250dps */
      GYRO_RANGE_500DPS  = 500,     /**< 500dps */
      GYRO_RANGE_1000DPS = 1000,    /**< 1000dps */
      GYRO_RANGE_2000DPS = 2000     /**< 2000dps */
    } bmx160GyroRange_t;
/*=========================================================================*/

/*=========================================================================
    RAW GYROSCOPE DATA TYPE
    -----------------------------------------------------------------------*/
    /*!
        @brief  Raw (integer) values from the gyroscope sensor.
    */
    typedef struct
    {
      int16_t x;    /**< Raw int16_t value from the x axis */
      int16_t y;    /**< Raw int16_t value from the y axis */
      int16_t z;    /**< Raw int16_t value from the z axis */
    } bmx160RawData_t;

    typedef uint8_t (*read_func_t)(uint8_t);
	typedef void (*write_func_t)(uint8_t, uint8_t);


/*=========================================================================*/

/**************************************************************************/
/*!
    @brief  Unified sensor driver for the BMX160 breakout.
*/
/**************************************************************************/
class BMX160 : public Adafruit_Sensor
{
  public:
    BMX160(int32_t accelSensorID = -1, int32_t gyroSensorID = -1, int32_t magSensorID = -1);

    bool begin           ( bmx160AccelRange_t rngAccel = BMX160_ACCELRANGE_2G, bmx160GyroRange_t rngGyro = GYRO_RANGE_250DPS );
    bool getEvent        ( sensors_event_t* accel );
    void getSensor       ( sensor_t* accel );
    bool getEvent        ( sensors_event_t* accel, sensors_event_t* gyro, sensors_event_t* mag );
    void getSensor       ( sensor_t* accel, sensor_t* gyro, sensor_t* mag );
    void standby         ( boolean standby );

    void SetReadFunc	 (read_func_t read_fn);
	void SetWriteFunc	 (write_func_t write_fn);

    /*! Raw accelerometer values from last sucsessful sensor read */
    bmx160RawData_t accel_raw;
    /*! Raw gyroscope values from last successful sensor read */
    bmx160RawData_t gyro_raw;
	/*! Raw magnetometer values from last successful sensor read */
    bmx160RawData_t mag_raw;

  private:
	read_func_t read8;
	write_func_t write8;

    bmx160AccelRange_t 	_rangeAccel;
    bmx160GyroRange_t 	_rangeGyro;
    int32_t             _accelSensorID;
    int32_t             _gyroSensorID;
	int32_t             _magSensorID;
};

#endif
