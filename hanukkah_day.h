struct HanukkahDay {
  byte yearOffset;  // 0-19
  byte monthOffset; // 0 (Nov), 1 (Dec), 2 (Jan)
  byte day;        // 1-31
  byte hanukkahDay; // 1-8
};

int getHanukkahDay(byte yearOffset, byte monthOffset, byte day);
int getHanukkahDayForDate(byte year, byte month, byte day); // real date: 2-digit year, month 1-12
void loadHanukkahDay(uint16_t index, HanukkahDay &hday);