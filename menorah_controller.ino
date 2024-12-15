#include <EEPROM.h>
#include <Wire.h>
#include "clock_controller.h"
#include "sound_controller.h"
#include "hanukkah_day.h"

#define CANDLE_1_PIN 9
#define CANDLE_2_PIN 8
#define CANDLE_3_PIN 10 // pin 7 on the arduino is burnt out
#define CANDLE_4_PIN 6
#define CANDLE_5_PIN 5
#define CANDLE_6_PIN 4
#define CANDLE_7_PIN 3
#define CANDLE_8_PIN 2

#define BUTTON_PIN 13

bool buttonState = LOW;
byte hanukkahDay = 1;
bool isActuallyHanukkah = false;

void setup() {
  Wire.begin();
  Serial.begin(9600);
  pinMode(CANDLE_1_PIN, OUTPUT);
  pinMode(CANDLE_2_PIN, OUTPUT);
  pinMode(CANDLE_3_PIN, OUTPUT);
  pinMode(CANDLE_4_PIN, OUTPUT);
  pinMode(CANDLE_5_PIN, OUTPUT);
  pinMode(CANDLE_6_PIN, OUTPUT);
  pinMode(CANDLE_7_PIN, OUTPUT);
  pinMode(CANDLE_8_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT);
  
  displayTime();
  setupMP3Player();

  getHanukkahDayFromTime();

  Serial.println("Today we light the candles for day " + hanukkahDay);
  lightCandles();
}

void getHanukkahDayFromTime() {
  byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;
  readTime(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month, &year);
  hanukkahDay = getHanukkahDay(year - 24, month - 11, dayOfMonth);

  if (hanukkahDay == 0) {
    Serial.println("Today isn't Hanukkah. But we'll light the candles.");
    hanukkahDay = 8;
    isActuallyHanukkah = false;
  } else {
    isActuallyHanukkah = true;
  }
}

void loop() {
  // check if the button state has changed since last loop, we want this available if we want to set the time
  checkButton();
  getHanukkahDayFromTime();
  lightCandles();
}

void checkButton() {
  bool buttonRead = digitalRead(BUTTON_PIN);
  if (buttonRead != buttonState) {
    if (buttonRead == HIGH) { // button became pushed
      inputTime();
    }
    buttonState = buttonRead;
  }
}

void lightCandles() {
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
    {CANDLE_8_PIN, HIGH}
  };

  Serial.println((String)"Lighting Candles for Hanukkah day " + hanukkahDay);

  if (isActuallyHanukkah) {
    // say the prayers
    Serial.println("בָּרוּךְ אַתָּה אֲדֹנָי אֱלֹהֵינוּ מֶלֶךְ הָעוֹלָם אֲשֶׁר קִדְּשָׁנוּ בְּמִצְוֹתָיו וְצִוָּנוּ לְהַדְלִיק נֵר חֲנֻכָּה");
    playFile(1);
    
    Serial.println("בָּרוּךְ אַתָּה אֲדֹנָי אֱלֹהֵינוּ מֶלֶךְ הָעוֹלָם שֶׁעָשָׂה נִסִּים לַאֲבוֹתֵינוּ בַּיָּמִים הָהֵם בִּזְּמַן הַזֶּה");
    playFile(2);
    
    if (hanukkahDay == 1) {
      Serial.println("בָּרוּךְ אַתָּה אֲדֹנָי אֱלֹהֵינוּ מֶלֶךְ הָעוֹלָם שֶׁהֶחֱיָנוּ וְקִיְּמָנוּ וְהִגִּיעָנוּ לִזְּמַן הַזֶּה");
      playFile(3);
    }
  }

  // set the candle state for each of the nights including this night to high, lighting from right to left
  for (byte i = 7; i > 7 - hanukkahDay; i--) {
    candleStates[i].state = LOW;
  }

  Serial.println("\n        | |");
  // output the state of each candle to turn the relay on or off for each candle
  for (byte i = 0; i < 8; i++) {
    digitalWrite(candleStates[i].pin, candleStates[i].state);
    if (candleStates[i].state == LOW) {
      Serial.print("^");
    } else {
      Serial.print("v");
    }
    if (i == 3) {
      Serial.print("_|S|");
    }
    if (i < 7) {
      Serial.print("_");
    }
  }
  // print out the base
  Serial.println("\n      _-| |-_\n      -------");
}