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

enum States{Start,wait, C, D, E}state;
unsigned char tmpA = 0x00;

void Tick(){
	switch(state){
		case Start:
			state = wait;
			break;
		case wait:
			if(tmpA == 0x01){
				state = C;
			}
			else if (tmpA == 0x02){
				state = D;
			}
			else if (tmpA == 0x04){
				state = E;
			}
			else{
				state = wait;
			}
			break;
		case C:
			if (tmpA == 0x01){
				state = C;
			}
			else{
				state = wait;
			}
			break;
		case D:
			if (tmpA == 0x02){
                                state = D;
                        }
                        else{
                                state = wait;
                        }
                        break;
                case E:
                        if (tmpA == 0x04){
                                state = E;
                        }
                        else{
                                state = wait;
                        }
                        break;
	}
	switch(state){
		case Start:
			set_PWM(0);
			break;
		case wait:
			set_PWM(0);
			break;
		case C:
			set_PWM(261.63);
			break;
		case D:
			set_PWM(293.66);
			break;
		case E:
			set_PWM(329.63);
			break;
	}
}

int main(void) {
    /* Insert DDR and PORT initializations */
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0x40; PORTB = 0xBF;
    /* Insert your solution below */
	PWM_on();
	state = Start;
    while (1) {
	tmpA = ~PINA & 0x07;
	Tick();
    }
    return 1;
}
