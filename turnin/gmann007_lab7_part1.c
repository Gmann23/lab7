/*	Author: lab
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #  Exercise #
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *
 *Demo Video: Programming chip is broken have to wait till monday.
 *
 * code, is my own original work.
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#include "io.h"
#endif

volatile unsigned char TimerFlag = 0;
unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;
unsigned char tmp = 0x00;
unsigned char dis = 0x00;

void TimerOn() {
        TCCR1B = 0x0B;
        OCR1A = 125;
        TIMSK1 = 0x02;
        TCNT1 = 0;
        _avr_timer_cntcurr = _avr_timer_M;
        SREG |= 0x80;
}

void TimerISR() { TimerFlag = 1;}

void TimerOff() {
        TCCR1B = 0x00;
}
ISR(TIMER1_COMPA_vect) {
        _avr_timer_cntcurr--;
        if (_avr_timer_cntcurr == 0) {
                TimerISR();
                _avr_timer_cntcurr = _avr_timer_M;
        }
}
void TimerSet (unsigned long M) {
        _avr_timer_M = M;
        _avr_timer_cntcurr = _avr_timer_M;
}

enum States {Start,Begin,Add,Sub,Inc,Dec,Reset} state;

void Tick(){
         switch(state) { //transitions
		case Start:
			state = Begin;
			break;

		case Begin:
			if ((~PINA & 0x03) == 0x01) {
				state = Add;
			} else if ((~PINA & 0x03) == 0x02) {
				state = Sub;
			} else if ((~PINA & 0x03) == 0x03) {
				state = Reset;
			} else if ((~PINA & 0x03) == 0x00) {
				state = Begin;
			}
			break;

		case Add: 
			if ((~PINA & 0x03) == 0x01) {
				state = Inc;
			} else if ((~PINA & 0x03) == 0x02) {
				state = Sub;
			} else if ((~PINA & 0x03) == 0x03) {
				state =  Reset;
			} else if ((~PINA & 0x03) == 0x00) {
				state = Begin;
			}
			break;

		case Sub: 
			if ((~PINA & 0x03) == 0x01) {
				state = Add;
			} else if ((~PINA & 0x03) == 0x02) {
				state = Dec;
			} else if ((~PINA & 0x03) == 0x03) {
				state =  Reset;
			} else if ((~PINA & 0x03) == 0x00) {
				state = Begin;
			}
			break;
			
		case Inc: 
			if ((~PINA & 0x03) == 0x01) {
				state = Inc;
			} else if ((~PINA & 0x03) == 0x02) {
				state = Sub;
			} else if ((~PINA & 0x03) == 0x03) {
				state =  Reset;
			} else if ((~PINA & 0x03) == 0x00) {
				state = Begin;
			}
			break;
		
		case Dec: 
			if ((~PINA & 0x03) == 0x01) {
				state = Add;
			} else if ((~PINA & 0x03) == 0x02) {
				state = Dec;
			} else if ((~PINA & 0x03) == 0x03) {
				state =  Reset;
			} else if ((~PINA & 0x03) == 0x00) {
				state = Begin;
			}
			break;

	       case Reset:
                        if ((~PINA & 0x03) == 0x01) {
                                state = Add;
                        } else if ((~PINA & 0x03) == 0x02) {
                                state = Sub;
                        } else if ((~PINA & 0x03) == 0x03) {
                                state =  Reset;
                        } else if ((~PINA & 0x03) == 0x00) {
                                state = Begin;
                        }
                        break;

	}
		
	switch(state){ 
		case Start:
			dis = 0x00;
			LCD_ClearScreen();
			LCD_WriteData( dis + '0');
			tmp = 0x00;
			break;

		case Begin:
			tmp = 0x00;
			LCD_ClearScreen();
			LCD_WriteData( dis + '0');
			break;

		case Add: 
			tmp = 0x00;
			if (dis < 0x09) {
				dis = dis + 0x01;
				LCD_ClearScreen();
				LCD_WriteData( dis + '0');
			}
			break;

		case Sub: 
			tmp = 0x00;
			if (dis > 0x00) {
				dis = dis - 0x01;
				LCD_ClearScreen();
				LCD_WriteData( dis + '0');
			}
			break;

		
		case Inc: 
			if ((dis < 0x09) && (tmp >= 0x0A)) {
				dis = dis + 0x01;
				tmp = 0x00;
				LCD_ClearScreen();
				LCD_WriteData( dis + '0');
			}
			tmp = tmp + 0x01;
			break;
		
		case Dec: 
			if ((dis > 0x00) && (tmp >= 0x0A)) {
				dis = dis - 0x01;
				tmp = 0x00;
				LCD_ClearScreen();
				LCD_WriteData( dis + '0');
			}
			tmp = tmp + 0x01;
			break;

			 case Reset:
                        dis = 0x00;
                        tmp = 0x00;
                        LCD_ClearScreen();
                        LCD_WriteData( dis + '0');
                        break;

	}
}

int main(void)
{
	DDRA = 0x00;
	PORTA = 0xFF;

	DDRC = 0xFF;
       	PORTC = 0x00; 
	DDRD = 0xFF;
       	PORTD = 0x00; 
	LCD_init();
	LCD_DisplayString(1, dis);
	
	
	state = Start;
	TimerSet(100);
	TimerOn();
	while(1) {
		Tick();
		while (!TimerFlag);
		TimerFlag = 0;
	}
}


