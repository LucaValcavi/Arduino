#include <MorseMunication.h>
MorseMunication MORSE = MorseMunication();
// Arduino pin connected to the TX-module pin
#define pinTX      12 // it is NOT USED in this example
// timeDot: value in milliseconds 
// 25ms is a good value for speed and reliability
// the minimum value is 6ms the maximum is 6500ms
#define timeDot  6 
// Arduino pin connected to the RX-module pin
// It can be the pin 2 or 3: only these pins can be connected
// to an external interrupt  
#define pinRX      3  
//
void setup() {
  pinMode(LED_BUILTIN, OUTPUT); 
  MORSE.setup(timeDot,pinTX);
  attachInterrupt(digitalPinToInterrupt(pinRX), dummyRoutine, RISING); 
  Serial.begin(57600);
  digitalWrite(LED_BUILTIN,LOW);
}
//
void loop() {
  // getNews return  
  // - LETTERS A-Z (uppercases) 
  // - NUMBERS 0-9
  // - the space " " 
  // - "?" (when the transmission is unintelligible)
  // - "#" (when nothing is received)
  char charReceived;
  char previousChar;
  MORSE.refresh();
  charReceived=MORSE.getNews();
  if (charReceived=='#'){
      // no char received
    } else {
      if (previousChar==' ' && charReceived==' '){
        Serial.print('\n');
      } else {
        Serial.print(charReceived);
        previousChar=charReceived;
      }
    }
}
//
void dummyRoutine(){
  MORSE.interruptFromRx();
}
