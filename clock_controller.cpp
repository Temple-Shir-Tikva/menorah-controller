#include <Arduino.h>
#include <Wire.h>

#ifndef SERIAL_WAIT_TIME
#define SERIAL_WAIT_TIME 100
#endif

/**
Code originally from https://www.instructables.com/Using-DS1307-and-DS3231-Real-time-Clock-Modules-Wi/
**/

#define DS3231_I2C_ADDRESS 0x68

// Convert normal decimal numbers to binary coded decimal
byte decToBcd(byte val) {
  return( (val/10*16) + (val%10) );
}

// Convert binary coded decimal to normal decimal numbers
byte bcdToDec(byte val) {
  return( (val/16*10) + (val%16) );
}

byte getSerialInput(String message) {
  Serial.println(message);
  delay(SERIAL_WAIT_TIME);
  while (Serial.available() == 0) {} // wait for the user to send something
  byte temp = Serial.parseInt();
  return temp;
}

void setTime(byte second, byte minute, byte hour, byte dayOfWeek, byte dayOfMonth, byte month, byte year) {
  // sets time and date data to DS3231
  // DS3231 seconds, minutes, hours, day, date, month, year
  
  Wire.beginTransmission(DS3231_I2C_ADDRESS);
  Wire.write(0); // set next input to start at the seconds register
  Wire.write(decToBcd(second)); // set seconds
  Wire.write(decToBcd(minute)); // set minutes
  Wire.write(decToBcd(hour)); // set hours
  Wire.write(decToBcd(dayOfWeek)); // set day of week (1=Sunday, 7=Saturday)
  Wire.write(decToBcd(dayOfMonth)); // set date (1 to 31)
  Wire.write(decToBcd(month)); // set month
  Wire.write(decToBcd(year)); // set year (0 to 99)
  Wire.endTransmission();
}

void readTime(byte *second, byte *minute, byte *hour, byte *dayOfWeek, byte *dayOfMonth, byte *month, byte *year) {
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

void displayTime() {
  byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;
  // retrieve data from DS3231
  readTime(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month,
  &year);
  
  switch(dayOfWeek) {
  case 1:
    Serial.print("Sunday");
    delay(SERIAL_WAIT_TIME);
    break;
  case 2:
    Serial.print("Monday");
    delay(SERIAL_WAIT_TIME);
    break;
  case 3:
    Serial.print("Tuesday");
    delay(SERIAL_WAIT_TIME);
    break;
  case 4:
    Serial.print("Wednesday");
    delay(SERIAL_WAIT_TIME);
    break;
  case 5:
    Serial.print("Thursday");
    delay(SERIAL_WAIT_TIME);
    break;
  case 6:
    Serial.print("Friday");
    delay(SERIAL_WAIT_TIME);
    break;
  case 7:
    Serial.print("Saturday");
    delay(SERIAL_WAIT_TIME);
    break;
  }
  Serial.print(" ");
  delay(SERIAL_WAIT_TIME);
  Serial.print(month, DEC);
  delay(SERIAL_WAIT_TIME);
  Serial.print("/");
  delay(SERIAL_WAIT_TIME);
  Serial.print(dayOfMonth, DEC);
  delay(SERIAL_WAIT_TIME);
  Serial.print("/");
  delay(SERIAL_WAIT_TIME);
  Serial.print(year, DEC);
  delay(SERIAL_WAIT_TIME);
  Serial.print(" ");
  delay(SERIAL_WAIT_TIME);
  Serial.print(hour, DEC);
  delay(SERIAL_WAIT_TIME);
  // convert the byte variable to a decimal number when displayed
  Serial.print(":");
  delay(SERIAL_WAIT_TIME);
  if (minute<10) {
    Serial.print("0");
    delay(SERIAL_WAIT_TIME);
  }
  Serial.print(minute, DEC);
  delay(SERIAL_WAIT_TIME);
  Serial.print(":");
  delay(SERIAL_WAIT_TIME);
  if (second<10) {
    Serial.print("0");
    delay(SERIAL_WAIT_TIME);
  }
  Serial.print(second, DEC);
  delay(SERIAL_WAIT_TIME);
  Serial.println();
  delay(SERIAL_WAIT_TIME);
}

void inputTime() {
  byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;
  year = getSerialInput("Enter Year (0 to 99)");
  month = getSerialInput("Enter Month");
  dayOfMonth = getSerialInput("Enter date (1 to 31)");
  dayOfWeek = getSerialInput("Enter day of week (1=Sunday, 7=Saturday)");
  hour = getSerialInput("Enter hour");
  minute = getSerialInput("Enter minute");
  second = getSerialInput("Enter second");
  
  setTime(second, minute, hour, dayOfWeek, dayOfMonth, month, year);

  Serial.println("Time is now set to:");
  delay(SERIAL_WAIT_TIME);
  displayTime();
}