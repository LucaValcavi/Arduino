// Author: Luca Valcavi (luca at valcavi dot com)
// Copyright (C) 2019 Luca Valcavi
// YAMorseRx.cpp

#include "YAMorseRx.h"
uint8_t _thebuffer[] = {0,0,0,0,0,0,0,0};
// public subs
void YAMorseRx::mm_setup(uint16_t slowness,uint8_t pin,uint16_t frequency_key_up,uint16_t frequency_key_down){
	if (slowness < 6) { // minimum 5 msec
		slowness=6;
	}
	if (slowness > 6500) { // max 6500 msec
		slowness=6500;
	}
  // RX timing values
	float calculus = 0.5 * slowness; // sperimental value (RX timing =0.5 TX timing)
  _SignDot = calculus;
  _SignDash = _SignDot * 3;
  _pauseBetwSigns = _SignDot;
  _pauseBetwLetters = _SignDot * 3;
  _pauseBetwWords = _pauseBetwLetters * 3;
	_pauseBetwSentences = _pauseBetwWords * 3;
	//-----------------------------------------
	_pinRx = pin; // used only with no interrupt (no modulation)
	pinMode(_pinRx, INPUT_PULLUP); // REMEMBER: signal inverted!!!
	//-------------------------------------------
	if ((!frequency_key_up) and (!frequency_key_down)){
		_no_modulation = true;
		_inverted= false;
	} else {
		_no_modulation = false;
		_inverted = true;
		float calculation;
		calculation = frequency_key_up;
		calculation = 1/ calculation;
		calculation = calculation * 1000000;
		//calculation = 1 / frequency_key_up * 1000000;
		_period_key_up_low = calculation - FREQ_TOLERANCE;
		_period_key_up_high = calculation + FREQ_TOLERANCE;
		calculation = frequency_key_down;
		calculation = 1/ calculation;
		calculation = calculation * 1000000;
		//calculation = 1 / frequency_key_down * 1000000;
		_period_key_down_low = calculation - FREQ_TOLERANCE;
		_period_key_down_high = calculation + FREQ_TOLERANCE;
	}
}
void YAMorseRx::mm_checkRxPinStatus(){// no interrupt (no modulation)
	static uint8_t counterKeyDownFreq;
	static uint8_t counterKeyUpFreq;
	//
	if (_no_modulation) {
		if (!digitalRead(_pinRx)) {
			counterKeyDownFreq += 1;
			if (counterKeyDownFreq > 20){ //
				counterKeyDownFreq = 21;
				counterKeyUpFreq = 0;
				_VirtualKeyStatus = PUSHED;
			}
		} else {
			counterKeyUpFreq += 1;
			if (counterKeyUpFreq > 20){ //
				counterKeyDownFreq = 0;
				counterKeyUpFreq = 21;
				_VirtualKeyStatus = RELEASED;
			}
		}
	}
}
void YAMorseRx::mm_interruptFromRxPin(){// interrupt (bitonal modulation)
	if (_no_modulation) {
		// nothing
	} else {
		// detect status of key from modulation
	  static unsigned long prevMicros;
	  unsigned long diff;
	  static uint8_t counterKeyDownFreq;
	  static uint8_t counterKeyUpFreq;
		static uint8_t counterNoCarrier;
		//
	  diff = micros() - prevMicros;
	  prevMicros = micros();
	  //
	  if ((diff >= _period_key_up_low) && (diff <= _period_key_up_high)){
	    // detect key up frequency
	    counterKeyDownFreq += 1;
	    if (counterKeyDownFreq > 5){ // minimum 6 consecutive periods
				_CarrierDetected = true;
	      counterKeyDownFreq = 6;
	      counterKeyUpFreq = 0;
				counterNoCarrier = 0;
		    _VirtualKeyStatus = PUSHED;
	    }
	  } else {
	  	if ((diff >= _period_key_down_low) && (diff <= _period_key_down_high)){
	      // detect key down frequency
	      counterKeyUpFreq += 1;
	      if (counterKeyUpFreq > 5){ //minimum 6 consecutive periods
					_CarrierDetected = true;
	        counterKeyDownFreq = 0;
					counterNoCarrier = 0;
	        counterKeyUpFreq = 6;
	        _VirtualKeyStatus = RELEASED;
	      }
	  	} else {
				counterNoCarrier += 1;
				if (counterNoCarrier > 10){
					counterNoCarrier = 11;
					counterKeyDownFreq = 0;
					counterKeyUpFreq = 0;
					_CarrierDetected = false;
				}
			}
	  }
	}
}
void YAMorseRx::mm_refresh(){// is it time to read the VirtualKeyStatus?
  unsigned long diff;
  static unsigned long tempoPrec = micros(); //
  diff = micros() - tempoPrec;
  if (diff > CHECK_KEY_STATUS_EVERY_USEC){
    tempoPrec = micros();
    // every millisecond
    if(_VirtualKeyStatus == RELEASED){
			if (_inverted){// no modulation -> inverts PRESSED and RELEASED
				mm_keyPressedRoutine();
			} else {
				mm_keyReleasedRoutine();
			}
    } else {
			if (_inverted){
				mm_keyReleasedRoutine();
			} else {
				mm_keyPressedRoutine();
			}
    }
  }
}
uint8_t YAMorseRx::mm_have_message(){
	if (_CarrierDetected) {
		if (_havemessage){
			_havemessage = false;
			_VirtualKeyStatus = UNDEFINED; //none frequency is detected
			return true;
		} else {
			return false;
		}
	} else {
		return false;
	}
}
void YAMorseRx::mm_decode_to_numbers(uint8_t* buffer, uint8_t len){

	for (uint8_t n = 0; n < len; n++){
		if (char(buffer[n]=='E')){
			buffer[n] = '0';
		}	else {
			if (char(buffer[n]=='T')){
				buffer[n] = '1';
			}	else {
				if (char(buffer[n]=='I')){
					buffer[n] = '2';
				}	else {
					if (char(buffer[n]=='A')){
						buffer[n] = '3';
					}	else {
						if (char(buffer[n]=='N')){
							buffer[n] = '4';
						}	else {
							if (char(buffer[n]=='S')){
								buffer[n] = '5';
							}	else {
								if (char(buffer[n]=='D')){
									buffer[n] = '6';
								}	else {
									if (char(buffer[n]=='R')){
										buffer[n] = '7';
									}	else {
										if (char(buffer[n]=='U')){
											buffer[n] = '8';
										}	else {
											if (char(buffer[n]=='H')){
												buffer[n] = '9';
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
}
void YAMorseRx::mm_get_message(uint8_t* buf, uint8_t* len){
	*len = _counterbuff;
	// memcopy
	for (uint8_t n =0; n< 8; n++){
		buf[n]=_thebuffer[n];
	}
	clearBuffer();
}
// private subs
void YAMorseRx::mm_keyReleasedRoutine(){
  uint8_t tmpNS;
	uint8_t number_of_signs;
	uint8_t sequence8bit;
	uint8_t received_char;
	//
  if (_PreviousStateOfKey == PUSHED){
    _PreviousStateOfKey = RELEASED;
    _counterReleased = 0;
  } else { // ---- KKK
    if (_counterReleased < 65535){
      _counterReleased += 1;
    }
		if (_counterReleased == _pauseBetwSentences){
			_thebuffer[_counterbuff] = '\n';
			_counterbuff++;
			_havemessage =true; // carriage return
		} else {
	    if (_counterReleased == _pauseBetwWords){
				_discard_next_char = false;
				_thebuffer[_counterbuff] = ' ';
				_counterbuff++;
				_havemessage = true; // rxSPACE;
	    } else {
	      // --- AAA
	      if (_counterReleased == _pauseBetwLetters){
					do{
						if (_now_receiving.numberOfSign > 5){
							number_of_signs = 5;
						} else {
							number_of_signs = _now_receiving.numberOfSign;
						}
						number_of_signs++;
						received_char = 0;
						do{
							// -- elaborates the 16bit sequence of signs
							// so we can compare some parts of in with the 8bit
							// BANK of SIGNS
							number_of_signs -= 1;
							uint8_t mask = 1;
							mask = mask << number_of_signs; // if number_of_signs = 4
							mask --;                // mask = 00001111
							sequence8bit = _now_receiving.sequenceOfSigns & mask;
							sequence8bit = sequence8bit | number_of_signs << 5;
							// find the character in the bank of SIGNS
							for (int n = 0; n < 36; n++){
								if (sequence8bit == _bank_of_signs[n]){
									received_char = _bank_of_letters[n];
								}
							}
						} while (received_char == 0);
						// add the character to the buffer
						_thebuffer[_counterbuff] = received_char;
						_counterbuff++;
						_now_receiving.sequenceOfSigns = _now_receiving.sequenceOfSigns >> number_of_signs;
						_now_receiving.numberOfSign -= number_of_signs;
					} while (_now_receiving.numberOfSign > 0);
					// informs (or not) if there is something to read
					if (_discard_next_char){
						clearBuffer();
						_discard_next_char = false;
					} else {
						_havemessage = true;
					}
					// reset values
					_now_receiving.numberOfSign = 0;
					_Sign = UNDEFINED;
	      } else {
	        // --- CCC
	        if (_counterReleased == _pauseBetwSigns){
						// the pause if between signs: I compose the sequence
						_now_receiving.sequenceOfSigns = _now_receiving.sequenceOfSigns << 1;
						_now_receiving.sequenceOfSigns = _now_receiving.sequenceOfSigns | _Sign;
						_now_receiving.numberOfSign++;
	        } else {
	          // the pause is another value, non useful for now
	          // so I do nothing
	        }
	        // --- CCC
	      }
	      // --- AAA
	    }
			// --- KKK
		}
  }
}
void YAMorseRx::mm_keyPressedRoutine(){
  if (_PreviousStateOfKey == RELEASED){
    _PreviousStateOfKey = PUSHED;
    _counterPushed = 0;
  } else {
    if (_counterPushed < 65535){
      _counterPushed += 1;
    }
    if (_counterPushed == _SignDash){
      _Sign = SIGN_DASH;
    } else {
      if (_counterPushed == _SignDot){
        _Sign = SIGN_DOT;
      } else {
        // (nothing)
      }
    }
  }
}
void YAMorseRx::clearBuffer(){
	_counterbuff = 0;
	for (uint8_t n =0; n< 8; n++){
		_thebuffer[n]=0;
	}
}
