void mp3Begin();                          // start non-blocking init (call early in setup)
void mp3Service();                        // advance init; call whenever the sketch is waiting
bool mp3Ready();
bool mp3WaitReady(unsigned long maxMs);   // bounded wait; never stalls once the board has given up
void playFile(int fileNumber);
void printMP3BoardDetail();
