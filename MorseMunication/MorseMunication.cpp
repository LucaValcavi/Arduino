// MorseMunication.cpp
//////////////////////////////////////
// This library is licensed under the
// GNU General Public License v2.0
/////////////////////////////////////
#include "MorseMunication.h"
//
uint8_t G_BANKofSIGNS[36]={
  two   | 0b00000001,    //A .-   two signs
  four  | 0b00001000,    //B -... four signs
  four  | 0b00001010,    //C -.-.
  three | 0b00000100,    //D -..
  one   | 0b00000000,    //E .
  four  | 0b00000010,    //F ..-.
  three | 0b00000110,    //G --.
  four  | 0b00000000,    //H ....
  two   | 0b00000000,    //I ..
  four  | 0b00000111,    //J ---.
  three | 0b00000101,    //K -.-
  four  | 0b00000100,    //L .-..
  two   | 0b00000011,    //M --
  two   | 0b00000010,    //N -.
  three | 0b00000111,    //O ---
  four  | 0b00000110,    //P .--.
  four  | 0b00001101,    //Q --.-
  three | 0b00000010,    //R .-.
  three | 0b00000000,    //S ...
  one   | 0b00000001,    //T -
  three | 0b00000001,    //U ..-
  four  | 0b00000001,    //V ...-
  three | 0b00000011,    //W .--
  four  | 0b00001001,    //X -..-
  four  | 0b00001011,    //Y -.--
  four  | 0b00001100,    //Z --..
  five  | 0b00001111,    //1 .----
  five  | 0b00000111,    //2 ..---
  five  | 0b00000011,    //3 ...--
  five  | 0b00000001,    //4 ....-
  five  | 0b00000000,    //5 .....
  five  | 0b00010000,    //6 -....
  five  | 0b00011000,    //7 --...
  five  | 0b00011100,    //8 ---..
  five  | 0b00011110,    //9 ----.
  five  | 0b00011111,    //0 -----
};
//
char G_BANKofLETTERS[36] = {'A','B','C','D','E','F','G','H','I','J','K','L',
'M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z','1','2','3','4','5','6',
'7','8','9','0'};
// RX timing (RX and TX timing differs slightly)
uint16_t G_signDot;
uint16_t G_signDash;
uint16_t G_pauseBetwSigns;
uint16_t G_pauseBetwLetters;
uint16_t G_pauseBetwWords;
// TX timing
uint16_t G_signDotTX;
uint16_t G_signDashTX;
uint16_t G_pauseBetwSignsTX;
uint16_t G_pauseBetwLettersTX;
uint16_t G_pauseBetwWordsTX;
//
uint8_t G_PreviousStateOfKey = RELEASED;
uint8_t G_Sign = SIGN_DOT;
//
uint8_t G_VirtualKeyStatus;
//
uint8_t G_pinTx;
//
uint8_t G_NowYouCanRx=true;
//
char G_ReceivedLetter;
uint8_t G_MASTERflag;
//
void MorseMunication::setMflag(uint8_t mask){
	G_MASTERflag = G_MASTERflag | mask;
}



////////////////////////////////////////////////////////////////////////////////
// Routine for transmission of tones
////////////////////////////////////////////////////////////////////////////////
void MorseMunication::txKeyUpFrequency(){
  tone(G_pinTx,freqKEYup);
}
void MorseMunication::txKeyDownFrequency(){
  tone(G_pinTx,freqKEYdown);
}
////////////////////////////////////////////////////////////////////////////////
// Transmits a letter
////////////////////////////////////////////////////////////////////////////////
void MorseMunication::sendChar(char letterToTransmit){
	// ATTENZIONE : controllare l'esatta spaziatura tra segni/caratteri/etc.
	// soprattutto visti nell'insieme come frasi e singolarmente come caratteri
	if (letterToTransmit==' '){ // pause as long as 1 dash
		tone(G_pinTx,freqKEYup);
		delay(G_pauseBetwWordsTX);
	} else {
		uint8_t signsFromBank = 0;
	  for (int n = 0; n < 36; n++){
	    if (letterToTransmit == G_BANKofLETTERS[n]){
	      signsFromBank = G_BANKofSIGNS[n];
	    }
	  }
	  uint8_t signsNr;
	  if (signsFromBank > 0){
			// transmission of letter
	    signsNr = signsFromBank & 0b11100000; // recover the number of signs
	    signsNr = signsNr >> 5;    // shifts of 5 places to obtain the number
	    uint8_t mask = 1;
	    uint8_t sign;
	    mask = mask << (signsNr - 1);
	    for (int n = 0; n < signsNr; n++){
	      sign = mask & signsFromBank;
	      if (sign == SIGN_DOT){
	        // dot
	        tone(G_pinTx,freqKEYdown);
	        delay(G_signDotTX);
	      } else {
	        // dash
	        tone(G_pinTx,freqKEYdown);
	        delay(G_signDashTX);
	      }
	      // pausa tra segni
	      tone(G_pinTx,freqKEYup);
	      delay(G_pauseBetwSignsTX);
	      //
	      mask = mask >> 1;
	    }
			tone(G_pinTx,freqKEYup);
			delay(G_pauseBetwLettersTX);
			// -- end transmission of letter
	  }
	}
}


////////////////////////////////////////////////////////////////////////////////
// send the message contained in buffer
////////////////////////////////////////////////////////////////////////////////
void MorseMunication::sendMessage(uint8_t * buffer, uint8_t len){
	tone(G_pinTx,freqKEYup);
	delay(G_pauseBetwSignsTX);
  for (int n = 0; n<len; n++){
      sendChar(char(buffer[n]));
  }
	noTone(G_pinTx);
}

void MorseMunication::rxStart(){
	G_NowYouCanRx=true;
}
void MorseMunication::rxStop(){
	G_NowYouCanRx=false;
}

////////////////////////////////////////////////////////////////////////////////
// setup
////////////////////////////////////////////////////////////////////////////////
void MorseMunication::setup(uint16_t txPOINTms,uint8_t pinTx){
	if (txPOINTms < 6) { // minimum 5 msec
		txPOINTms=6;
	}
	if (txPOINTms > 6500) { // max 6500 msec
		txPOINTms=6500;
	}
  // RX
  G_signDot = txPOINTms / 2; // experimental value: RX point length is 1/2 of TX
  G_signDash = G_signDot * 3; // 300ms
  G_pauseBetwSigns = G_signDot; // 100ms
  G_pauseBetwLetters = G_signDot * 3; // 300 ms
  G_pauseBetwWords = G_pauseBetwLetters * 3; // 900 ms
  // TX
  G_signDotTX = txPOINTms;
  G_signDashTX = G_signDotTX * 3;
  G_pauseBetwSignsTX = G_signDotTX;
  G_pauseBetwLettersTX = G_signDotTX * 3;
  G_pauseBetwWordsTX = G_pauseBetwLettersTX * 3;
  //
  G_pinTx = pinTx;
}


////////////////////////////////////////////////////////////////////////////////
// It has been received an interrupt from the receive pin.
// Depending from the number and the frequency of previous intterruptions
// it set the status of the virtual key accordingly.
////////////////////////////////////////////////////////////////////////////////
void MorseMunication::interruptFromRx(){
	if (G_NowYouCanRx){
	  static unsigned long prevMicros;
	  unsigned long diff;
	  static uint8_t counterKeyDownFreq;
	  static uint8_t counterKeyUpFreq;
	  //
	  diff = micros() - prevMicros;
	  prevMicros = micros();
	  //
	  if ((diff >= 310) && (diff <= 360)){
	    // detect frequenza 3000 Hz (key down)
	    counterKeyDownFreq += 1;
	    if (counterKeyDownFreq > 10){ // minimum 11 periods
	      counterKeyDownFreq = 11;
	      counterKeyUpFreq = 0;
		    G_VirtualKeyStatus = PUSHED;
	    }
	  } else {
	  	if ((diff >= 640) && (diff <= 680)){
	      // detect frequenza 1500 Hz (key up)
	      counterKeyUpFreq += 1;
	      if (counterKeyUpFreq > 5){ //minimum 6 periods
	        counterKeyDownFreq = 0;
	        counterKeyUpFreq = 6;
	        G_VirtualKeyStatus = RELEASED;
	      }
	  	} // else the UNDEFINED (set from getNews)
	  }
	}
}
////////////////////////////////////////////////////////////////////////////////
// This routine must be called as soon as possible by the main-application.
// It checks the status of VirtualKeyStatus, calling the KeyRoutines or
// setting flags.
////////////////////////////////////////////////////////////////////////////////
void MorseMunication::refresh(){
  unsigned long diff;
  static unsigned long tempoPrec = micros(); //
  diff = micros()-tempoPrec;
  if (diff > CHECK_KEY_STATUS_EVERY_USEC){
    tempoPrec = micros();
    // ogni millisecondo
    if(G_VirtualKeyStatus == RELEASED){
      keyReleasedRoutine();
    } else {
      keyPressedRoutine();
    }
  }
}
////////////////////////////////////////////////////////////////////////////////
// This routine is called when it is detected the status of "key released"
// In this phase we can we can detect a new letter (character) or
// a CR/LF , a SPACE
////////////////////////////////////////////////////////////////////////////////
void MorseMunication::keyReleasedRoutine(){
  uint8_t tmpNS;
  static uint8_t sequenceOfSigns;
  static uint16_t counterReleased = 65535;
  static uint8_t nrOfSigns;
  //
  if (G_PreviousStateOfKey == PUSHED){
    G_PreviousStateOfKey = RELEASED;
    counterReleased = 0;
  } else {
    if (counterReleased < 65535){
      counterReleased += 1;
    }
    if (counterReleased == G_pauseBetwWords){
      setMflag(rxSPACE); // detected a SPACE (end of a word)
    } else {
      // --- AAA
      if (counterReleased == G_pauseBetwLetters){
        // checks if the sequence of sign could be a letter
        for (int n = 0; n < 36; n++){
          if (sequenceOfSigns == G_BANKofSIGNS[n]){
            G_ReceivedLetter = G_BANKofLETTERS[n];
		        setMflag(rxLETTER);       // FLAG received Char
          }
        }
        if (G_ReceivedLetter == 0){
	        setMflag(rxUNINTELLIGIBLE);    // FLAG unintelligible
        }
        // reset values
        nrOfSigns = 0;
        sequenceOfSigns = 0;
        G_Sign = SIGN_DOT;
        // END checks for letter
      } else {
        // --- CCC
        if (counterReleased == G_pauseBetwSigns){
          // the pause if between signs: I compose the sequence
          sequenceOfSigns = sequenceOfSigns & 0b00011111;
          sequenceOfSigns = sequenceOfSigns << 1;
          sequenceOfSigns = sequenceOfSigns | G_Sign;
          nrOfSigns++;
          tmpNS = nrOfSigns << 5;
          sequenceOfSigns = sequenceOfSigns | tmpNS;
        } else {
          // the pause is another value, non useful for now
          // so I do nothing
        }
        // --- CCC
      }
      // --- AAA
    }
  }
}
////////////////////////////////////////////////////////////////////////////////
// This routine detect if a sign can be a dot/dash
////////////////////////////////////////////////////////////////////////////////
void MorseMunication::keyPressedRoutine(){
  static uint16_t counterPushed = 0;
  //
  if (G_PreviousStateOfKey == RELEASED){
    G_PreviousStateOfKey = PUSHED;
    counterPushed = 0;
  } else {
    if (counterPushed < 65535){
      counterPushed += 1;
    }
    if (counterPushed == G_signDash){
      // dash
      G_Sign = SIGN_DASH; // linea
    } else {
      if (counterPushed == G_signDot){
        // dot
        G_Sign = SIGN_DOT; // punto
      } else {
        // (nothing)
      }
    }
  }
}
////////////////////////////////////////////////////////////////////////////////
// This routine is called by the main-application after a "true" from
// thereAreNews routine.
// It return both the letter (if present) and the MASTER-flag status
////////////////////////////////////////////////////////////////////////////////
char MorseMunication::getNews(){
	char temp;
	temp = '#';
	if (G_NowYouCanRx){
		if (G_MASTERflag > 0){
			switch(G_MASTERflag){
			case rxLETTER:
				temp = G_ReceivedLetter;
				break;
			case rxUNINTELLIGIBLE:
				temp = '?';
				break;
			case rxSPACE:
				temp = ' ';
				break;
			}
			G_MASTERflag = 0;
		  G_ReceivedLetter = 0;
		  G_VirtualKeyStatus = UNDEFINED; //none frequency is detected
		}
	}
	return(temp);
}
