#include "Arduino.h"
#include "menorah_strings.h"

#ifndef SERIAL_WAIT_TIME
#define SERIAL_WAIT_TIME 100
#endif

byte getSerialInput(String message)
{
    Serial.print(message);
    delay(SERIAL_WAIT_TIME);
    while (Serial.available() == 0)
    {
    } // wait for the user to send something
    byte temp = Serial.parseInt();
    Serial.println((String)SPACE+temp);
    return temp;
}