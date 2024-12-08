#include <EEPROM.h>

#define CANDLE_1_PIN 9
#define CANDLE_2_PIN 8
#define CANDLE_3_PIN 10 // pin 7 on the arduino is burnt out
#define CANDLE_4_PIN 6
#define CANDLE_5_PIN 5
#define CANDLE_6_PIN 4
#define CANDLE_7_PIN 3
#define CANDLE_8_PIN 2

#define BUTTON_PIN 13
#define MILLIS_BUTTON_HOLD_DURATION 5000 // 5 seconds in milliseconds
#define MILLIS_MIN_BUTTON_HOLD_DURATION 100 // 100 milliseconds minimum to register a push

#define MILLIS_OUTSIDE_TIMER_IS_ACTIVE 21600000 // 6 hours in milliseconds
// #define MILLIS_OUTSIDE_TIMER_IS_ACTIVE 1000 // testing 5 seconds per day

int buttonState = LOW;
int buttonHoldTime = 0;

int hanukkahDay = 1;
int hanukkahDayEEPROMAddress = 0;

int runStartTime = 0;

void setup() {
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
  
  EEPROM.get(hanukkahDayEEPROMAddress, hanukkahDay);
  Serial.println((String)"Hanukkah Day Loaded from EEPROM: " + hanukkahDay);

  advanceHanukkahDay();

  runStartTime = millis();
  buttonState = LOW;
  buttonHoldTime = millis();
  lightCandles();
  
}

void loop() {
  // if this has been running for the duration of the outside timer, advance the day
  if (millis() >= runStartTime + MILLIS_OUTSIDE_TIMER_IS_ACTIVE) {
    advanceHanukkahDay();
    runStartTime = millis();
  }
  // check if the button state has changed since last loop
  checkButton();
}

void checkButton() {
  int buttonRead = digitalRead(BUTTON_PIN);
  int now = millis();
  if (buttonRead != buttonState) {
    Serial.println("Button State Change Detected");
    if (buttonRead == HIGH) { // button became pushed
      Serial.println("Start of push");
      buttonHoldTime = now;
    } else if ((now - buttonHoldTime) > MILLIS_MIN_BUTTON_HOLD_DURATION) { // button was let go
      Serial.println("Advance Day based on button push");
      advanceHanukkahDay();
    }
    buttonState = buttonRead;
  }
}

void lightCandles() {
  int candleStates[8][2] = {
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

  // say the prayers
  Serial.println("בָּרוּךְ אַתָּה אֲדֹנָי אֱלֹהֵינוּ מֶלֶךְ הָעוֹלָם אֲשֶׁר קִדְּשָׁנוּ בְּמִצְוֹתָיו וְצִוָּנוּ לְהַדְלִיק נֵר חֲנֻכָּה");
  Serial.println("בָּרוּךְ אַתָּה אֲדֹנָי אֱלֹהֵינוּ מֶלֶךְ הָעוֹלָם שֶׁעָשָׂה נִסִּים לַאֲבוֹתֵינוּ בַּיָּמִים הָהֵם בִּזְּמַן הַזֶּה");

  if (hanukkahDay == 1) {
    Serial.println("בָּרוּךְ אַתָּה אֲדֹנָי אֱלֹהֵינוּ מֶלֶךְ הָעוֹלָם שֶׁהֶחֱיָנוּ וְקִיְּמָנוּ וְהִגִּיעָנוּ לִזְּמַן הַזֶּה");
  }

  // set the candle state for each of the nights including this night to high
  for (int i = 7; i > 7 - hanukkahDay; i--) {
    candleStates[i][1] = LOW;
  }

  Serial.println("\n        | |");
  // output the state of each candle to turn the relay on or off for each candle
  for (int i = 0; i < 8; i++) {
    digitalWrite(candleStates[i][0], candleStates[i][1]);
    if (candleStates[i][1] == LOW) {
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

void resetHanukkahDay() {
  // reset to the first night
  Serial.println("Day Resetting to 1");
  hanukkahDay = 1;
  writeAndLight();
}

void writeAndLight() {
  int checkDayWritten = 0;
  EEPROM.put(hanukkahDayEEPROMAddress, hanukkahDay);
  EEPROM.get(hanukkahDayEEPROMAddress, checkDayWritten);
  Serial.println((String)"Check Day Written to EEPROM: " + hanukkahDay + " " + checkDayWritten);
  lightCandles();
}

void advanceHanukkahDay() {
  // set the night to the next one
  hanukkahDay++;
  if (hanukkahDay > 8) {
    resetHanukkahDay();
  } else { // we don't want to light twice because that also writes to the eeprom twice and the eeprom has a limited lifespan.
    writeAndLight();
  }
}