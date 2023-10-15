#define F_CPU 16000000  // Define clock frequency for delay functions
#define SKIP_ROM 0xCC
#define CONVERT_T 0x44
#define READ_SCRATCHPAD 0xBE
#define READ_ROM 0x33
#define MATCH_ROM 0x55
#define DS18B20_FAMILY_CODE 0x28
#define MAX_DEVICES 3  // Maximum expected number of DS18B20 devices

// Include necessary libraries
#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include "wire1.h"  // Custom library for 1-Wire communication
#include "uart.h"   // Custom library for UART communication

int16_t temperature_10;  // Variable to store temperature multiplied by 10
uint8_t w1_devices_rom_codes[MAX_DEVICES][1];  // Array to store ROM codes of detected devices
uint8_t w1_devices;  // Variable to store number of detected devices

int main(void)
{
	uart_setup(UART_RX_DISABLE, UART_TX_ENABLE, 19200);  // Initialize UART with specific settings
	w1_setup(&PORTC, 2);  // Set up 1-Wire communication on PORTC, pin 2
	sei();  // Enable global interrupts

	// Print a header message
	printf("\r\n----------------------\r\n1 Wire Bus Thermometer\r\n----------------------\r\n\r\n");

	// Initialize 1-Wire communication and proceed only if devices are detected
	if (w1_init())
	{
		// Search for DS18B20 temperature sensors and get their count
		w1_devices = w1_search(&w1_devices_rom_codes[MAX_DEVICES][1], DS18B20_FAMILY_CODE);

		// Print the number of sensors found
		printf("Sensors found: %d\r\n\r\n", w1_devices);

		// Print ROM codes of detected sensors
		for (uint8_t i = 0; i < w1_devices; i++)
		{
			printf("Sensor:%d  ROM-Code:%X \r\n", i, w1_devices_rom_codes[i][1]);
		}

		// Print a message if no sensors were found
		if (w1_devices == 0)
		{
			printf("No sensors found on the 1-Wire bus.\r\n\r\n");
		}

		// Continuously read temperature from detected sensors
		while (1)
		{
			for (uint8_t i = 0; i < w1_devices; i++)
			{
				// Read temperature from a sensor
				temperature_10 = ds1820_temperature_10(&w1_devices_rom_codes[i][1]);

				// Print the temperature if reading was successful
				if (temperature_10 != -9999)
				{
					printf("Temperature sensor:0x%X Temperature=%d°C/10 \r\n", w1_devices_rom_codes[i][1], temperature_10);
				}
				// Otherwise, print an error message
				else
				{
					printf("Error reading temperature from sensor: 0x%X\r\n", w1_devices_rom_codes[i][1]);
				}
			}
		}
	}
}