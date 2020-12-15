#include <YAMorseTx.h>
YAMorseTx MORSE = YAMorseTx();

unsigned long previousTime;
unsigned long nextRandomTermMillisec;

char bufferTx[]="  FP "; // matteo PC
//char bufferTx[]="QP"; // alessandro PC
//char bufferTx[]="FS"; // matteo smartphone
//char bufferTx[]="QS"; // alessandro smartphone

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

#define dotLength    24 // ms (values from 6ms to 6500ms are allowed)
#define pinTx         12 // you can use any pin
#define freqKeyUp   1500 // Hz
#define freqKeyDown 3000 // Hz

void setup() {
  MORSE.mm_setup(dotLength,pinTx,freqKeyUp,freqKeyDown);
  pinMode(LED_BUILTIN,OUTPUT);
  digitalWrite(LED_BUILTIN,LOW);
  }

void loop() {
 if ((millis()-previousTime)>=nextRandomTermMillisec){
    previousTime=millis();
    digitalWrite(LED_BUILTIN,HIGH);
    MORSE.mm_send((uint8_t *)bufferTx,sizeof(bufferTx));
    nextRandomTermMillisec=random(30000,40000);
    delay(100);
    MORSE.mm_txNoTones(); // you can turn off the modulation
    digitalWrite(LED_BUILTIN,LOW);
 }
}
