#include <EEPROM.h>
#include <Wire.h>
#include "Menorah.h"

void runDiagnosticMenu();
void lightCandles(bool sayPrayers, bool recordPrayers);
void lightForRealDay();
bool showMenuAndRunCommand();
void writeCandlePins();
void restoreRealMenorahState();

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

#define MENU_IDLE_TIMEOUT_MS 60000UL

#define QUIET_HOURS_START_HOUR 20 // from this hour on, a reboot won't repeat the prayers
#define QUIET_HOURS_END_HOUR 10   // before this hour it is still last evening's menorah
#define CLOCK_CHECK_INTERVAL_MS 1000UL

// EEPROM layout: 3 bytes holding the date (year, month, day) the prayers were last said, so a
// power blip mid-evening doesn't repeat them.  EEPROM.update() only writes bytes that changed,
// so this is roughly one write per night; the EEPROM is rated for ~100,000 writes per cell.
#define PRAYER_RECORD_ADDRESS 0

#define DATE __DATE__
#define TIME __TIME__

byte hanukkahDay = 1;
bool isNewHanukkahDay = false;
#define MP3_PRAYER_WAIT_MS 10000 // max extra wait for the mp3 board before skipping prayers
char lastLitTimeStamp[32] = "00/00/00 00:00:00";
byte todayYear = 0, todayMonth = 0, todayDay = 0; // set by getHanukkahDayFromTime()

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
  // Drive each relay pin HIGH (off) *before* making it an output, so the relays never see a glitch.
  for (byte i = 0; i < 9; i++) {
    digitalWrite(candleStates[i].pin, HIGH);
    pinMode(candleStates[i].pin, OUTPUT);
  }

  Wire.begin();
  Serial.begin(9600);
  blankOutCandles();

  // Start the mp3 board booting now; it is serviced from loop() and lightCandles().
  mp3Begin();

  Serial.println(F("Menorah starting. Send any character at any time to open the diagnostic menu."));

  getHanukkahDayFromTime();

  lightForRealDay();
}

void getHanukkahDayFromTime() {
  byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;
  readTime(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month, &year);
  todayYear = year;
  todayMonth = month;
  todayDay = dayOfMonth;

  // The menorah is lit in the evening, so the small hours of the morning still
  // belong to the previous date's night.
  byte nightYear = year, nightMonth = month, nightDay = dayOfMonth;
  if (hour < QUIET_HOURS_END_HOUR) {
    previousDate(&nightYear, &nightMonth, &nightDay);
  }

  hanukkahDay = getHanukkahDayForDate(nightYear, nightMonth, nightDay);
  bool quietHours = (hour >= QUIET_HOURS_START_HOUR) || (hour < QUIET_HOURS_END_HOUR);

  if (hanukkahDay == 0) {
    // Not Hanukkah: light all 8 candles, no prayers.
    hanukkahDay = 8;
    isNewHanukkahDay = false;
  } else {
    // Prayers are only for a fresh lighting, not for a reboot during quiet hours.
    isNewHanukkahDay = !quietHours;
  }
}

bool prayersAlreadySaidToday() {
  return EEPROM.read(PRAYER_RECORD_ADDRESS) == todayYear
      && EEPROM.read(PRAYER_RECORD_ADDRESS + 1) == todayMonth
      && EEPROM.read(PRAYER_RECORD_ADDRESS + 2) == todayDay;
}

void recordPrayersSaidToday() {
  EEPROM.update(PRAYER_RECORD_ADDRESS, todayYear);
  EEPROM.update(PRAYER_RECORD_ADDRESS + 1, todayMonth);
  EEPROM.update(PRAYER_RECORD_ADDRESS + 2, todayDay);
}

// Light the menorah for the real (clock-derived) day, saying the prayers only if they're due.
void lightForRealDay() {
  bool sayPrayers = isNewHanukkahDay && !prayersAlreadySaidToday();
  if (isNewHanukkahDay && !sayPrayers) {
    Serial.println(F("Prayers were already said tonight; not repeating them."));
  }
  lightCandles(sayPrayers, /*recordPrayers = */ true);
}

void loop() {
  mp3Service();

  // Any serial input interrupts normal operation and opens the diagnostic menu.
  if (Serial.available() > 0) {
    runDiagnosticMenu();
  }

  // Check the clock once a second rather than as fast as the loop can spin.
  static unsigned long lastClockCheck = 0;
  if (millis() - lastClockCheck < CLOCK_CHECK_INTERVAL_MS) {
    return;
  }
  lastClockCheck = millis();

  byte lastHanukkahDay = hanukkahDay;
  getHanukkahDayFromTime();

  // If this is a new hanukkah day or we haven't lit the menorah yet at all (or somehow got into a bad state, because the Shamash should always be lit)
  if ((lastHanukkahDay != hanukkahDay) || (candleStates[8].state == HIGH))  {
    lightForRealDay();
  }
}

void blankOutTerminal() {
  for (int i = 0; i < 50; i++) { // Print 50 new lines
    Serial.println();
  }
}

void runDiagnosticMenu() {
  // discard whatever keystroke(s) triggered the menu
  delay(50);
  while (Serial.available() > 0) {
    Serial.read();
  }

  while (showMenuAndRunCommand()) {
  }
  Serial.println(CONTINUING_MESSAGE);
}

// Shows the menu and runs one command.  Returns false to leave diagnostic mode
// (command 0, or no input for MENU_IDLE_TIMEOUT_MS).
bool showMenuAndRunCommand() {
  blankOutTerminal();
  char timeText[32];
  formatDisplayTime(timeText, sizeof(timeText));
  Serial.println(timeText);
  Serial.print(F("Compile Time: "));
  Serial.print(F(DATE));
  Serial.print(F(" "));
  Serial.println(F(TIME));
  Serial.print(F("The Menorah was last lit: "));
  Serial.println(lastLitTimeStamp);
  Serial.println(F("\nMain Menu\n0: Exit (resume normal operation)\n1: Set Clock\n2: Display Time\n3: Play a prayer\n4: Test A Hanukkah Day (with or without prayers)\n5: Show the state of the menorah.\n6: Print MP3 Board Diagnostic Info.\n7: Test each candle 1 by 1.\n\nEnter a command (press enter when done). Menu exits after 60 seconds of inactivity."));
  unsigned long start = millis();
  while (millis() - start <= MENU_IDLE_TIMEOUT_MS) {
    mp3Service();
    if (Serial.available() > 0)  {
      byte input = getSerialInput(NEWLINE);

      if (input == 0) {
        return false;
      }

      // this can be expanded with other commands.
      if (input == 1) {
        // set time
        inputTime();
      } else if (input == 2) {
        // display time
        displayTime();
      } else if (input == 3) {
        // play a sound
        if (mp3Ready()) {
          playFile(1);
        } else {
          Serial.println(F("Sound Board Not Present.  If you believe this is an error, check code and wiring."));
        }
      } else if (input == 4) {
        // test a hanukkah day w/ or w/out prayers, then restore the real state
        byte testDay = getSerialInput(TEST_DAY_MESSAGE);
        if (testDay < 1 || testDay > 8) {
          Serial.println(F("Day must be between 1 and 8."));
        } else {
          byte sayPrayers = getSerialInput(SAY_PRAYERS_MESSAGE);
          hanukkahDay = testDay;
          lightCandles(sayPrayers == 1, /*recordPrayers = */ false);
          getSerialInput(WAIT_TO_CONTINUE_MESSAGE);
          restoreRealMenorahState();
        }
      } else if (input == 5) {
        // print the state of the menorah
        printMenorahToSerial();
      } else if (input == 6) {
        // get mp3 board diagnostics
        printMP3BoardDetail();
      } else if (input == 7) {
        // test one by one candle, then put the relays back to match the menorah state
        for (byte i = 0; i < 9; i++) {
          digitalWrite(candleStates[i].pin, HIGH);
        }
        for (byte i = 0; i < 9; i++) {
          digitalWrite(candleStates[i].pin, LOW);
          delay(1000);
          digitalWrite(candleStates[i].pin, HIGH);
        }
        writeCandlePins();
      }
      return true; // command handled; redisplay the menu
    }
  }
  return false; // idle timeout
}

// Drive the relays from candleStates (used after a test that touched the pins directly).
void writeCandlePins() {
  for (byte i = 0; i < 9; i++) {
    digitalWrite(candleStates[i].pin, candleStates[i].state);
  }
}

// After a test day, re-derive the real Hanukkah day from the clock and relight
// without repeating the prayers.
void restoreRealMenorahState() {
  getHanukkahDayFromTime();
  lightCandles(false, /*recordPrayers = */ false);
}

void printMenorahToSerial() {
  // build the candle row in one buffer: c_c_c_c_|S|_c_c_c_c
  char row[24];
  byte n = 0;
  for (byte i = 0; i < 8; i++) {
    row[n++] = (candleStates[i].state == LOW) ? LIT_CANDLE : UNLIT_CANDLE;
    if (i == 3) {
      row[n++] = '_';
      row[n++] = '|';
      row[n++] = 'S';
      row[n++] = '|';
    }
    if (i < 7) {
      row[n++] = '_';
    }
  }
  row[n] = '\0';

  Serial.print(F("\n         "));
  Serial.println((candleStates[8].state == LOW) ? LIT_CANDLE : UNLIT_CANDLE);
  Serial.println(F("        | |"));
  Serial.print(row);
  Serial.println(MENORAH_BASE);
  delay(SERIAL_WAIT_TIME);
}

void blankOutCandles() {
  // output the state of each candle to turn the relay on or off for each candle
  for (byte i = 0; i < 8; i++) {
    candleStates[i].state = HIGH;
    digitalWrite(candleStates[i].pin, candleStates[i].state);
  }

  formatDisplayTime(lastLitTimeStamp, sizeof(lastLitTimeStamp));
}

void lightCandles(bool sayPrayers, bool recordPrayers) {

  formatDisplayTime(lastLitTimeStamp, sizeof(lastLitTimeStamp));
  Serial.print(F("Lighting Candles for Hanukkah day "));
  Serial.println(hanukkahDay);
  delay(SERIAL_WAIT_TIME);

  // Start with all candles (other than Shamash) unlit
  blankOutCandles();

  if (sayPrayers) {
    // say the prayers (give the mp3 board a bounded chance to finish booting, then move on)
    bool prayersAudible = mp3WaitReady(MP3_PRAYER_WAIT_MS);
    if (!prayersAudible) {
      Serial.println(F("mp3 board not ready; lighting without audio prayers."));
    } else if (recordPrayers) {
      recordPrayersSaidToday();
    }
    Serial.println(F("בָּרוּךְ אַתָּה אֲדֹנָי אֱלֹהֵינוּ מֶלֶךְ הָעוֹלָם אֲשֶׁר קִדְּשָׁנוּ בְּמִצְוֹתָיו וְצִוָּנוּ לְהַדְלִיק נֵר חֲנֻכָּה"));
    delay(SERIAL_WAIT_TIME);
    if (prayersAudible) {
      playFile(1);
    }

    Serial.println(F("בָּרוּךְ אַתָּה אֲדֹנָי אֱלֹהֵינוּ מֶלֶךְ הָעוֹלָם שֶׁעָשָׂה נִסִּים לַאֲבוֹתֵינוּ בַּיָּמִים הָהֵם בִּזְּמַן הַזֶּה"));
    delay(SERIAL_WAIT_TIME);
    if (prayersAudible) {
      playFile(2);
    }
    if (hanukkahDay == 1) {
      Serial.println(F("בָּרוּךְ אַתָּה אֲדֹנָי אֱלֹהֵינוּ מֶלֶךְ הָעוֹלָם שֶׁהֶחֱיָנוּ וְקִיְּמָנוּ וְהִגִּיעָנוּ לִזְּמַן הַזֶּה"));
      delay(SERIAL_WAIT_TIME);
      if (prayersAudible) {
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