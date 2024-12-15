# Temple Shir Tikva Menorah Controller

Load this code on an arduino to control a sainsmart relay board which can be used to automatically turn menorah lights on and off in succession.

The controller will keep track of time and know which day of Hanukkah it is supposed to be (between 2024 and 2043).

The button can be used to set the time if it is set incorrectly.

## During Hanukkah

If you are monitoring the serial output of the arduino, you will see it display what it thinks the menorah is supposed to be showing and see that it is saying the prayers before lighting the candles just in case no one else is there to say them.
It is also saying the prayers using an mp3 player module so someone listening could hear it speaking the prayers.

## Other nights
If the menorah is being controlled on non-hanukkah nights, it lights up all 8 candles

## Setting the time
The `inputTime()` method can be used to set the real time clock chip's time using the serial connection.  Be sure not to send double line endings by setting "No Line Ending" in the Arduino IDE.

## Prayer Sound Files
The sound files should be copied to an SD card which is 32gb or less formatted in FAT16 or FAT32.  They should be names with 4 digits as the lead of the file name (e.g. `0001.mp3`) and placed in a folder named `mp3` in the root of the sd card.

## Parts List
* DFPlayer Audio Music Module for Arduino https://www.amazon.com/dp/B08FFLH5XL?ref=ppx_yo2ov_dt_b_fed_asin_title&th=1
* HiLetgo 5pcs DS3231 High Precision RTC Real Time Clock Module https://www.amazon.com/dp/B01N1LZSK3?psc=1&smid=A30QSGOJR8LMXA&ref_=chk_typ_imgToDp
* Arduino Uno https://www.amazon.com/Arduino-A000066-ARDUINO-UNO-R3/dp/B008GRTSV6?crid=P0FCQ22V3FZT&dib=eyJ2IjoiMSJ9.MazmhFfn-DF8W5oyX_S-tDFAqLRDaMJSkroaZhdQMdgGxJujWYy51xVRihYRF1ezRrtJ-qByH-U1X8qhZiyjdX93sX8KDwJX4Vv0L7iEq8K9WuU9q8dO5m5DPJkUSHe07yBbz5Rz-isUq88MNhsr4XVCxpwwcu4XyjQXw4ktsxGVKCMboqtVG-p9TlsvXIoEhqV67LNCO3PjX2sEgkN85nuvrQS1DNPaOf40qkhsoSmJat3iE7BYru8jdhtEjKpavbVESGvKxYLBLwS2jZr5iFX2HYLeTueK03srczb9tCmdbL6TG_6V_0b0KgygJDc2-FQ_zEE0fjzscyuo-dqF1Py2GKinREPu7lf3AhaK6Wc.OxZLVwwsgU3AA9Y6fsc8F62m4oTB1HacpDt_Ea00zSA&dib_tag=se&keywords=arduino+uno&qid=1734288439&s=industrial&sprefix=arduino+uno%2Cindustrial%2C125&sr=1-3
* PNY 32gb SD Card https://www.amazon.com/dp/B07R8GVGN9?ref=ppx_yo2ov_dt_b_fed_asin_title
* Jumper Wires https://www.amazon.com/dp/B07GD2BWPY?ref=ppx_yo2ov_dt_b_fed_asin_title
* Waterproof project box https://www.amazon.com/dp/B0CQG2KM5X?ref=ppx_yo2ov_dt_b_fed_asin_title&th=1
* 12vdc 5a power supply https://www.amazon.com/dp/B07GFFG1BQ?ref=ppx_yo2ov_dt_b_fed_asin_title&th=1
* Sainsmart 16 channel 12v relay module https://www.amazon.com/dp/B0057OC66U?ref=ppx_yo2ov_dt_b_fed_asin_title&th=1
