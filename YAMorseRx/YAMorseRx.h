// Author: Luca Valcavi (luca at valcavi dot com)
// Copyright (C) 2019 Luca Valcavi
// YAMorseRx.h

#ifndef YAMorseRx_h
#define YAMorseRx_h
//
#include <Arduino.h>
// number of signs
#define one   0b00100000
#define two   0b01000000
#define three 0b01100000
#define four  0b10000000
#define five  0b10100000
// for prosigns
#define p_five  0b0101000000000000
#define p_six   0b0110000000000000
#define p_seven 0b0111000000000000
#define p_nine  0b1001000000000000
// dot/dash
#define SIGN_DOT 0
#define SIGN_DASH 1
// virtual key status
#define RELEASED   0
#define PUSHED     1
#define UNDEFINED  2
//
#define FREQ_TOLERANCE 25 // difference from the central frequency (HZ)
// number of microseconds between every check
#define CHECK_KEY_STATUS_EVERY_USEC 1000 // check every millisecond
// prosigns
#define P_SOS p_nine | 0b0000000000111000
#define P_BK  p_six  | 0b0000000001000101
#define P_AS  p_five | 0b0000000000001000
//
typedef struct{
    uint16_t sequenceOfSigns;
    uint8_t numberOfSign;
} morseSigns;
//
class YAMorseRx{
	public:
		void    mm_setup(uint16_t slowness,uint8_t pin,uint16_t frequency_key_up,uint16_t frequency_key_down);
		void    mm_checkRxPinStatus();
		void    mm_interruptFromRxPin();
		void    mm_refresh();
		uint8_t mm_have_message();
		void    mm_get_message(uint8_t* buf, uint8_t* len);
	private:
		uint8_t _bank_of_signs[36]={
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
		uint8_t _bank_of_letters[36] = {'A','B','C','D','E','F','G','H','I','J','K','L',
		'M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z','1','2','3','4','5','6',
		'7','8','9','0'};
		// RX timing
		//
		uint16_t _period_key_up_low = 310; // uS values for 3000Hz
		uint16_t _period_key_up_high = 360;
		uint16_t _period_key_down_low = 640; // uS values for 1500Hz
		uint16_t _period_key_down_high = 680;
		//
		uint8_t _no_modulation = false;
		uint8_t _pinRx = 3; // necessary only for no-carrier
		uint8_t _VirtualKeyStatus = UNDEFINED;
		//
		uint16_t _counterReleased = 65535;
		uint16_t _counterPushed = 0;
		//
		uint8_t _havemessage = 0;
		uint8_t _PreviousStateOfKey = RELEASED;
		uint8_t _Sign = UNDEFINED;
		//
		uint16_t _SignDot;
		uint16_t _SignDash;
		uint16_t _pauseBetwSigns;
		uint16_t _pauseBetwLetters;
		uint16_t _pauseBetwWords;
		uint16_t _pauseBetwSentences;
		//
		uint8_t _discard_next_char = true;
		//
		morseSigns _now_receiving = {0,0}; // the character that now I'm receiving
		uint8_t _counterbuff = 0;
		uint8_t _inverted = false;
		/////////////////
		void 		mm_keyReleasedRoutine();
		void 		mm_keyPressedRoutine();
		void 		clearBuffer();
};
//
#endif
