#include "Arduino.h"

#ifndef SERIAL_WAIT_TIME
#define SERIAL_WAIT_TIME 100
#endif

/**
  The struct stores every day of hanukkah between 2024 and 2043.
  The first number is the number of years away from 2024 (eg 2024 = 0, 2025 = 1, etc)
  The second number is the month
  The third number is the day of the month (0 = Nov, 1 = Dec, 2 = Jan)
  The return value is what night of hanukkah it is. (or 0 if it isn't)
**/

struct HanukkahDay {
  byte yearOffset;  // 0-19
  byte monthOffset; // 0 (Nov), 1 (Dec), 2 (Jan)
  byte day;        // 1-31
  byte hanukkahDay; // 1-8
};

const HanukkahDay hanukkahDays[] PROGMEM = {
  // Testing
  /**
   John Legg - if you want to test, change the last number in these blocks to 1-8 (per days of Hanukkah)
  the 3rd number is the day of the month so I've put one day in this block for every day between 
  Dec 16th, 2024 and Dec 24th, 2024
     -- Jon Schwartz
        Dec 18th, 2024
  **/
  {0, 1, 16, 1}, {0, 1, 17, 4}, {0, 1, 18, 0}, {0, 1, 19, 0},
  {0, 1, 20, 0}, {0, 1, 21, 0}, {0, 1, 22, 0}, {0, 1, 23, 0},
  {0, 1, 24, 0},
   
  // 2024
  {0, 1, 25, 1}, {0, 1, 26, 2}, {0, 1, 27, 3}, {0, 1, 28, 4},
  {0, 1, 29, 5}, {0, 1, 30, 6}, {0, 1, 31, 7}, {1, 2, 1, 8}, // Corrected yearIndex to 1 for {1, 2, 1, 8}

  // 2025
  {1, 1, 14, 1}, {1, 1, 15, 2}, {1, 1, 16, 3}, {1, 1, 17, 4},
  {1, 1, 18, 5}, {1, 1, 19, 6}, {1, 1, 20, 7}, {1, 1, 21, 8},

  // 2026
  {2, 1, 4, 1}, {2, 1, 5, 2}, {2, 1, 6, 3}, {2, 1, 7, 4},
  {2, 1, 8, 5}, {2, 1, 9, 6}, {2, 1, 10, 7}, {2, 1, 11, 8},

  // 2027
  {3, 1, 24, 1}, {3, 1, 25, 2}, {3, 1, 26, 3}, {3, 1, 27, 4},
  {3, 1, 28, 5}, {3, 1, 29, 6}, {3, 1, 30, 7}, {4, 2, 1, 8}, // Corrected yearIndex to 4 for {4, 2, 1, 8}

  // 2028
  {4, 1, 12, 1}, {4, 1, 13, 2}, {4, 1, 14, 3}, {4, 1, 15, 4},
  {4, 1, 16, 5}, {4, 1, 17, 6}, {4, 1, 18, 7}, {4, 1, 19, 8},

  // 2029
  {5, 1, 1, 1}, {5, 1, 2, 2}, {5, 1, 3, 3}, {5, 1, 4, 4},
  {5, 1, 5, 5}, {5, 1, 6, 6}, {5, 1, 7, 7}, {5, 1, 8, 8},

  // 2030
  {6, 1, 20, 1}, {6, 1, 21, 2}, {6, 1, 22, 3}, {6, 1, 23, 4},
  {6, 1, 24, 5}, {6, 1, 25, 6}, {6, 1, 26, 7}, {6, 1, 27, 8},

  // 2031
  {7, 1, 9, 1}, {7, 1, 10, 2}, {7, 1, 11, 3}, {7, 1, 12, 4},
  {7, 1, 13, 5}, {7, 1, 14, 6}, {7, 1, 15, 7}, {7, 1, 16, 8},

  // 2032
  {8, 0, 27, 1}, {8, 0, 28, 2}, {8, 0, 29, 3}, {8, 0, 30, 4},
  {8, 1, 1, 5}, {8, 1, 2, 6}, {8, 1, 3, 7}, {8, 1, 4, 8},

  // 2033
  {9, 1, 16, 1}, {9, 1, 17, 2}, {9, 1, 18, 3}, {9, 1, 19, 4},
  {9, 1, 20, 5}, {9, 1, 21, 6}, {9, 1, 22, 7}, {9, 1, 23, 8},

  // 2034
  {10, 1, 6, 1}, {10, 1, 7, 2}, {10, 1, 8, 3}, {10, 1, 9, 4},
  {10, 1, 10, 5}, {10, 1, 11, 6}, {10, 1, 12, 7}, {10, 1, 13, 8},

  // 2035
  {11, 1, 25, 1}, {11, 1, 26, 2}, {11, 1, 27, 3}, {11, 1, 28, 4},
  {11, 1, 29, 5}, {11, 1, 30, 6}, {11, 1, 31, 7}, {12, 2, 1, 8}, // Corrected yearIndex to 12 for {12, 2, 1, 8}

  // 2036
  {12, 1, 13, 1}, {12, 1, 14, 2}, {12, 1, 15, 3}, {12, 1, 16, 4},
  {12, 1, 17, 5}, {12, 1, 18, 6}, {12, 1, 19, 7}, {12, 1, 20, 8},

  // 2037
  {13, 1, 2, 1}, {13, 1, 3, 2}, {13, 1, 4, 3}, {13, 1, 5, 4},
  {13, 1, 6, 5}, {13, 1, 7, 6}, {13, 1, 8, 7}, {13, 1, 9, 8},

  // 2038
  {14, 1, 21, 1}, {14, 1, 22, 2}, {14, 1, 23, 3}, {14, 1, 24, 4},
  {14, 1, 25, 5}, {14, 1, 26, 6}, {14, 1, 27, 7}, {14, 1, 28, 8},

  // 2039
  {15, 1, 11, 1}, {15, 1, 12, 2}, {15, 1, 13, 3}, {15, 1, 14, 4},
  {15, 1, 15, 5}, {15, 1, 16, 6}, {15, 1, 17, 7}, {15, 1, 18, 8},

  // 2040
  {16, 0, 29, 1}, {16, 0, 30, 2}, {16, 1, 1, 3}, {16, 1, 2, 4},
  {16, 1, 3, 5}, {16, 1, 4, 6}, {16, 1, 5, 7}, {16, 1, 6, 8},

  // 2041
  {17, 1, 17, 1}, {17, 1, 18, 2}, {17, 1, 19, 3}, {17, 1, 20, 4},
  {17, 1, 21, 5}, {17, 1, 22, 6}, {17, 1, 23, 7}, {17, 1, 24, 8},

  // 2042
  {18, 1, 7, 1}, {18, 1, 8, 2}, {18, 1, 9, 3}, {18, 1, 10, 4},
  {18, 1, 11, 5}, {18, 1, 12, 6}, {18, 1, 13, 7}, {18, 1, 14, 8},

  // 2043
  {19, 1, 26, 1}, {19, 1, 27, 2}, {19, 1, 28, 3}, {19, 1, 29, 4},
  {19, 1, 30, 5}, {19, 1, 31, 6}, {20, 2, 1, 7}, {20, 2, 2, 8}
};

const uint16_t numHanukkahDays = sizeof(hanukkahDays) / sizeof(HanukkahDay);

void loadHanukkahDay(uint16_t index, HanukkahDay &hday) {
  memcpy_P(&hday, &hanukkahDays[index], sizeof(HanukkahDay));
}

int getHanukkahDay(byte yearOffset, byte monthOffset, byte day) {
  
  HanukkahDay hday;

  for (uint16_t i = 0; i < numHanukkahDays; i++) {
    loadHanukkahDay(i, hday); // Fetch entry i from PROGMEM
    if (hday.yearOffset == yearOffset && hday.monthOffset == monthOffset && hday.day == day) {
      Serial.print("Hanukkah Day: ");
      delay(SERIAL_WAIT_TIME);
      Serial.println(hday.hanukkahDay);
      delay(SERIAL_WAIT_TIME);
      return hday.hanukkahDay;
    }
  }
  return 0;
}

