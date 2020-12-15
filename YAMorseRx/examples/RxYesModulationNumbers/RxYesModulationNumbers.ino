#include <YAMorseRx.h>
YAMorseRx MORSE = YAMorseRx();

// the prosign 'as'  will be decoded as 'EB'
// the prosign 'bk'  will be decoded as 'BK'
// the prosign 'sk'  will be decoded as 'SK'
// the prosign 'sos' will be decoded as 'V7'
// the prosign 'kn'  will be decoded as 'TP'
//
// you can transmit any prosign writing them using lowcase letters.
// For example you can transmit the arbitrary prosign 'fhks' (..-.....-.-...)
// and it will be decoded as 'IBFB' (..-.....-.-...) that is the same sequence
// of signs.

#define dotLength     24 // ms (values from 6ms to 6500ms are allowed)
#define pinRx          2 // issues with pin 3 when used together with YAMorseTx
#define freqKeyUp   1500 // Hz
#define freqKeyDown 3000 // Hz

void setup() {
  MORSE.mm_setup(dotLength,pinRx,freqKeyUp,freqKeyDown);
  attachInterrupt(digitalPinToInterrupt(pinRx), dummyRoutine, RISING);
  Serial.begin(57600);
}

void loop() {
  uint8_t buf[8];
  uint8_t buflen = 0;

  MORSE.mm_checkRxPinStatus();
  MORSE.mm_refresh();

  if (MORSE.mm_have_message()){
    MORSE.mm_get_message(buf, &buflen);
    MORSE.mm_decode_to_numbers(buf,buflen);
    if (buflen>1){                    // prosigns
      Serial.print("(");
      for (uint8_t n=buflen;n>0;n--){ // (!) please read chars in reverse order (!)
        Serial.print(char(buf[n-1])); // (!) from the buffer (!)
      }
      Serial.print(")");
    } else {
      Serial.print(char(buf[0]));     // chars A-Z or 0-9
    }
  }
}

void dummyRoutine(){
  MORSE.mm_interruptFromRxPin();
}
