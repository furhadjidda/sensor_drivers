
/*!
  @file Adafruit_GPS.cpp

  @mainpage Adafruit Ultimate GPS Breakout

  @section intro Introduction

  This is the Adafruit GPS library - the ultimate GPS library for the ultimate GPS module!

  Tested and works great with the Adafruit Ultimate GPS module using MTK33x9 chipset
  ------> http://www.adafruit.com/products/746

  Adafruit invests time and resources providing this open source code,please support Adafruit and open-source
  hardware by purchasing products from Adafruit!

  @section author Author

  Written by Limor Fried/Ladyada for Adafruit Industries.
  Adapted by Furhad Jidda for pico

  @section license License

  BSD license, check license.txt for more information
  All text above must be included in any redistribution
*/

#include <string.h>

#include <Adafruit_GPS.hpp>

#include "utils.hpp"

#define I2C_SDA_PIN 4
#define I2C_SCL_PIN 5

bool Adafruit_GPS::Init(uint32_t aI2cAddress) {
    mI2cAddress = aI2cAddress;
    i2c_init(mI2c, 100 * 1000);
    gpio_set_function(I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA_PIN);
    gpio_pull_up(I2C_SCL_PIN);

    return true;
}

/*!
    @brief Constructor when using I2C
    @param theWire Pointer to an I2C object
*/
Adafruit_GPS::Adafruit_GPS(i2c_inst_t *aI2cInstance) {
    mCurrentLine = mLine1;
    mLastline = mLine2;

#ifdef NMEA_EXTENSIONS
    data_init();
#endif
    mI2c = aI2cInstance;
}

/*!
    @brief    Destroy the object.
    @return   none
*/
Adafruit_GPS::~Adafruit_GPS() {
#ifdef NMEA_EXTENSIONS
    for (int i = 0; i < (int)NMEA_MAX_INDEX; i++) removeHistory((nmea_index_t)i);  // to free any history mallocs
#endif
}

/*!
    @brief Read one character from the GPS device.

    Call very frequently and multiple times per opportunity or the buffer
    may overflow if there are frequent NMEA sentences. An 82 character NMEA
    sentence 10 times per second will require 820 calls per second, and
    once a loop() may not be enough. Check for newNMEAreceived() after at
    least every 10 calls, or you may miss some short sentences.
    @return The character that we received, or 0 if nothing was available
*/
char Adafruit_GPS::ReadData(void) {
    static uint32_t firstChar = 0;  // first character received in current sentence
    uint32_t tStart = millis();     // as close as we can get to time char was sent
    char c = 0;

    if (mPaused || mNoComms) return c;

    if (mI2c) {
        if (mBuffIdx <= mBuffMax) {
            c = mI2cBuffer[mBuffIdx];
            mBuffIdx++;
        } else {
            uint8_t buffer[GPS_MAX_I2C_TRANSFER];
            int bytes_read = i2c_read_blocking(mI2c, mI2cAddress, buffer, GPS_MAX_I2C_TRANSFER, false);

            if (bytes_read == GPS_MAX_I2C_TRANSFER) {
                // Got data!
                mBuffMax = 0;
                char curr_char = 0;
                for (int i = 0; i < GPS_MAX_I2C_TRANSFER; i++) {
                    curr_char = buffer[i];
                    if ((curr_char == 0x0A) && (mLastChar != 0x0D)) {
                        // Skip duplicate 0x0A's - but keep as part of a CRLF
                        continue;
                    }
                    mLastChar = curr_char;
                    mI2cBuffer[mBuffMax] = curr_char;
                    mBuffMax++;
                }
                mBuffMax--;  // Back up to the last valid slot
                if ((mBuffMax == 0) && (mI2cBuffer[0] == 0x0A)) {
                    mBuffMax = -1;  // Ahh there was nothing to read after all
                }
                mBuffIdx = 0;
            }
            return c;
        }
    }

    mCurrentLine[mLineidx] = c;
    mLineidx = mLineidx + 1;
    if (mLineidx >= MAXLINELENGTH)
        mLineidx = MAXLINELENGTH - 1;  // ensure there is someplace to put the next received character

    if (c == '\n') {
        mCurrentLine[mLineidx] = 0;

        if (mCurrentLine == mLine1) {
            mCurrentLine = mLine2;
            mLastline = mLine1;
        } else {
            mCurrentLine = mLine1;
            mLastline = mLine2;
        }

        mLineidx = 0;
        mRecvdflag = true;
        mRecvdTime = millis();  // time we got the end of the string
        mSentTime = firstChar;
        firstChar = 0;  // there are no characters yet
        return c;       // wait until next character to set time
    }

    if (firstChar == 0) firstChar = tStart;
    return c;
}

/*!
    @brief Send a command to the GPS device
    @param str Pointer to a string holding the command to send
*/
void Adafruit_GPS::SendCommand(const uint8_t *str, uint8_t len) {
    i2c_write_blocking(mI2c, mI2cAddress, str, len, false);
}

/*!
    @brief Check to see if a new NMEA line has been received
    @return True if received, false if not
*/
bool Adafruit_GPS::NewNMEAreceived(void) { return mRecvdflag; }

/*!
    @brief Pause/unpause receiving new data
    @param p True = pause, false = unpause
*/
void Adafruit_GPS::Pause(bool p) { mPaused = p; }

/*!
    @brief Returns the last NMEA line received and unsets the received flag
    @return Pointer to the last line string
*/
char *Adafruit_GPS::LastNMEA(void) {
    mRecvdflag = false;
    return (char *)mLastline;
}

/*!
    @brief Wait for a specified sentence from the device
    @param wait4me Pointer to a string holding the desired response
    @param max How long to wait, default is MAXWAITSENTENCE
    @param usingInterrupts True if using interrupts to read from the GPS
   (default is false)
    @return True if we got what we wanted, false otherwise
*/
bool Adafruit_GPS::WaitForSentence(const char *wait4me, uint8_t max, bool usingInterrupts) {
    uint8_t i = 0;
    while (i < max) {
        if (!usingInterrupts) ReadData();

        if (NewNMEAreceived()) {
            char *nmea = LastNMEA();
            i++;

            if (StrStartsWith(nmea, wait4me)) {
                return true;
            }
        }
    }

    return false;
}

/*!
    @brief Start the LOCUS logger
    @return True on success, false if it failed
*/
bool Adafruit_GPS::LOCUS_StartLogger(void) {
    SendCommand(reinterpret_cast<const uint8_t *>(PMTK_LOCUS_STARTLOG), strlen(PMTK_LOCUS_STARTLOG));
    mRecvdflag = false;
    return WaitForSentence(PMTK_LOCUS_STARTSTOPACK);
}

/*!
    @brief Stop the LOCUS logger
    @return True on success, false if it failed
*/
bool Adafruit_GPS::LOCUS_StopLogger(void) {
    SendCommand(reinterpret_cast<const uint8_t *>(PMTK_LOCUS_STOPLOG), strlen(PMTK_LOCUS_STOPLOG));
    mRecvdflag = false;
    return WaitForSentence(PMTK_LOCUS_STARTSTOPACK);
}

/*!
    @brief Read the logger status
    @return True if we read the data, false if there was no response
*/
bool Adafruit_GPS::LOCUS_ReadStatus(void) {
    SendCommand(reinterpret_cast<const uint8_t *>(PMTK_LOCUS_QUERY_STATUS), strlen(PMTK_LOCUS_QUERY_STATUS));

    if (!WaitForSentence("$PMTKLOG")) return false;

    char *response = LastNMEA();
    uint16_t parsed[10];
    uint8_t i;

    for (i = 0; i < 10; i++) parsed[i] = -1;

    response = strchr(response, ',');
    for (i = 0; i < 10; i++) {
        if (!response || (response[0] == 0) || (response[0] == '*')) break;
        response++;
        parsed[i] = 0;
        while ((response[0] != ',') && (response[0] != '*') && (response[0] != 0)) {
            parsed[i] *= 10;
            char c = response[0];
            if (isdigit(c))
                parsed[i] += c - '0';
            else
                parsed[i] = c;
            response++;
        }
    }
    mLOCUS_serial = parsed[0];
    mLOCUS_type = parsed[1];
    if (isalpha(parsed[2])) {
        parsed[2] = parsed[2] - 'a' + 10;
    }
    mLOCUS_mode = parsed[2];
    mLOCUS_config = parsed[3];
    mLOCUS_interval = parsed[4];
    mLOCUS_distance = parsed[5];
    mLOCUS_speed = parsed[6];
    mLOCUS_status = !parsed[7];
    mLOCUS_records = parsed[8];
    mLOCUS_percent = parsed[9];

    return true;
}

/*!
    @brief Standby Mode Switches
    @return False if already in Standby, true if it entered Standby
*/
bool Adafruit_GPS::Standby(void) {
    if (mInStandbyMode) {
        return false;  // Returns false if already in Standby mode, so that you
                       // do not wake it up by sending commands to GPS
    } else {
        mInStandbyMode = true;
        SendCommand(reinterpret_cast<const uint8_t *>(PMTK_STANDBY), strlen(PMTK_STANDBY));
        // return WaitForSentence(PMTK_STANDBY_SUCCESS);  // don't seem to be
        // fast enough to catch the message, or something else just is not
        // working
        return true;
    }
}

/*!
    @brief Wake the sensor up
    @return True if woken up, false if not in Standby or failed to wake
*/
bool Adafruit_GPS::Wakeup(void) {
    if (mInStandbyMode) {
        mInStandbyMode = false;
        // send byte to wake it up
        SendCommand(reinterpret_cast<const uint8_t *>(" "), 1);
        return WaitForSentence(PMTK_AWAKE);
    } else {
        return false;  // Returns false if not in Standby mode, nothing to Wakeup
    }
}

/*!
    @brief Time in mSeconds since the last position mFix was obtained. The
    time returned is limited to 2^32 mMilliseconds, which is about 49.7 days.
    It will wrap around to zero if no position mFix is received
    for this long.
    @return nmea_float_t value in mSeconds since last mFix.
*/
nmea_float_t Adafruit_GPS::SecondsSinceFix() { return (millis() - mLastFix) / 1000.; }

/*!
    @brief Time in mSeconds since the last GPS time was obtained. The time
    returned is limited to 2^32 mMilliseconds, which is about 49.7 days. It
    will wrap around to zero if no GPS time is received for this long.
    @return nmea_float_t value in mSeconds since last GPS time.
*/
nmea_float_t Adafruit_GPS::SecondsSinceTime() { return (millis() - mLastTime) / 1000.; }

/*!
    @brief Time in mSeconds since the last GPS date was obtained. The time
    returned is limited to 2^32 mMilliseconds, which is about 49.7 days. It
    will wrap around to zero if no GPS date is received for this long.
    @return nmea_float_t value in mSeconds since last GPS date.
*/
nmea_float_t Adafruit_GPS::SecondsSinceDate() { return (millis() - mLastDate) / 1000.; }

/*!
    @brief Fakes time of receipt of a sentence. Use between build() and parse()
    to make the timing look like the sentence arrived from the GPS.
*/
void Adafruit_GPS::ResetSentTime() { mSentTime = millis(); }