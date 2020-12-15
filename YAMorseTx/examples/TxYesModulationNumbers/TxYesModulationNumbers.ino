#include <YAMorseTx.h>
YAMorseTx MORSE = YAMorseTx();

char bufferTx[]={0,0,0,0,0,0,0,0,0,0};


#define dotLength     24 // ms (values from 6ms to 6500ms are allowed)
#define pinTx         12 // you can use any pin
#define freqKeyUp   1500 // Hz
#define freqKeyDown 3000 // Hz

void setup() {
  MORSE.mm_setup(dotLength,pinTx,freqKeyUp,freqKeyDown);
  Serial.begin(57600);
  pinMode(LED_BUILTIN,OUTPUT);
  digitalWrite(LED_BUILTIN,LOW);
  long lval=87654;
  ltoa(lval,bufferTx,10);   
  }

void loop() {
  delay(1000);
  MORSE.mm_numericOn();
  MORSE.mm_send((uint8_t *)bufferTx,sizeof(bufferTx));
  MORSE.mm_numericOff();
  delay(1000);
  MORSE.mm_txNoTones(); // you can turn off the modulation
  delay(2000);          // at the end of the transmission
}
