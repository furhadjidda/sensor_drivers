/*!
 * @file Adafruit_VL53L0X.h

  This is a library for the Adafruit VL53L0X Sensor Breakout

  Designed specifically to work with the VL53L0X sensor from Adafruit
  ----> https://www.adafruit.com/products/3317

  These sensors use I2C to communicate, 2 pins are required to
  interface
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  Updated by Andrew DeVries for Digital Example to include methods needed for
  Interrupt triggering.
  BSD license, all text above must be included in any
  redistribution
 ****************************************************/

#ifndef ADAFRUIT_VL53L0X_H
#define ADAFRUIT_VL53L0X_H

#include "vl53l0x_api.h"

#define VL53L0X_I2C_ADDR 0x29  ///< Default sensor I2C address

/**************************************************************************/
/*!
    @brief  Class that stores state and functions for interacting with VL53L0X
   time-of-flight sensor chips
*/
/**************************************************************************/
class Adafruit_VL53L0X {
   public:
    /** Sensor configurations */
    typedef enum {
        VL53L0X_SENSE_DEFAULT = 0,
        VL53L0X_SENSE_LONG_RANGE,
        VL53L0X_SENSE_HIGH_SPEED,
        VL53L0X_SENSE_HIGH_ACCURACY
    } VL53L0X_Sense_config_t;

    bool begin(uint8_t i2c_addr = VL53L0X_I2C_ADDR, bool debug = false, i2c_inst_t *i2c = i2c0,
               VL53L0X_Sense_config_t vl_config = VL53L0X_SENSE_DEFAULT);
    bool setAddress(uint8_t newAddr);

    // uint8_t getAddress(void); // not currently implemented

    /**************************************************************************/
    /*!
        @brief  get a ranging measurement from the device
        @param  pRangingMeasurementData the pointer to the struct the data will be
       stored in
        @param debug Optional debug flag. If true debug information will print via
       Serial.print during execution. Defaults to false.
        @returns True if address was set successfully, False otherwise
    */
    /**************************************************************************/
    VL53L0X_Error rangingTest(VL53L0X_RangingMeasurementData_t *pRangingMeasurementData, bool debug = false) {
        return getSingleRangingMeasurement(pRangingMeasurementData, debug);
    };

    VL53L0X_Error getSingleRangingMeasurement(VL53L0X_RangingMeasurementData_t *pRangingMeasurementData,
                                              bool debug = false);
    void printRangeStatus(VL53L0X_RangingMeasurementData_t *pRangingMeasurementData);

    VL53L0X_Error getRangingMeasurement(VL53L0X_RangingMeasurementData_t *pRangingMeasurementData, bool debug = false);
    VL53L0X_Error startMeasurement(bool debug = false);
    VL53L0X_Error stopMeasurement(bool debug = false);
    VL53L0X_Error getLimitCheckCurrent(uint8_t LimitCheckId, FixPoint1616_t *pLimitCheckCurrent, bool debug = false);
    VL53L0X_Error getDeviceMode(VL53L0X_DeviceModes *pDeviceMode, bool debug = false);
    VL53L0X_Error setDeviceMode(VL53L0X_DeviceModes DeviceMode, bool debug = false);

    VL53L0X_Error setInterruptThresholds(FixPoint1616_t ThresholdLow, FixPoint1616_t ThresholdHigh, bool debug = false);
    VL53L0X_Error getInterruptThresholds(FixPoint1616_t *pThresholdLow, FixPoint1616_t *pThresholdHigh,
                                         bool debug = false);
    VL53L0X_Error clearInterruptMask(bool debug = false);

    VL53L0X_Error getGpioConfig(VL53L0X_DeviceModes *pDeviceMode, VL53L0X_GpioFunctionality *pFunctionality,
                                VL53L0X_InterruptPolarity *pPolarity, bool debug = false);
    VL53L0X_Error setGpioConfig(VL53L0X_DeviceModes DeviceMode, VL53L0X_GpioFunctionality Functionality,
                                VL53L0X_InterruptPolarity Polarity, bool debug = false);

    VL53L0X_Error Status = VL53L0X_ERROR_NONE;  ///< indicates whether or not the sensor has
                                                ///< encountered an error
    // Add similar methods as Adafruit_VL6180X class adapted to range of device
    uint16_t readRange(void);
    // float readLux(uint8_t gain);
    uint8_t readRangeStatus(void);

    bool startRange(void);
    bool isRangeComplete(void);
    bool waitRangeComplete(void);
    uint16_t readRangeResult(void);

    bool startRangeContinuous(uint16_t period_ms = 50);
    void stopRangeContinuous(void);

    //  void setTimeout(uint16_t timeout) { io_timeout = timeout; }
    // uint16_t getTimeout(void) { return io_timeout; }
    /**************************************************************************/
    /*!
        @brief  timeout status
        @returns True if timeout has occurred, False otherwise
    */
    /**************************************************************************/
    bool timeoutOccurred(void) { return false; }

    bool configSensor(VL53L0X_Sense_config_t vl_config);

    // Export some wrappers to internal setting functions
    // that are used by the above helper function to allow
    // more complete control.
    bool setMeasurementTimingBudgetMicroSeconds(uint32_t budget_us);
    uint32_t getMeasurementTimingBudgetMicroSeconds(void);

    bool setVcselPulsePeriod(VL53L0X_VcselPeriod VcselPeriodType, uint8_t VCSELPulsePeriod);

    uint8_t getVcselPulsePeriod(VL53L0X_VcselPeriod VcselPeriodType);

    bool setLimitCheckEnable(uint16_t LimitCheckId, uint8_t LimitCheckEnable);
    uint8_t getLimitCheckEnable(uint16_t LimitCheckId);
    bool setLimitCheckValue(uint16_t LimitCheckId, FixPoint1616_t LimitCheckValue);
    FixPoint1616_t getLimitCheckValue(uint16_t LimitCheckId);

   private:
    VL53L0X_Dev_t MyDevice;
    VL53L0X_Dev_t *pMyDevice = &MyDevice;
    VL53L0X_DeviceInfo_t DeviceInfo;

    uint8_t _rangeStatus;
};

#endif
