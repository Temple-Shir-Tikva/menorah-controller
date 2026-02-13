byte decToBcd(byte val);
byte bcdToDec(byte val);
void setTime(byte second, byte minute, byte hour, byte dayOfWeek, byte dayOfMonth, byte month, byte year);
void inputTime();
byte getSerialInput(String message);
void readTime(byte *second, byte *minute, byte *hour, byte *dayOfWeek, byte *dayOfMonth, byte *month, byte *year);
void displayTime();
String getDisplayTime();