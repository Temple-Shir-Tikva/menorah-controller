#include "Arduino.h"
#include <SoftwareSerial.h>
#include "DFRobotDFPlayerMini.h"
#include "menorah_strings.h"

/**
 prayer files from
  1st blessing https://www.youtube.com/watch?v=_fb-tkAet5A
  2nd blessing https://www.youtube.com/watch?v=6n3aQlSSkCM
  3rd blessing (first night) https://www.youtube.com/watch?v=LEO0z5jfYtw
 **/

// Using a soft serial port.  SoftwareSerial's constructor takes (rx, tx).
// These are the pins as wired and working; do not change the numbers.
#define MP3_ARDUINO_RX_PIN 12 // Arduino receives here (from the DFPlayer's TX)
#define MP3_ARDUINO_TX_PIN 11 // Arduino transmits here (to the DFPlayer's RX)

SoftwareSerial softSerial(MP3_ARDUINO_RX_PIN, MP3_ARDUINO_TX_PIN);
#define FPSerial softSerial

DFRobotDFPlayerMini myDFPlayer;

// ---- Non-blocking MP3 board initialization ----
// The DFPlayer needs 1-3 seconds after power-up to mount its SD card, and it
// announces itself ("card online") when ready.  Rather than blocking in setup(),
// mp3Begin() starts a state machine and mp3Service() advances it from wherever the
// sketch is waiting anyway (e.g. the startup menu window).  If the board never
// comes up, the candles still light; initialization keeps retrying in the background.
#define MP3_BOOT_DELAY_MS 1500     // listen for the board's own online message this long before forcing a reset
#define MP3_ONLINE_TIMEOUT_MS 3000 // after a reset, how long to wait for "card online"
#define MP3_SETTLE_MS 200          // pause between "online" and sending settings
#define MP3_MAX_ATTEMPTS 5         // attempts per round before backing off
#define MP3_RETRY_PAUSE_MS 30000   // pause between rounds of attempts
#define MP3_MIN_FILES 3            // the three prayers

enum Mp3State { MP3_BOOTING, MP3_AWAIT_ONLINE, MP3_SETTLING, MP3_READY, MP3_BACKOFF };

static Mp3State mp3State = MP3_BACKOFF;
static unsigned long mp3StateSince = 0;
static byte mp3Attempts = 0;

static void mp3SetState(Mp3State state)
{
  mp3State = state;
  mp3StateSince = millis();
}

static void mp3Fail(const __FlashStringHelper *reason)
{
  mp3Attempts++;
  Serial.print(F("mp3 init attempt "));
  Serial.print(mp3Attempts);
  Serial.print(F(" failed: "));
  Serial.println(reason);
  if (mp3Attempts >= MP3_MAX_ATTEMPTS)
  {
    Serial.println(F("mp3 board unavailable; will keep retrying in the background."));
    mp3SetState(MP3_BACKOFF);
  }
  else
  {
    mp3SetState(MP3_BOOTING);
  }
}

void mp3Begin()
{
  FPSerial.begin(9600);
  // doReset = false: begin() only attaches the stream and returns immediately.
  myDFPlayer.begin(FPSerial, /*isACK = */ true, /*doReset = */ false);
  mp3Attempts = 0;
  mp3SetState(MP3_BOOTING);
}

void mp3Service()
{
  if (mp3State == MP3_READY)
  {
    return;
  }

  bool online = false;
  if (myDFPlayer.available())
  {
    uint8_t type = myDFPlayer.readType();
    online = (type == DFPlayerCardOnline || type == DFPlayerCardUSBOnline);
  }

  unsigned long elapsed = millis() - mp3StateSince;

  switch (mp3State)
  {
  case MP3_BOOTING:
    if (online)
    {
      mp3SetState(MP3_SETTLING);
    }
    else if (elapsed >= MP3_BOOT_DELAY_MS)
    {
      myDFPlayer.reset();
      mp3SetState(MP3_AWAIT_ONLINE);
    }
    break;

  case MP3_AWAIT_ONLINE:
    if (online)
    {
      mp3SetState(MP3_SETTLING);
    }
    else if (elapsed >= MP3_ONLINE_TIMEOUT_MS)
    {
      mp3Fail(F("no response from board (check wiring/power/SD card)"));
    }
    break;

  case MP3_SETTLING:
    if (elapsed >= MP3_SETTLE_MS)
    {
      myDFPlayer.volume(30);
      myDFPlayer.EQ(DFPLAYER_EQ_NORMAL);
      myDFPlayer.outputDevice(DFPLAYER_DEVICE_SD);
      int files = myDFPlayer.readFileCounts();
      if (files < MP3_MIN_FILES)
      {
        mp3Fail(F("too few files found on SD card (need /mp3/0001-0003)"));
      }
      else
      {
        Serial.println(F("mp3 initialized"));
        mp3SetState(MP3_READY);
      }
    }
    break;

  case MP3_BACKOFF:
    if (elapsed >= MP3_RETRY_PAUSE_MS)
    {
      mp3Attempts = 0;
      myDFPlayer.reset();
      mp3SetState(MP3_AWAIT_ONLINE);
    }
    break;

  default:
    break;
  }
}

bool mp3Ready()
{
  return mp3State == MP3_READY;
}

// Waits up to maxMs for the board to become ready.  Returns immediately if the
// board has already given up for this round (so we never stall the candles).
bool mp3WaitReady(unsigned long maxMs)
{
  unsigned long start = millis();
  while (!mp3Ready() && mp3State != MP3_BACKOFF && (millis() - start) < maxMs)
  {
    mp3Service();
    delay(10);
  }
  return mp3Ready();
}

#define MP3_MAX_PRAYER_MS 40000UL // safety cap; each prayer is under 32 seconds

// Plays a file and returns when the board reports it finished (or the safety cap passes).
void playFile(int fileNumber)
{
  if (!mp3Ready())
  {
    Serial.println(F("mp3 board not ready; skipping prayer."));
    return;
  }
  delay(SERIAL_WAIT_TIME);
  Serial.print(F("Playing file "));
  Serial.println(fileNumber);
  delay(SERIAL_WAIT_TIME);

  while (myDFPlayer.available())
  {
    myDFPlayer.readType(); // drop stale events so an old "finished" can't end this prayer early
  }
  myDFPlayer.play(fileNumber);

  unsigned long start = millis();
  while (millis() - start < MP3_MAX_PRAYER_MS)
  {
    if (myDFPlayer.available())
    {
      uint8_t type = myDFPlayer.readType();
      if (type == DFPlayerPlayFinished)
      {
        return;
      }
      if (type == DFPlayerError)
      {
        Serial.println(F("mp3 board reported an error during playback."));
        return;
      }
    }
    delay(10);
  }
  Serial.println(F("Gave up waiting for the prayer to finish."));
}

static void printLabeled(const __FlashStringHelper *label, long value)
{
  Serial.print(label);
  Serial.println(value);
}

void printMP3BoardDetail()
{
  if (!mp3Ready())
  {
    Serial.println(F("mp3 board not ready."));
    return;
  }

  //----Read information----
  Serial.println(DASHLINE);
  printLabeled(F("State: "), myDFPlayer.readState());
  printLabeled(F("Volume: "), myDFPlayer.readVolume());
  printLabeled(F("EQ: "), myDFPlayer.readEQ());
  printLabeled(F("FileCounts: "), myDFPlayer.readFileCounts());
  printLabeled(F("File Number: "), myDFPlayer.readCurrentFileNumber());
  Serial.println(DASHLINE);

  uint8_t type;
  int value;

  if (myDFPlayer.available())
  {
    type = myDFPlayer.readType();
    value = myDFPlayer.read();

    switch (type)
    {
    case TimeOut:
      Serial.println(F("Time Out!"));
      break;
    case WrongStack:
      Serial.println(F("Stack Wrong!"));
      break;
    case DFPlayerCardInserted:
      Serial.println(F("Card Inserted!"));
      break;
    case DFPlayerCardRemoved:
      Serial.println(F("Card Removed!"));
      break;
    case DFPlayerCardOnline:
      Serial.println(F("Card Online!"));
      break;
    case DFPlayerUSBInserted:
      Serial.println(F("USB Inserted!"));
      break;
    case DFPlayerUSBRemoved:
      Serial.println(F("USB Removed!"));
      break;
    case DFPlayerPlayFinished:
      Serial.print(F("Number:"));
      Serial.print(value);
      Serial.println(F(" Play Finished!"));
      break;
    case DFPlayerError:
      Serial.print(F("DFPlayerError:"));
      switch (value)
      {
      case Busy:
        Serial.println(F("Card not found"));
        break;
      case Sleeping:
        Serial.println(F("Sleeping"));
        break;
      case SerialWrongStack:
        Serial.println(F("Get Wrong Stack"));
        break;
      case CheckSumNotMatch:
        Serial.println(F("Check Sum Not Match"));
        break;
      case FileIndexOut:
        Serial.println(F("File Index Out of Bound"));
        break;
      case FileMismatch:
        Serial.println(F("Cannot Find File"));
        break;
      case Advertise:
        Serial.println(F("In Advertise"));
        break;
      default:
        break;
      }
      break;
    default:
      break;
    }
  }
}