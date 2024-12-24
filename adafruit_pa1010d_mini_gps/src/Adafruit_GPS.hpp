/**************************************************************************/
/*!
  @file Adafruit_GPS.cpp

  @mainpage Adafruit Ultimate GPS Breakout

  @section intro Introduction

  This is the Adafruit GPS library - the ultimate GPS library
  for the ultimate GPS module!

  Tested and works great with the Adafruit Ultimate GPS module
  using MTK33x9 chipset
  ------> http://www.adafruit.com/products/746

  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  @section author Author

  Written by Limor Fried/Ladyada for Adafruit Industries.
  Adapted by Furhad Jidda for pico

  @section license License

  BSD license, check license.txt for more information
  All text above must be included in any redistribution
*/
/**************************************************************************/

#ifndef _ADAFRUIT_GPS_H
#define _ADAFRUIT_GPS_H

#include <Adafruit_PMTK.hpp>
#include <NMEA_data.hpp>

#include "i2c_wrapper.hpp"
#ifndef BUILD_FOR_HOST
#include "pico/stdlib.h"
#endif

#define NMEA_EXTENSIONS

#define GPS_MAX_I2C_TRANSFER 32  ///< The max number of bytes we'll try to read at once
#define MAXLINELENGTH 120        ///< how long are max NMEA lines to parse?
#define NMEA_MAX_SENTENCE_ID 20  ///< maximum length of a sentence ID name, including terminating 0
#define NMEA_MAX_SOURCE_ID 3     ///< maximum length of a source ID name, including terminating 0

/// type for resulting code from running check()
typedef enum {
    NMEA_BAD = 0,             ///< passed none of the checks
    NMEA_HAS_DOLLAR = 1,      ///< has a dollar sign or exclamation mark in the first position
    NMEA_HAS_CHECKSUM = 2,    ///< has a valid checksum at the end
    NMEA_HAS_NAME = 4,        ///< there is a token after the $ followed by a comma
    NMEA_HAS_SOURCE = 10,     ///< has a recognized source ID
    NMEA_HAS_SENTENCE = 20,   ///< has a recognized sentence ID
    NMEA_HAS_SENTENCE_P = 40  ///< has a recognized parseable sentence ID
} nmea_check_t;

class Adafruit_GPS {
   public:
    Adafruit_GPS(i2c_inst_t *aI2cInstance);

    virtual ~Adafruit_GPS();

    bool Init(uint32_t aI2cAddress);

    char ReadData(void);
    void SendCommand(const uint8_t *str, uint8_t len);
    bool NewNMEAreceived();
    void Pause(bool b);
    char *LastNMEA(void);
    bool WaitForSentence(const char *wait, uint8_t max = MAXWAITSENTENCE, bool usingInterrupts = false);
    bool LOCUS_StartLogger(void);
    bool LOCUS_StopLogger(void);
    bool LOCUS_ReadStatus(void);
    bool Standby(void);
    bool Wakeup(void);
    nmea_float_t SecondsSinceFix();
    nmea_float_t SecondsSinceTime();
    nmea_float_t SecondsSinceDate();
    void ResetSentTime();

    // NMEA_parse.cpp
    bool Parse(char *);
    bool Check(char *nmea);
    bool OnList(char *nmea, const char **list);
    uint8_t ParseHex(char c);

    // NMEA_build.cpp
#ifdef NMEA_EXTENSIONS
    char *Build(char *nmea, const char *thisSource, const char *thisSentence, char ref = 'R', bool noCRLF = false);
#endif
    void AddChecksum(char *buff);

    // NMEA_data.cpp
    void NewDataValue(nmea_index_t tag, nmea_float_t v);
#ifdef NMEA_EXTENSIONS
    nmea_float_t get(nmea_index_t idx);
    nmea_float_t getSmoothed(nmea_index_t idx);
    void initDataValue(nmea_index_t idx, char *label = NULL, char *fmt = NULL, char *unit = NULL,
                       unsigned long response = 0, nmea_value_type_t type = NMEA_SIMPLE_FLOAT);
    nmea_history_t *initHistory(nmea_index_t idx, nmea_float_t scale = 10.0, nmea_float_t offset = 0.0,
                                unsigned historyInterval = 20, unsigned historyN = 192);
    void removeHistory(nmea_index_t idx);
    void showDataValue(nmea_index_t idx, int n = 7);
    bool isCompoundAngle(nmea_index_t idx);
#endif
    nmea_float_t boatAngle(nmea_float_t s, nmea_float_t c);
    nmea_float_t compassAngle(nmea_float_t s, nmea_float_t c);

    int thisCheck = 0;                              ///< the results of the check on the current sentence
    char thisSource[NMEA_MAX_SOURCE_ID] = {0};      ///< the first two letters of the current sentence, e.g. WI, GP
    char thisSentence[NMEA_MAX_SENTENCE_ID] = {0};  ///< the next three letters of the current sentence, e.g. GLL, RMC
    char lastSource[NMEA_MAX_SOURCE_ID] = {0};      ///< the results of the check on the most recent successfully
                                                    ///< parsed sentence
    char lastSentence[NMEA_MAX_SENTENCE_ID] = {0};  ///< the next three letters of the most recent successfully parsed
                                                    ///< sentence, e.g. GLL, RMC

    // ========================== Data ====================================================
    // TODO: Cluster  them for simplicity and readability
    // For example cluster into coordinates , time and date , LOCUS data
    uint8_t mHour = 0;           ///< GMT hours
    uint8_t mMinute = 0;         ///< GMT minutes
    uint8_t mSeconds = 0;        ///< GMT mSeconds
    uint16_t mMilliseconds = 0;  ///< GMT mMilliseconds
    uint8_t mYear = 0;           ///< GMT mYear
    uint8_t mMonth = 0;          ///< GMT mMonth
    uint8_t mDay = 0;            ///< GMT mDay

    nmea_float_t mLatitude = 0.0;   ///< Floating point mLatitude value in degrees/minutes
                                    ///< as received from the GPS (DDMM.MMMM)
    nmea_float_t mLongitude = 0.0;  ///< Floating point mLongitude value in degrees/minutes
                                    ///< as received from the GPS (DDDMM.MMMM)

    /** Fixed point mLatitude and mLongitude value with degrees stored in units of
      1/10000000 of a degree. See pull #13 for more details:
      https://github.com/adafruit/Adafruit-GPS-Library/pull/13 */
    int32_t mLatitude_fixed;   ///< Fixed point mLatitude in decimal degrees.
                               ///< Divide by 10000000.0 to get a double.
    int32_t mLongitude_fixed;  ///< Fixed point mLongitude in decimal degrees
                               ///< Divide by 10000000.0 to get a double.

    nmea_float_t mLatitudeDegrees = 0.0;   ///< Latitude in decimal degrees
    nmea_float_t mLongitudeDegrees = 0.0;  ///< Longitude in decimal degrees
    nmea_float_t mGeoidheight = 0.0;       ///< Diff between geoid height and WGS84 height
    nmea_float_t mAltitude = 0.0;          ///< Altitude in meters above MSL
    nmea_float_t mSpeed = 0.0;             ///< Current mSpeed over ground in knots
    nmea_float_t mAngle = 0.0;             ///< Course in degrees from true north
    nmea_float_t mMagvariation = 0.0;      ///< Magnetic variation in degrees (vs. true north)
    nmea_float_t mHDOP = 0.0;              ///< Horizontal Dilution of Precision - relative
                                           ///< accuracy of horizontal position
    nmea_float_t mVDOP = 0.0;              ///< Vertical Dilution of Precision - relative
                                           ///< accuracy of vertical position
    nmea_float_t mPDOP = 0.0;              ///< Position Dilution of Precision - Complex maths derives
                                           ///< a simple, single number for each kind of DOP
    char mLat = 'X';                       ///< N/S
    char mLon = 'X';                       ///< E/W
    char mMag = 'X';                       ///< Magnetic variation direction
    bool mFix = false;                     ///< Have a fix?
    uint8_t mFixquality = 0;               ///< Fix quality (0, 1, 2 = Invalid, GPS, DGPS)
    uint8_t mFixquality_3d = 0;            ///< 3D mFix quality (1, 3, 3 = Nofix, 2D mFix, 3D mFix)
    uint8_t mSatellites = 0;               ///< Number of mSatellites in use
    uint8_t mAntenna = 0;                  ///< Antenna that is used (from PGTOP)

    uint16_t mLOCUS_serial = 0;   ///< Log serial number
    uint16_t mLOCUS_records = 0;  ///< Log number of data record
    uint8_t mLOCUS_type = 0;      ///< Log type, 0: Overlap, 1: FullStop
    uint8_t mLOCUS_mode = 0;      ///< Logging mode, 0x08 interval logger
    uint8_t mLOCUS_config = 0;    ///< Contents of configuration
    uint8_t mLOCUS_interval = 0;  ///< Interval setting
    uint8_t mLOCUS_distance = 0;  ///< Distance setting
    uint8_t mLOCUS_speed = 0;     ///< Speed setting
    uint8_t mLOCUS_status = 0;    ///< 0: Logging, 1: Stop logging
    uint8_t mLOCUS_percent = 0;   ///< Log life used percentage

#ifdef NMEA_EXTENSIONS
    // NMEA additional public variables
    nmea_datavalue_t mVal[NMEA_MAX_INDEX];  ///< an array of data value structs, mVal[0] = most
                                            ///< recent mHDOP so that ockam indexing works
    nmea_float_t mDepthToKeel = 2.4;        ///< depth from surface to bottom of keel in metres
    nmea_float_t mDepthToTransducer = 0.0;  ///< depth of transducer below the surface in metres

    char mToID[NMEA_MAX_WP_ID] = {0};    ///< id of waypoint going to on this segment of the route
    char mFromID[NMEA_MAX_WP_ID] = {0};  ///< id of waypoint coming from on this segment of the route

    char mTxtTXT[63] = {0};  ///< text content from most recent TXT sentence
    int mTxtTot = 0;         ///< total TXT sentences in group
    int mTxtID = 0;          ///< id of the text message
    int mTxtNumber = 0;      ///< the TXT sentence number
#endif                       // NMEA_EXTENSIONS

   private:
    // NMEA_data.cpp
    void data_init();
    // NMEA_parse.cpp
    const char *tokenOnList(char *token, const char **list);
    bool parseCoord(char *p, nmea_float_t *angleDegrees = NULL, nmea_float_t *mAngle = NULL,
                    int32_t *angle_fixed = NULL, char *dir = NULL);
    char *parseStr(char *buff, char *p, int n);
    bool parseTime(char *);
    bool parseFix(char *);
    bool parseAntenna(char *);
    bool isEmpty(char *pStart);

    // Make all of these times far in the past by setting them near the middle
    // of the millis() range. Timing assumes that sentences are parsed promptly.
    uint32_t mLastUpdate = 2000000000L;  ///< millis() when last full sentence successfully parsed
    uint32_t mLastFix = 2000000000L;     ///< millis() when last mFix received
    uint32_t mLastTime = 2000000000L;    ///< millis() when last time received
    uint32_t mLastDate = 2000000000L;    ///< millis() when last date received
    uint32_t mRecvdTime = 2000000000L;   ///< millis() when last full sentence received
    uint32_t mSentTime = 2000000000L;    ///< millis() when first character of last
                                         ///< full sentence received
    bool mPaused = false;

    bool mNoComms = false;

    i2c_inst_t *mI2c{nullptr};
    uint8_t mI2cAddress = 0x00;
    char mI2cBuffer[GPS_MAX_I2C_TRANSFER];
    int8_t mBuffMax = -1;
    int8_t mBuffIdx = 0;
    char mLastChar = 0;

    volatile char mLine1[MAXLINELENGTH];   ///< We double buffer: read one line in
                                           ///< and leave one for the main program
    volatile char mLine2[MAXLINELENGTH];   ///< Second buffer
    volatile uint8_t mLineidx = 0;         ///< our index into filling the current line
    volatile char *mCurrentLine{nullptr};  ///< Pointer to current line buffer
    volatile char *mLastline{nullptr};     ///< Pointer to previous line buffer
    volatile bool mRecvdflag = false;      ///< Received flag
    volatile bool mInStandbyMode = false;  ///< In standby flag
};

#endif
