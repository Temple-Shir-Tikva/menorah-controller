#ifndef MENORAH_STRINGS_H
#define MENORAH_STRINGS_H

#include <Arduino.h>

// Pause so the terminal can catch up after serial output.
// Defined once here because every .cpp file is compiled separately and needs it.
#ifndef SERIAL_WAIT_TIME
#define SERIAL_WAIT_TIME 100
#endif

// Text is kept in flash (F()) rather than RAM, and always printed directly,
// never concatenated into String objects.
#define LIT_CANDLE '^'
#define UNLIT_CANDLE 'v'
#define MENORAH_BASE F("\n      _-| |-_\n      -------")
#define DASHLINE F("------------------")
#define TEST_DAY_MESSAGE F("Enter Hanukkah Day to test (1-8):\n")
#define SAY_PRAYERS_MESSAGE F("Say Prayers? (1 = Yes, 0 = No)\n")
#define CONTINUING_MESSAGE F("Continuing.")
#define WAIT_TO_CONTINUE_MESSAGE F("The menorah should now be lit following the image.\nSend any character to restore normal operation.\n")
#endif
