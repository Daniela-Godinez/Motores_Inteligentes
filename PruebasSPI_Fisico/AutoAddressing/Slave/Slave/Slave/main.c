/*
 * Slave.c
 *
 * Created: 16/11/2024 17:41:49
 * Author : Daniela Godinez
 */ 

#define F_CPU 16000000 // Oscilador de 16MHz

#include <avr/io.h>
#include <avr/sfr_defs.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

//-----------------------------
//			  MACROS
//-----------------------------
#define set_bit(sfr,bit)	(_SFR_BYTE(sfr) |= _BV(bit))	// Macro para setear el bit del puerto (0 logico)
#define clear_bit(sfr,bit)	(_SFR_BYTE(sfr) &= ~_BV(bit))	// Macro para resetear el bit del puerto

// MACROS INTERNAS DE LIBRERIA <avr/sfr_defs.h> -- SOLO Lectura de bits
// #define bit_is_set(sfr, bit) (_SFR_BYTE(sfr) & _BV(bit))
// #define bit_is_clear(sfr, bit) (!(_SFR_BYTE(sfr) & _BV(bit)))

void SPI_init(){
	// SPI's PIN
	clear_bit(DDRB, 3);		// MOSI - Input
	set_bit(DDRB, 4);		// MISO - Output
	clear_bit(DDRB, 5);		// SCK - Input
	clear_bit(DDRB, 2);		// SS - Input
	// SPCR - SPI CONTROL REGISTER
	SPCR = (1 << SPE) | (1 << SPIE);
	sei(); // Enable global interrupts
}

void SPI_send(uint8_t data){
	SPDR = data;					// Load data into the SPDR register
	while(!(SPSR & (1 << SPIF)));	// Wait until transmission is complete
}

uint8_t SPI_receive(void){
	while(!(SPSR & (1 << SPIF)));	// Wait until transmission is complete
	return SPDR;					// Return into the SPDR register
}

uint8_t data = 0;
uint8_t counter = 0;
uint8_t flag = 1;
uint8_t my_address = 0;
volatile char jsonin[41];

uint8_t counter_spi = 0;
bool recording = false;

ISR(SPI_STC_vect) {  // SPI interrupt routine
	if (flag)
	{
		data = SPDR;  // Read received data once
		if (my_address == 0 && data < 16)
		{
			my_address = data;
		}else if (my_address != 0)
		{
			flag = 0;
		}
		counter =  data + 1;
		SPDR = counter;
	}else{
		data = SPDR; 
		uint8_t new_data = data;
		if ((char*)new_data == '{')
			{
				recording = true;
				counter_spi = 0;
				// Limpiar el buffer antes de iniciar una nueva grabación
				memset(jsonin, 0, 41);
			}
		if ((char*)new_data == '}' && recording)
		{
			recording = false;
		}
		
		if (recording)
		{
			if (counter_spi < 41)
			{
				jsonin[counter_spi] = (char*)new_data;
				counter_spi++;
			}
		}
		SPDR = new_data;
	}
	
	
}

uint8_t id_read;
uint8_t fnc_read;
uint8_t prm1_read;
uint8_t prm2_read;

int main(void)
{
	set_bit(DDRD, 2);
	set_bit(DDRD, 3);
	set_bit(DDRD, 4);
	set_bit(DDRD, 5);
	SPI_init();
    /* Replace with your application code */
    while (1) 
    {
		sscanf(jsonin, "{\"id\":%d, \"fnc\":%u, \"prm\":[%u, %u]}", &id_read, &fnc_read, &prm1_read, &prm2_read);
		if (my_address == id_read)
		{
			set_bit(PORTD, 2);
			clear_bit(PORTD, 3);
		}else{
			clear_bit(PORTD, 2);
			set_bit(PORTD, 3);
		}
		//switch(my_address){
			//case 1:
			//set_bit(PORTD, 2);
			//clear_bit(PORTD, 3);
			//clear_bit(PORTD, 4);
			//break;
			//case 2:
			//clear_bit(PORTD, 2);
			//set_bit(PORTD, 3);
			//clear_bit(PORTD, 4);
			//break;
			//case 3:
			//clear_bit(PORTD, 2);
			//clear_bit(PORTD, 3);
			//set_bit(PORTD, 4);
			//break;
		//}
    }
}

