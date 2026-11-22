#include "Arduino.h"
#include "menorah_strings.h"

byte getSerialInput(const __FlashStringHelper *message)
{
    Serial.print(message);
    delay(SERIAL_WAIT_TIME);
    while (Serial.available() == 0)
    {
    } // wait for the user to send something
    byte temp = Serial.parseInt();
    Serial.print(F(" "));
    Serial.println(temp);
    return temp;
}
