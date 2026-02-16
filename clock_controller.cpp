#include <Arduino.h>
#include <Wire.h>
#include "user_input.h"
#include "menorah_strings.h"

#ifndef SERIAL_WAIT_TIME
#define SERIAL_WAIT_TIME 100
#endif

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

String getDisplayTime()
{
  byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;
  // retrieve data from DS3231
  readTime(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month,
           &year);

  String outtime = "";

  switch (dayOfWeek)
  {
  case 1:
    outtime = (String) outtime + F("Sun");
    break;
  case 2:
    outtime = (String) outtime + F("Mon");
    break;
  case 3:
    outtime = (String) outtime + F("Tues");
    break;
  case 4:
    outtime = (String) outtime + F("Wednes");
    break;
  case 5:
    outtime = (String) outtime + F("Thurs");
    break;
  case 6:
    outtime = (String) outtime + F("Fri");
    break;
  case 7:
    outtime = (String) outtime + F("Satur");
    break;
  }
  
  outtime = (String) outtime + DAY + SPACE + month + SLASH + dayOfMonth + SLASH + year + SPACE + hour + COLON;
  
  if (minute < 10)
  {
    outtime = (String) outtime + ZERO;
  }
  
  outtime = (String) outtime + minute + COLON;

  if (second < 10)
  {
    outtime = (String) outtime + ZERO;
  }
  
  outtime = (String) outtime + second;

  return outtime;
}

void displayTime()
{
  Serial.println(getDisplayTime());
  delay(SERIAL_WAIT_TIME);
}

void inputTime()
{
  Serial.println(F("inputTime: "));
  byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;
  year = getSerialInput("Enter Year (0 to 99)");
  month = getSerialInput("Enter Month");
  dayOfMonth = getSerialInput("Enter date (1 to 31)");
  dayOfWeek = getSerialInput("Enter day of week (1=Sunday, 7=Saturday)");
  hour = getSerialInput("Enter hour");
  minute = getSerialInput("Enter minute");
  second = getSerialInput("Enter second");

  setTime(second, minute, hour, dayOfWeek, dayOfMonth, month, year);

  Serial.println(F("Time is now set to:"));
  delay(SERIAL_WAIT_TIME);
  displayTime();
}