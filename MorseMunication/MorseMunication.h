// MorseMunication.h
//////////////////////////////////////////////
// This library is licensed under the
// GNU General Public License v2.0
// ///////////////////////////////////////////
// Author: Luca Valcavi - luca at valcavi com
//////////////////////////////////////////////
#ifndef MorseMunication_h
#define MorseMunication_h
//
#include <Arduino.h>
// number of signs
#define one   0b00100000
#define two   0b01000000
#define three 0b01100000
#define four  0b10000000
#define five  0b10100000
// dot/dash
#define SIGN_DOT 0
#define SIGN_DASH 1
// number of microseconds between every check
#define CHECK_KEY_STATUS_EVERY_USEC 1000 // controllo tasto ogni mS
// virtual key status
#define RELEASED   0
#define PUSHED     1
#define UNDEFINED  2
// frequencies
#define freqKEYdown 3000 // hertz
#define freqKEYup   1500 // hertz
// MASTER FLAG meaning bits
#define rxLETTER           1 // A letter is received
#define rxUNINTELLIGIBLE   2 // unintelligible
#define rxSPACE            4 // speca between words
#define rxSPACELONG        8 // space between paragraphs

//
class MorseMunication{
public:
  void refresh();
  char getNews();
  void interruptFromRx();
  void sendMessage(uint8_t * buffer, uint8_t len);
  void setup(uint16_t txPOINTms,uint8_t pinTx);
	void rxStart();
	void rxStop();
private:
	void setKey(uint8_t status);
  void keyReleasedRoutine();
  void keyPressedRoutine();
  void setMflag(uint8_t mask);
	void sendChar(char letterToTransmit);
  //
  // end class
};
//
#endif
