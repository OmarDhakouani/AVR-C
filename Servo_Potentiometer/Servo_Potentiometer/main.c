#define F_CPU 16000000

#include <avr/interrupt.h>
#include <avr/io.h>

// Global variable declaration
int16_t servo_offset;   // Offset value to adjust the servo's middle position

// Compare register values for PWM operation
#define SERVO_PERIOD 40000      // Compare value of 40,000 corresponds to 20 ms
#define SERVO_PULSE_MAX 4000    // Compare value of 4,000 corresponds to 2 ms (servo max value)
#define SERVO_PULSE_MID 3000    // Compare value of 3,000 corresponds to 1.5 ms (servo middle position)
#define SERVO_PULSE_MIN 2000    // Compare value of 2,000 corresponds to 1 ms (servo min value)

// Interrupt handler for ADC
ISR(ADC_vect) {
	servo_offset = (ADC >> 5) - 4;   // Calculate servo_offset from ADC value (range: -1024 to 1023)
	OCR1B = SERVO_PULSE_MID + 1000 * (servo_offset >> 2);   // Calculate compare value for t_high = 1 to 2 ms from servo_offset
}

// Interrupt handler for Timer/Counter 0
ISR(TIMER0_COMPA_vect) {
	ADCSRA |= (1 << ADSC);  // Start a new ADC sequence
}

// Interrupt handler for Timer/Counter 1 (start of a new PWM period)
ISR(TIMER1_COMPB_vect) {
	PORTC |= (1 << 0);  // Set servo output to HIGH
}

// Interrupt handler for Timer/Counter 1 (end of HIGH phase of a PWM period)
ISR(TIMER1_OVF_vect) {
	PORTC &= ~(1 << 0);  // Set servo output to LOW
}

int main(void) {
	// Initialize ADC
	ADMUX = 0x40;   // ADC reference voltage AVcc, right-aligned for 10-bit, use channel 0 for ADC input
	ADCSRA = 0x8D;  // Enable ADC, ADC Interrupt, and set frequency divider to 32

	// Initialize Timer/Counter 0: CTC mode, Prescaler 1/1024, interrupt interval of 10 ms
	TCCR0A = 0x02;
	TCCR0B = 0x05;
	OCR0A = 0x4B;   
	TIMSK0 = (1 << OCIE0A);  // Enable Compare-Interrupt A

	// Initialize Timer/Counter 1 for PWM generation: Fast PWM, Prescaler 1/8, Compare Match IRQ B, Overflow IRQ
	TCCR1A = 0x03;
	TCCR1B = 0x1A;
	OCR1A = SERVO_PERIOD;  // Define the PWM period
	OCR1B = SERVO_PULSE_MID;  // Define PWM pulse width for servo middle position
	TIMSK1 = (1 << OCIE1B) | (1 << TOIE1);  // Enable Compare-Interrupt B and Overflow-Interrupt

	// Initialize servo
	DDRC |= (1 << 0);    // Set servo output to OUTPUT mode
	PORTC &= ~(1 << 0);  // Set servo output to LOW

	sei();  // Globally enable interrupts

	while (1) {
		// No additional code needed here
	}
}