// YAMorseTx.h
//////////////////////////////////////////////
// This library is licensed under the
// GNU General Public License v2.0
// ///////////////////////////////////////////
// Author: Luca Valcavi - luca at valcavi com
//////////////////////////////////////////////
#ifndef YAMorseTx_h
#define YAMorseTx_h
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
// virtual key status
#define RELEASED   0
#define PUSHED     1
#define UNDEFINED  2
//
typedef struct{
    uint16_t sequenceOfSigns;
    uint8_t numberOfSign;
} morseSigns;

class YAMorseTx{
public:
	void    mm_setup(uint16_t slowness,uint8_t pin,uint16_t frequency_key_up,uint16_t frequency_key_down);
	uint8_t mm_send(uint8_t* buf, uint8_t len);
	void    mm_txKeyUp();
	void    mm_txKeyDown();
	void    mm_txNoTones();

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
	char _bank_of_letters[36] = {'A','B','C','D','E','F','G','H','I','J','K','L',
	'M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z','1','2','3','4','5','6',
	'7','8','9','0'};
	// TX timing
	uint16_t _signDotTX;
	uint16_t _signDashTX;
	uint16_t _pauseBetwSignsTX;
	uint16_t _pauseBetwLettersTX;
	uint16_t _pauseBetwWordsTX;
	//
	uint8_t _pinTx = 12;
	//
	uint16_t _modulation_key_up = 1500; // hz
	uint16_t _modulation_key_down = 3000; // hz
	//
	uint8_t _no_modulation = false;
	// subroutines
	morseSigns signs_from_character(char character);
	morseSigns compose_prosigns(uint8_t* chars, uint8_t len);
	void tx_morse_sequence_of_signs(morseSigns morse_signs);
	// end class
};
//
#endif

///////////////////////////////
// receive a message/char with/without carrier
///////////////////////////////
//
// void mm_set_rx_pin(uint8_t pin);
// void mm_rx_start();
// void mm_rx_stop();
// //
// void mm_refresh_from_extern();
// void mm_interrupt_from_extern();
// //
// void mm_wait_rx();
// uint8_t mm_wait_rx_max(unsigned long milliseconds);
// //
// char vw_received_char();
// uint8_t vw_have_message();
// uint8_t vw_get_message(uint8_t* buf);
//
///////////////////////////////
// receive with carrier
///////////////////////////////
