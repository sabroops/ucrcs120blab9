/*	Author: lab
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #  Exercise #
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

void set_PWM(double frequency){
	static double current_frequency;
	if (frequency != current_frequency){
		if (!frequency) {TCCR3B &= 0x08;}
		else {TCCR3B |= 0x03;}
		if (frequency < 0.954) { OCR3A = 0xFFFF; }
		else if (frequency > 31250) {OCR3A = 0x0000; }
		else { OCR3A = (short) (8000000 / (128 * frequency)) - 1; }
		TCNT3 = 0;
		current_frequency = frequency;
	}
}

void PWM_on(){
	TCCR3A = (1 << COM3A0);
	TCCR3B = (1 << WGM32) | (1 << CS31) | (1 << CS30);
	set_PWM(0);
}

void PWM_off(){
	TCCR3A = 0x00;
	TCCR3B = 0x00;
}

//enum States{Start, Press, Release, WAIT}state;
unsigned char tmpA = 0x00;
unsigned char sound = 0x00;
unsigned char stateTracker = 0x00;

/*
void Toggle_Tick(){
	switch(state){
		case Start:
			state = WAIT;
			break;
		case WAIT:
			if (tmpA ==1) { state = Press;}
			else{
				state = WAIT;
			}
			break;
		case Press: 
			if (tmpA == 1){
				state = Release;
			}
			else{
				state = WAIT;
			}
			break;
		case Release: //wait for release 
			if (tmpA == 1){
				state = Release;
			}
			else{
				state = WAIT;
			}
			break;
		}
	switch(state){
		case Start:
			break;
		case WAIT:
			break;
		case Press:
			stateTracker = 0;
			sound = ~sound;
			if (!sound){
				set_PWM(0);
			}
			break;
		case Release:
			break;
	}
}
*/
enum States{start,wait, OnWait, OnRelease, OffPressWait, inc, dec, incWait,decWait}state;

//unsigned char stateTracker = 0x00;
double arr[] = {261.63, 293.66, 329.63, 349.23, 392.00, 440.00, 493.88, 523.25};

void Sound_Tick(){
	switch(state){
		case start:
			state = wait;
			break;
		case wait:
			if (tmpA == 0x00){ state = wait;}
			else if(tmpA == 0x01){
				//if (sound){
				state = OnWait;
				//}
				//else{
				//	state = OffPressWait;
				//}//
			}
			else if(tmpA == 0x02){
				state = inc;
			}
			else if (tmpA == 0x04){
				state = dec;
			}
			else{
				state = wait;
			}
			break;
		case OnWait:
			if (tmpA == 0x00){
				state = OnRelease;
			}
			else{
				state = OnWait;
			}
			break;
		case OnRelease:
			if (tmpA == 0x01){
				state = OffPressWait;
			}
			else if (tmpA == 0x02){
				state = inc;
			}
			else if (tmpA == 0x04){
				state = dec;
			}
			else{
				state = OnRelease;
			}
			break;
		case OffPressWait:
			if (tmpA == 0x01){
				state = OffPressWait;
			}
			else if (tmpA == 0x00){
				state = wait;
			}
			else{
				state = OffPressWait;
			}
			break;
		case inc:
			if (tmpA == 0x02){
				state = incWait;
			}
			else{
				state = OnRelease;
			}
			break;
		case incWait:
			if (tmpA == 0x02){
                               state = incWait;
                        }
                        else if (sound){
                                state = OnRelease;
                        }
			else{
				state = wait;
			}
                        break;
		case dec:
	//		if (tmpA == 0x04){
                                state = decWait;
      //                  }
    //                    else{
  //                              state = OnRelease;
//                        }
                        break;
		case decWait:
			if (tmpA == 0x04){
                                state = decWait;
                        }
                        else if (sound){
                                state = OnRelease;
                        }
			else{
				state = wait;
			}
                        break;
	}
	switch(state){
		case start:
			set_PWM(0);
			break;
		case wait:
			set_PWM(0); //WHY IS THIS NOT WORKING
			break;
		case OnWait:
			sound = 0x01;
			set_PWM(arr[stateTracker]);
			break;
		case OnRelease:
			set_PWM(arr[stateTracker]);
			break;
		case OffPressWait:
			sound = 0x00;
			set_PWM(0);
			break;
		case inc:
			if (stateTracker < 7) stateTracker++;
			break;
		case dec:
			if (stateTracker > 0) stateTracker--;
			break;
		case incWait:
			break;
		case decWait:
			break;
	}
}

int main(void) {
    /* Insert DDR and PORT initializations */
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0x40; PORTB = 0xBF;
    /* Insert your solution below */
	PWM_on();
	state = start;
    while (1) {
	tmpA = ~PINA & 0x07;
//	Toggle_Tick();
//	if (sound == 0x01) {
	Sound_Tick();
//	}
//	else{
//		set_PWM(0);
//	}
    }
    return 1;
}
