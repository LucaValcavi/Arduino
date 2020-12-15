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

unsigned long previousTime;
uint8_t sequence = 0;
uint8_t charSequence[4];
uint8_t sequenceA[]={'F','P',0,0};
uint8_t sequenceB[]={'P','R',0,0};

  
void setup() {
  MORSE.mm_setup(dotLength,pinRx,freqKeyUp,freqKeyDown);
  attachInterrupt(digitalPinToInterrupt(pinRx), dummyRoutine, RISING);
  pinMode(LED_BUILTIN,OUTPUT);
  checkSequence();
}

void loop() {
  uint8_t buf[8];
  uint8_t buflen = 0;


  MORSE.mm_checkRxPinStatus();
  MORSE.mm_refresh();

  if (MORSE.mm_have_message()){
    MORSE.mm_get_message(buf, &buflen);
    if (buflen>0){
      if (char(buf[0])=='\n'){
        checkSequence();
      }
      addCharacterToSequence(buf[0]);
    }
  }

  if (sequence==2){
    digitalWrite(LED_BUILTIN,HIGH);
    previousTime=millis();
    sequence=0;
  }

  if ((millis()-previousTime)>=20000){
    digitalWrite(LED_BUILTIN,LOW);
  }
  
}

void dummyRoutine(){
  MORSE.mm_interruptFromRxPin();
}

void addCharacterToSequence(uint8_t character){
  charSequence[sequence]=character;
  sequence++;
}

void checkSequence(){
  if (charSequence==sequenceA){
    digitalWrite(LED_BUILTIN,HIGH);
  }
  clearSequence();
}

void clearSequence(){
  for (int x = 0; x < 4; x++){
    charSequence[x]=0;
  }
}
