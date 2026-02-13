#include "Arduino.h"
#include <SoftwareSerial.h>
#include "DFRobotDFPlayerMini.h"
#include "menorah_strings.h"

#ifndef SERIAL_WAIT_TIME
#define SERIAL_WAIT_TIME 100
#endif

/**
 prayer files from
  1st blessing https://www.youtube.com/watch?v=_fb-tkAet5A
  2nd blessing https://www.youtube.com/watch?v=6n3aQlSSkCM
  3rd blessing (first night) https://www.youtube.com/watch?v=LEO0z5jfYtw
 **/

// Using a soft serial port
#define RX_PIN 11
#define TX_PIN 12

SoftwareSerial softSerial(TX_PIN, RX_PIN);
#define FPSerial softSerial

DFRobotDFPlayerMini myDFPlayer;

bool setupMP3Player()
{
  FPSerial.begin(9600);
  if (!myDFPlayer.begin(FPSerial, /*isACK = */ true, /*doReset = */ true))
  { // Use serial to communicate with mp3.
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    
    return false;
  }
  
  myDFPlayer.volume(30);
  myDFPlayer.EQ(DFPLAYER_EQ_NORMAL);
  myDFPlayer.outputDevice(DFPLAYER_DEVICE_SD);

  Serial.println(F("mp3 initialized"));
  delay(SERIAL_WAIT_TIME);
  return true;
}

void playFile(int fileNumber)
{
  delay(SERIAL_WAIT_TIME);
  Serial.println((String)F("Playing file ") + fileNumber);
  delay(SERIAL_WAIT_TIME);
  myDFPlayer.play(fileNumber);
  delay(32000); // each prayer is under 32 seconds
}

void printMP3BoardDetail()
{

  //----Read information----
  Serial.println(DASHLINE);
  Serial.println((String)F("State: ") + myDFPlayer.readState());             // read mp3 state
  Serial.println((String)F("Volume: ") + myDFPlayer.readVolume());            // read current volume
  Serial.println((String)F("EQ: ") + myDFPlayer.readEQ());                // read EQ setting
  Serial.println((String)F("FileCounts: ") + myDFPlayer.readFileCounts());        // read all file counts in SD card
  Serial.println((String)F("File Number: ") + myDFPlayer.readCurrentFileNumber()); // read current play file number
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