#include <EEPROM.h>
#include <Wire.h>
#include "Menorah.h"

// 12 & 11 are the mp3 board
// a4 a5 are the rtc board, unless this board has dedicated SDA and SCL pins, in which case, use those.
#define CANDLE_1_PIN 6
#define CANDLE_2_PIN 10 // pin 7 on the original arduino was burnt out
#define CANDLE_3_PIN 8
#define CANDLE_4_PIN 9
#define CANDLE_5_PIN 5
#define CANDLE_6_PIN 4
#define CANDLE_7_PIN 3
#define CANDLE_8_PIN 2
#define CANDLE_SHAMASH_PIN 13

#define SERIAL_WAIT_TIME 100 // this is a pause so the terminal can catch up

#define QUIET_HOURS_START_HOUR 20
#define QUIET_HOURS_END_HOUR 10

#define DATE __DATE__
#define TIME __TIME__

byte hanukkahDay = 1;
bool isNewHanukkahDay = false;
bool soundBoardPresent = true;
String lastLitTimeStamp = INITIAL_TIMESTAMP;

struct CandleState {
  byte pin;
  bool state;
};

CandleState candleStates[] = {
  {CANDLE_1_PIN, HIGH},
  {CANDLE_2_PIN, HIGH},
  {CANDLE_3_PIN, HIGH},
  {CANDLE_4_PIN, HIGH},
  {CANDLE_5_PIN, HIGH},
  {CANDLE_6_PIN, HIGH},
  {CANDLE_7_PIN, HIGH},
  {CANDLE_8_PIN, HIGH},
  {CANDLE_SHAMASH_PIN, HIGH}
};

void setup() {
  pinMode(CANDLE_1_PIN, OUTPUT);
  pinMode(CANDLE_2_PIN, OUTPUT);
  pinMode(CANDLE_3_PIN, OUTPUT);
  pinMode(CANDLE_4_PIN, OUTPUT);
  pinMode(CANDLE_5_PIN, OUTPUT);
  pinMode(CANDLE_6_PIN, OUTPUT);
  pinMode(CANDLE_7_PIN, OUTPUT);
  pinMode(CANDLE_8_PIN, OUTPUT);
  pinMode(CANDLE_SHAMASH_PIN, OUTPUT);

  blankOutCandles();
  Wire.begin();
  Serial.begin(9600);
  
  checkForCommandOnSerial();

  soundBoardPresent = setupMP3Player();

  getHanukkahDayFromTime();

  lightCandles();
}

void getHanukkahDayFromTime() {
  byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;
  readTime(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month, &year);

  hanukkahDay = getHanukkahDay(year - 24, month - 11, dayOfMonth);

  if (hanukkahDay == 0) {
    Serial.println(F("Today isn't Hanukkah. But we'll light the candles."));
    delay(SERIAL_WAIT_TIME);
    hanukkahDay = 8;
    isNewHanukkahDay = false;
  } else if ((hour > QUIET_HOURS_START_HOUR) || (hour < QUIET_HOURS_END_HOUR)) { 
    // Get Yesterday's Hanukkah Day
    if (dayOfMonth != 1) {
      hanukkahDay = getHanukkahDay(year - 24, month - 11, dayOfMonth - 1);
    } else if (month != 11) {
      hanukkahDay = getHanukkahDay(year - 24, month - 10, 31);
    } else {
      hanukkahDay = getHanukkahDay(year - 24, month - 10, 30);
    }

    Serial.println(F("It's during quiet hours.  We don't count this as a new Hanukkah Day."));
    isNewHanukkahDay = false;
  } else {
    isNewHanukkahDay = true;
  }
}

void loop() {
  checkForCommandOnSerial();
  byte lastHanukkahDay = hanukkahDay;
  getHanukkahDayFromTime();

  // If this is a new hanukkah day or we haven't lit the menorah yet at all (or somehow got into a bad state, because the Shamash should always be lit)
  if ((lastHanukkahDay != hanukkahDay) || (candleStates[8].state == HIGH))  {
    lightCandles();
  }
}

void blankOutTerminal() {
  for (int i = 0; i < 50; i++) { // Print 50 new lines
    Serial.println();
  }
}

void checkForCommandOnSerial() {
  blankOutTerminal();
  Serial.println((String) getDisplayTime() + "\nCompile Time: " + DATE + TIME + F("\nThe Menorah was last lit: ") + lastLitTimeStamp + F("\n\nMain Menu\n1: Set Clock\n2: Display Time\n3: Play a prayer\n4: Test A Hanukkah Day (with or without prayers)\n5: Show the state of the menorah.\n6: Print MP3 Board Diagnostic Info.\n7: Test each candle 1 by 1.\n\nYou have 5 seconds to enter a command (press enter when done)"));
  int start = millis();
  int now = millis();
  while (now - start <= 5000) { 
    now = millis();
    if (Serial.available() > 0)  {
      byte input = getSerialInput(NEWLINE);

      // this can be expanded with other commands.
      if (input == 1) {
        // set time
        inputTime();
      } else if (input == 2) {
        // display time
        displayTime();
      } else if (input == 3) {
        // play a sound
        if (soundBoardPresent) {
          playFile(1);
        } else {
          Serial.println(F("Sound Board Not Present.  If you believe this is an error, check code and wiring."));
        }
      } else if (input == 4) {
        // test a hanukkah day w/ or w/out prayers
        Serial.println(F("You Must Restart the Arduino after using this command."));
        hanukkahDay = getSerialInput(TEST_DAY_MESSAGE);
        byte sayPrayers = getSerialInput(SAY_PRAYERS_MESSAGE);
        isNewHanukkahDay = false;
        if (sayPrayers == 1) {
          isNewHanukkahDay = true;
        }
        lightCandles();
        byte waitToContinue = getSerialInput(WAIT_TO_CONTINUE_MESSAGE);
      } else if (input == 5) {
        // print the state of the menorah
        printMenorahToSerial();
      } else if (input == 6) {
        // get mp3 board diagnostics
        printMP3BoardDetail();
      } else if (input == 7) {
        // test one by one candle
        Serial.println(F("You Must Restart the Arduino after using this command."));
        for (byte i = 0; i < 9; i++) {
          digitalWrite(candleStates[i].pin, HIGH);
        }
        for (byte i = 0; i < 9; i++) {
          digitalWrite(candleStates[i].pin, LOW);
          delay(1000);
          digitalWrite(candleStates[i].pin, HIGH);
        }
      }
    }
  }
  Serial.println(CONTINUING_MESSAGE);
}

void printMenorahToSerial() {
  Serial.print(F("\n         "));
  delay(SERIAL_WAIT_TIME);
  if (candleStates[8].state == LOW) {
    Serial.print(LIT_CANDLE);
  } else {
    Serial.print(UNLIT_CANDLE);
  }
  delay(SERIAL_WAIT_TIME);
  Serial.println(F("\n        | |"));
  delay(SERIAL_WAIT_TIME);

  for (byte i = 0; i < 8; i++) {
    if (candleStates[i].state == LOW) {
      Serial.print(LIT_CANDLE);
      delay(SERIAL_WAIT_TIME);
    } else {
      Serial.print(UNLIT_CANDLE);
      delay(SERIAL_WAIT_TIME);
    }
    if (i == 3) {
      Serial.print(F("_|S|"));
      delay(SERIAL_WAIT_TIME);
    }
    if (i < 7) {
      Serial.print(UNDERSCORE);
      delay(SERIAL_WAIT_TIME);
    }
  }
  // print out the base
  Serial.println(MENORAH_BASE);
}

void blankOutCandles() {
  // output the state of each candle to turn the relay on or off for each candle
  for (byte i = 0; i < 8; i++) {
    candleStates[i].state = HIGH;
    digitalWrite(candleStates[i].pin, candleStates[i].state);
  }

  lastLitTimeStamp = getDisplayTime();
}

void lightCandles() {

  lastLitTimeStamp = getDisplayTime();
  Serial.println((String)F("Lighting Candles for Hanukkah day ") + hanukkahDay);
  delay(SERIAL_WAIT_TIME);

  // Start with all candles (other than Shamash) unlit
  blankOutCandles();

  if (isNewHanukkahDay) {
    // say the prayers
    Serial.println(F("בָּרוּךְ אַתָּה אֲדֹנָי אֱלֹהֵינוּ מֶלֶךְ הָעוֹלָם אֲשֶׁר קִדְּשָׁנוּ בְּמִצְוֹתָיו וְצִוָּנוּ לְהַדְלִיק נֵר חֲנֻכָּה"));
    delay(SERIAL_WAIT_TIME);
    if (soundBoardPresent) {
      playFile(1);
    }

    Serial.println(F("בָּרוּךְ אַתָּה אֲדֹנָי אֱלֹהֵינוּ מֶלֶךְ הָעוֹלָם שֶׁעָשָׂה נִסִּים לַאֲבוֹתֵינוּ בַּיָּמִים הָהֵם בִּזְּמַן הַזֶּה"));
    delay(SERIAL_WAIT_TIME);
    if (soundBoardPresent) {
      playFile(2);
    }
    if (hanukkahDay == 1) {
      Serial.println(F("בָּרוּךְ אַתָּה אֲדֹנָי אֱלֹהֵינוּ מֶלֶךְ הָעוֹלָם שֶׁהֶחֱיָנוּ וְקִיְּמָנוּ וְהִגִּיעָנוּ לִזְּמַן הַזֶּה"));
      delay(SERIAL_WAIT_TIME);
      if (soundBoardPresent) {
        playFile(3);
      }
    }
  }

  candleStates[8].state = LOW;
  // set the candle state for each of the nights including this night to LOW, lighting from right to left
  for (byte i = 0; i < hanukkahDay; i++) {
    candleStates[i].state = LOW;
  }

  // output the state of each candle to turn the relay on or off for each candle
  for (byte i = 0; i < 8; i++) {
    digitalWrite(candleStates[i].pin, candleStates[i].state);
  }
  // set the shamash
  digitalWrite(candleStates[8].pin, candleStates[8].state);
  printMenorahToSerial();
}