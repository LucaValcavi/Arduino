#include <MorseMunication.h>
MorseMunication MORSE = MorseMunication();
// Arduino pin connected to the TX-module pin
#define pinTX       12
// timeDot: value in milliseconds 
// 25ms is a good value for speed and reliability
// the minimum value is 6ms the maximum is 6500ms
#define timeDot   6
// example of message
char buf[]="ABCDE FGHIJ KLMNO PQRST UVWXY Z 12345 67890"; // 43 chars
//
void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  MORSE.setup(timeDot,pinTX);
  digitalWrite(LED_BUILTIN,LOW);
}
//
void loop() {
  digitalWrite(LED_BUILTIN,HIGH);
  delay(100);
  digitalWrite(LED_BUILTIN,LOW);
  // With sendChar and sendMessage can use only:
  // - LETTERS A-Z (uppercases) 
  // - NUMBERS 0-9
  // - the space " " 
  MORSE.sendMessage((uint8_t *)buf,43);
  delay(1000);
}