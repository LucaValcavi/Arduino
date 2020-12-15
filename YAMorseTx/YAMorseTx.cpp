// YAMorseTx.cpp
//////////////////////////////////////////////
// This library is licensed under the
// GNU General Public License v2.0
// ///////////////////////////////////////////
// Author: Luca Valcavi - luca at valcavi com
//////////////////////////////////////////////
#include "YAMorseTx.h"
//
void YAMorseTx::mm_setup(uint16_t slowness,uint8_t pin,uint16_t frequency_key_up,uint16_t frequency_key_down){
	if (slowness < 6) { // minimum 6 msec
		slowness=6;
	}
	if (slowness > 6500) { // max 6500 msec
		slowness=6500;
	}
  // TX
  _signDotTX = slowness;
  _signDashTX = _signDotTX * 3;
  _pauseBetwSignsTX = _signDotTX;
  _pauseBetwLettersTX = _signDotTX * 3;
  _pauseBetwWordsTX = _pauseBetwLettersTX * 3;
	//----------------------------
	_pinTx = pin;
	pinMode(_pinTx, OUTPUT);
	//----------------------------
	if ((!frequency_key_up) and (!frequency_key_down)){
		_no_modulation = true;
	} else {
		_no_modulation = false;
		_modulation_key_up = frequency_key_up;
		_modulation_key_down = frequency_key_down;
	}
}
void YAMorseTx::mm_txKeyUp(){
	if (!_no_modulation){
		tone(_pinTx,_modulation_key_up);
	} else {
		digitalWrite(_pinTx,HIGH);
	}
}
void YAMorseTx::mm_txKeyDown(){
	if (!_no_modulation){
		tone(_pinTx,_modulation_key_down);
	} else {
		digitalWrite(_pinTx,LOW);
	}
}
void YAMorseTx::mm_txNoTones(){
	if (!_no_modulation){
		noTone(_pinTx);
	} else {
		digitalWrite(_pinTx,HIGH);
	}
}
void YAMorseTx::mm_numericOn(){
	_numeric_on = true;
}
void YAMorseTx::mm_numericOff(){
	_numeric_on = false;
}
uint8_t YAMorseTx::mm_send(uint8_t* buffer, uint8_t len){
	delay(_pauseBetwSignsTX);
	for (uint8_t n = 0; n < len; n++){
		////////////////////////////////////////
		if (_numeric_on){
			if (char(buffer[n]=='0')){
				buffer[n] = 'E';
			}	else {
				if (char(buffer[n]=='1')){
					buffer[n] = 'T';
				}	else {
					if (char(buffer[n]=='2')){
						buffer[n] = 'I';
					}	else {
						if (char(buffer[n]=='3')){
							buffer[n] = 'A';
						}	else {
							if (char(buffer[n]=='4')){
								buffer[n] = 'N';
							}	else {
								if (char(buffer[n]=='5')){
									buffer[n] = 'S';
								}	else {
									if (char(buffer[n]=='6')){
										buffer[n] = 'D';
									}	else {
										if (char(buffer[n]=='7')){
											buffer[n] = 'R';
										}	else {
											if (char(buffer[n]=='8')){
												buffer[n] = 'U';
											}	else {
												if (char(buffer[n]=='9')){
													buffer[n] = 'H';
												}	else {
													//
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
		////////////////////////////////////////
		if (char(buffer[n]==' ')){ // pause as long as 1 dash
			mm_txKeyUp();
			delay(_pauseBetwWordsTX);
		} else {
			if (buffer[n] > 90) {
				// prosign (ex <sos> <rt> <bk>)
				// compose the letters
				uint8_t tempbuffer[]={0,0,0,0,0,0,0,0}; // only 8 char max
				uint8_t x = 0;
				while (buffer[n] > 90) {// beyond the letter Z
					if (x < 7){ // prosign must be composed by 8 chars max
						tempbuffer[x] = buffer[n];
						x++;
					}
					n++;
				}
				// retrieve and tx the signs
				tx_morse_sequence_of_signs(compose_prosigns(tempbuffer,x));
				delay(_pauseBetwLettersTX);
			} else {
				// normal character (A-Z 0-9)
				tx_morse_sequence_of_signs(signs_from_character(char(buffer[n])));
			}
		}
	}
}
morseSigns YAMorseTx::signs_from_character(char character){
	uint8_t signsFromBank = 0;
	morseSigns reply = {0,0};
	if (character > 90){
		character -= 32; // only UPPERCASES
	}
	for (int n = 0; n < 36; n++){
		if (character == _bank_of_letters[n]){
			signsFromBank = _bank_of_signs[n];
			break;
		}
	}
	if (signsFromBank > 0){
		uint8_t signsNr;
		signsNr = signsFromBank & 0b11100000; // recover the number of signs
		signsNr = signsNr >> 5;    // shifts of 5 places to obtain the number
		signsFromBank = signsFromBank & 0b00011111; // remove counter
		reply = {signsFromBank, signsNr};
	}
	return reply;
}
morseSigns YAMorseTx::compose_prosigns(uint8_t* chars, uint8_t len){
	// prosigns are character assembled by basic letters
	// example <SOS> is ...---... no pauses between signs, like it was one letter
	morseSigns reply = {0,0};
	morseSigns temp;
	for (int m = 0; m < len; m++){// for each char
		temp = signs_from_character(chars[m]);
		reply.numberOfSign += temp.numberOfSign;
		if (m > 0) {
			reply.sequenceOfSigns = reply.sequenceOfSigns << temp.numberOfSign;
		}
		reply.sequenceOfSigns = reply.sequenceOfSigns | temp.sequenceOfSigns;
	}
	if (reply.numberOfSign > 16) { // max number of signs
		reply.numberOfSign = 16;
	}
	return reply;
}
void YAMorseTx::tx_morse_sequence_of_signs(morseSigns morse_signs){
	if (morse_signs.numberOfSign > 0){
		// character has been found in bank
		uint16_t mask = 1;
		mask = mask << (morse_signs.numberOfSign - 1);
		uint16_t sign = 0;
		for (uint8_t n = 0; n < morse_signs.numberOfSign; n++){
			sign = mask & morse_signs.sequenceOfSigns;
			if (sign == SIGN_DOT){ // SIGN_DOT = 0
				// dot
				mm_txKeyDown();
				delay(_signDotTX);
			} else {
				// dash
				mm_txKeyDown();
				delay(_signDashTX);
			}
			// pause between signs
			mm_txKeyUp();
			delay(_pauseBetwSignsTX);
			//
			mask = mask >> 1;
		}
		mm_txKeyUp();
		delay(_pauseBetwLettersTX);
		// -- end transmission of letter
	}
}
