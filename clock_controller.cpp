#include <Arduino.h>
#include <Wire.h>
#include "user_input.h"
#include "menorah_strings.h"

/**
Code originally from https://www.instructables.com/Using-DS1307-and-DS3231-Real-time-Clock-Modules-Wi/
**/

#define DS3231_I2C_ADDRESS 0x68

// Convert normal decimal numbers to binary coded decimal
byte decToBcd(byte val)
{
  return ((val / 10 * 16) + (val % 10));
}

// Convert binary coded decimal to normal decimal numbers
byte bcdToDec(byte val)
{
  return ((val / 16 * 10) + (val % 16));
}

void setTime(byte second, byte minute, byte hour, byte dayOfWeek, byte dayOfMonth, byte month, byte year)
{
  // sets time and date data to DS3231
  // DS3231 seconds, minutes, hours, day, date, month, year

  Wire.beginTransmission(DS3231_I2C_ADDRESS);
  Wire.write(0);                    // set next input to start at the seconds register
  Wire.write(decToBcd(second));     // set seconds
  Wire.write(decToBcd(minute));     // set minutes
  Wire.write(decToBcd(hour));       // set hours
  Wire.write(decToBcd(dayOfWeek));  // set day of week (1=Sunday, 7=Saturday)
  Wire.write(decToBcd(dayOfMonth)); // set date (1 to 31)
  Wire.write(decToBcd(month));      // set month
  Wire.write(decToBcd(year));       // set year (0 to 99)
  Wire.endTransmission();
}

void readTime(byte *second, byte *minute, byte *hour, byte *dayOfWeek, byte *dayOfMonth, byte *month, byte *year)
{
  Wire.beginTransmission(DS3231_I2C_ADDRESS);
  Wire.write(0); // set DS3231 register pointer to 00h
  Wire.endTransmission();
  Wire.requestFrom(DS3231_I2C_ADDRESS, 7);
  // request seven bytes of data from DS3231 starting from register 00h
  *second = bcdToDec(Wire.read() & 0x7f);
  *minute = bcdToDec(Wire.read());
  *hour = bcdToDec(Wire.read() & 0x3f);
  *dayOfWeek = bcdToDec(Wire.read());
  *dayOfMonth = bcdToDec(Wire.read());
  *month = bcdToDec(Wire.read());
  *year = bcdToDec(Wire.read());
}

static const char DAY_SUN[] PROGMEM = "Sunday";
static const char DAY_MON[] PROGMEM = "Monday";
static const char DAY_TUE[] PROGMEM = "Tuesday";
static const char DAY_WED[] PROGMEM = "Wednesday";
static const char DAY_THU[] PROGMEM = "Thursday";
static const char DAY_FRI[] PROGMEM = "Friday";
static const char DAY_SAT[] PROGMEM = "Saturday";
static const char DAY_UNKNOWN[] PROGMEM = "???";
static const char *const DAY_NAMES[] PROGMEM = {DAY_SUN, DAY_MON, DAY_TUE, DAY_WED, DAY_THU, DAY_FRI, DAY_SAT};

// Writes the current RTC time into out, e.g. "Sunday 12/14/25 9:05:03".
void formatDisplayTime(char *out, size_t outSize)
{
  byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;
  readTime(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month, &year);

  const char *dayName = (dayOfWeek >= 1 && dayOfWeek <= 7)
                            ? (const char *)pgm_read_ptr(&DAY_NAMES[dayOfWeek - 1])
                            : DAY_UNKNOWN;

  snprintf_P(out, outSize, PSTR("%S %u/%u/%u %u:%02u:%02u"),
             dayName, month, dayOfMonth, year, hour, minute, second);
}

static byte daysInMonth(byte month, byte year)
{
  static const byte DAYS[] PROGMEM = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
  if (month < 1 || month > 12)
  {
    return 31;
  }
  if (month == 2 && year % 4 == 0) // valid for 2000-2099, which covers the RTC's range
  {
    return 29;
  }
  return pgm_read_byte(&DAYS[month - 1]);
}

void previousDate(byte *year, byte *month, byte *day)
{
  if (*day > 1)
  {
    (*day)--;
    return;
  }
  if (*month <= 1)
  {
    *month = 12;
    (*year)--;
  }
  else
  {
    (*month)--;
  }
  *day = daysInMonth(*month, *year);
}

void displayTime()
{
  char timeText[32];
  formatDisplayTime(timeText, sizeof(timeText));
  Serial.println(timeText);
  delay(SERIAL_WAIT_TIME);
}

void inputTime()
{
  Serial.println(F("inputTime: "));
  byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;
  year = getSerialInput(F("Enter Year (0 to 99)"));
  month = getSerialInput(F("Enter Month"));
  dayOfMonth = getSerialInput(F("Enter date (1 to 31)"));
  dayOfWeek = getSerialInput(F("Enter day of week (1=Sunday, 7=Saturday)"));
  hour = getSerialInput(F("Enter hour"));
  minute = getSerialInput(F("Enter minute"));
  second = getSerialInput(F("Enter second"));

  setTime(second, minute, hour, dayOfWeek, dayOfMonth, month, year);

  Serial.println(F("Time is now set to:"));
  delay(SERIAL_WAIT_TIME);
  displayTime();
}