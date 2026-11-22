byte decToBcd(byte val);
byte bcdToDec(byte val);
void setTime(byte second, byte minute, byte hour, byte dayOfWeek, byte dayOfMonth, byte month, byte year);
void inputTime();
void readTime(byte *second, byte *minute, byte *hour, byte *dayOfWeek, byte *dayOfMonth, byte *month, byte *year);
void displayTime();
void formatDisplayTime(char *out, size_t outSize); // e.g. "Sunday 12/14/25 9:05:03" (needs ~26 bytes)
void previousDate(byte *year, byte *month, byte *day); // steps a 2-digit-year date back one day
