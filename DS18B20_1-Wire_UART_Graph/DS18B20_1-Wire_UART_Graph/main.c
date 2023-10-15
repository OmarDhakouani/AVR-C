#define F_CPU 16000000

// 1-Wire Command Codes
#define SKIP_ROM 0xCC
#define CONVERT_T 0x44
#define READ_SCRATCHPAD 0xBE

// Include necessary library files
#include <avr/io.h>
#include <util/delay.h>
#include "wire1.h"  // Must be located in the working directory
#include "uart.h"   // Must be located in the working directory

// Declaration of global variables
uint16_t temp_value;                // Assembled 16-bit value dependent on temperature
uint8_t temp_high_byte, temp_low_byte;  // High and low part of the reading value
uint8_t graph_elements;
uint8_t i;                          // Number of characters to be displayed for the graph

int main(void)
{
	// Initialize UART 0
	uart_setup(UART_RX_DISABLE, UART_TX_ENABLE, 19200);  // Disable receiving, enable sending, Baud rate ... Bit/s

	// Initialize 1-Wire interface
	w1_setup(&PORTC, 2);  // Choose port and port bit according to hardware description

	sei();  // Library functions use interrupts, so they must be globally enabled

	while (1)
	{
		if (w1_init())  // Initialize a new 1-Wire command sequence and only send further if a participant is detected on the bus
		{
			// Start temperature measurement
			w1_write(CONVERT_T);
			
			// Wait until a new temperature value is available, then read both bytes of the temperature value from the DS18B20's scratchpad
			w1_read();
			while (w1_read() != 0)
			w1_write(READ_SCRATCHPAD);

			temp_low_byte = w1_read();
			temp_high_byte = w1_read();

			// Combine both temperature bytes into a 16-bit value
			temp_value = (temp_low_byte >> 4) + ((temp_high_byte & 0x07) << 5);

			// Convert this 16-bit temperature value (with a resolution of 12-bit, signed) to a 7-bit temperature value (range 0 to 127) for the bar chart display
			graph_elements = (temp_value & 0x00FE);

			// Create a temperature graph as a bar chart from "stacked" text characters, sending a temperature-dependent number of characters per text line via USART 0 for each new measurement
			for (i = 1; i < graph_elements + 1; i++)
			{
				printf("*");  // Alternatively, another character can be used
			}
			
			// End each line of "stacked" text characters with carriage return and new line control characters
			printf("\r\n");
		}
		else  // If no participants were found on the 1-Wire bus...
		{
			printf("No bus participant found\r\n\r\n");  // ... send a corresponding message via USART 0
			_delay_ms(2000);  // ... and repeat this message every two seconds
		}
	}
}





