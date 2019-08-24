#include <YAMorseTx.h>
YAMorseTx MORSE = YAMorseTx();

char bufferTx[]="as ABCDE 01234 bk sk va sos QRSTU kn fhks";

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

#define dotLength 24 // ms (values from 6ms to 6500ms are allowed)
#define pinTx     12 // you can use any pin

void setup() {
  MORSE.mm_setup(dotLength,pinTx,0,0);
  }

void loop() {
  delay(1000);
  MORSE.mm_send((uint8_t *)bufferTx,sizeof(bufferTx));
}
