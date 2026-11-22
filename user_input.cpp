#include "Arduino.h"
#include "menorah_strings.h"

#define DIGIT_GAP_MS 250 // end of a number if no further digit arrives within this long

static void discardSerialInput()
{
    while (Serial.available() > 0)
    {
        Serial.read();
    }
}

bool serialNumberPending()
{
    while (Serial.available() > 0 && !isDigit(Serial.peek()))
    {
        Serial.read(); // stray line endings, spaces, etc.
    }
    return Serial.available() > 0;
}

byte readSerialNumber()
{
    // wait for the first digit, ignoring anything else
    while (!serialNumberPending())
    {
    }

    unsigned int value = 0;
    unsigned long lastDigit = millis();
    while (millis() - lastDigit < DIGIT_GAP_MS)
    {
        if (Serial.available() > 0)
        {
            int c = Serial.peek();
            if (!isDigit(c))
            {
                break;
            }
            Serial.read();
            if (value < 255)
            {
                value = value * 10 + (c - '0');
            }
            lastDigit = millis();
        }
    }

    // consume the line ending (\r, \n or both) so it can't satisfy the next prompt
    delay(10);
    while (Serial.available() > 0 && (Serial.peek() == '\r' || Serial.peek() == '\n'))
    {
        Serial.read();
    }

    return value > 255 ? 255 : (byte)value;
}

byte getSerialInput(const __FlashStringHelper *message)
{
    discardSerialInput();
    Serial.print(message);
    byte value = readSerialNumber();
    Serial.print(F(" "));
    Serial.println(value);
    return value;
}

void waitForSerialKey(const __FlashStringHelper *message)
{
    discardSerialInput();
    Serial.print(message);
    while (Serial.available() == 0)
    {
    }
    delay(50); // let the rest of the line ending arrive
    discardSerialInput();
}
