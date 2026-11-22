#ifndef USER_INPUT_H
#define USER_INPUT_H

// Prompt, discard anything stale in the input buffer, then wait for a number.
byte getSerialInput(const __FlashStringHelper *message);

// Prompt, discard stale input, then wait for any character (e.g. "press enter to continue").
void waitForSerialKey(const __FlashStringHelper *message);

// For command loops: discards non-digit bytes and returns true if a digit is waiting.
bool serialNumberPending();

// Reads a number that is being typed (waits for its digits) and consumes the line ending.
byte readSerialNumber();

#endif // USER_INPUT_H
