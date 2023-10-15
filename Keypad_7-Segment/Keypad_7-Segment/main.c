//		  A
//		#####
//	   #     #
//    F#     #B
//     #  G  #
//      #####
//     #     #
//    E#     #C
//     #  D  #
//      #####  H
//			   #
//
// A: PC3,  B: PC4,  C: PD2,  D: PD3,  E: PD4,  F: PB0,  G: PB1,  H: NC (PD7)
//
//
//			S0	S1	S2
//		Z0	1	2	3
//		Z1	4	5	6
//		Z2	7	8	9
//		Z3	*	0	#
//
// S0: PD5,  S1: PD6,  S2: PD7
// Z0: PB2,  Z1: PB3,  Z2: PB4,  Z3: NC

#define F_CPU 16000000
#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>

#define FALSE 0
#define TRUE 1

uint8_t PC_Bitmaske[10] = {0xFF, 0xF7, 0xFF, 0xFF, 0xF7, 0xEF, 0xEF, 0xFF, 0xFF, 0xFF};
uint8_t PD_Bitmaske[10] = {0xBF, 0x87, 0xDB, 0xCF, 0xE7, 0xEF, 0xFF, 0x87, 0xFF, 0xEF};
uint8_t PB_Bitmaske[10] = {0xFC, 0xFC, 0xFC, 0xFC, 0xFC, 0xFD, 0xFE, 0xFC, 0xFC, 0xFC};

uint8_t number = 0;
uint8_t trigger = FALSE;

ISR(TIMER0_COMPA_vect) {
	trigger = TRUE;
}

int main(void) {
	// Initialize 7 Segment Display
	DDRC  = 0x18;
	PORTC = 0x18;
	DDRD  = 0x1C;  // PD2, PD3, PD4 as outputs
	PORTD = 0x1C;
	DDRB  = 0x03;  // PB0, PB1 as outputs
	PORTB = 0x03;

	// Initialize Keypad
	DDRD  |= 0xE0;  // PD5, PD6, PD7 as outputs
	PORTD |= 0xE0;
	DDRB  &= ~(0x1C);  // PB2, PB3, PB4 as inputs
	PORTB &= ~(0x1C);

	// Initialize Timer/Counter 0
	TCCR0A = 0x02;
	TCCR0B = 0x05;
	OCR0A  = 156;
	TIMSK0 = 0x02;

	sei();

	while (1) {
		_delay_us(100);

		if (trigger) {
			trigger = FALSE;
			number = 0;

			// Check which button is pressed
			for (uint8_t col = 0; col < 3; col++)
			{
				for (uint8_t row = 0; row < 3; row++)
				{
					if (!(PINB & (1 << (row + 2))) && !(PIND & (1 << (col + 5))))
					{
						number = row * 3 + col + 1;
						break;  // Break out of the row loop once a button is detected
					}
				}
				if (number) break;  // Break out of the column loop if a button is detected
			}
		}

		if (!number) {
			PORTC = 0xFF;
			PORTD = 0xFF;
			PORTB = 0xFF;
			} else {
			PORTC = ~PC_Bitmaske[number];
			PORTD = ~PD_Bitmaske[number];
			PORTB = ~PB_Bitmaske[number];
		}
	}
}