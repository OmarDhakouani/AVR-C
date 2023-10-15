// 7-segment display mapping
// A: PC3, B: PC4, C: PD2, D: PD3, E: PD4, F: PD5, G: PD6, H: NC (PD7)
#define F_CPU 16000000  // Clock frequency

// Include necessary libraries
#include <avr/interrupt.h>
#include <avr/io.h>

// Segment masks for numbers 0 through 9 for PORTC and PORTD
uint8_t PC_Bitmaske[10] = { 0x00, 0x10, 0x18, 0x18, 0x10, 0x08, 0x08, 0x18, 0x18, 0x18 };
uint8_t PD_Bitmaske[10] = { 0x00, 0x04, 0x58, 0x4C, 0x64, 0x6C, 0x7C, 0x04, 0x7C, 0x6C };

// Global variable to store the number to display
uint8_t number = 0;

// ADC interrupt service routine
ISR(ADC_vect)
{
	number = (uint8_t)(ADCH);  // Obtain the 3 most significant bits from the ADC result (0-7)

	// Turn off all segments
	PORTC = 0x18;
	PORTD = 0x7C;
	
	// Update the 7-segment display based on the ADC value
	PORTC = ~PC_Bitmaske[number];
	PORTD = ~PD_Bitmaske[number];
}

// Timer interrupt service routine
ISR(TIMER0_COMPA_vect)
{
	ADCSRA |= (1 << ADSC);  // Start a new ADC sequence
}

int main(void)
{
	// Initialize ADC
	ADMUX = 0x60;  // Use AVcc as the reference voltage, left adjust for 8 bits, and select ADC channel 0
	ADCSRA = 0x8D; // Enable ADC, enable ADC interrupt, and set prescaler to 32

	// Initialize Timer/Counter 0 for CTC mode with a prescaler of 1024 and an interrupt interval of 10 ms
	TCCR0A = 0x02;
	TCCR0B = 0x05;
	OCR0A = 0x4B;
	TIMSK0 = (1 << OCIE0A);  // Enable compare interrupt A

	// Initialize 7-segment display (all LEDs off and the rest set for input and tri-state)
	DDRC = 0x18;
	PORTC = 0x18;
	DDRD = 0x7C;
	PORTD = 0x7C;

	sei();  // Enable global interrupts

	while (1)
	{
		// Main loop (nothing needed here as functionality is handled by interrupts)
	}
}