#include <avr/io.h>
#include <util/delay.h>
#define F_CPU 16000000	


int main(void)
{
	DDRC |= (1<<5);      //set pin 5 of portC as output(LED)
    while (1) 
    {
		PORTC ^= (1<<5)  //flip state of pin 5
		_delay_ms(500);    //delay by half a second
    }
	return(0);
}

