/*
 * Master.c
 *
 * Created: 16/11/2024 17:09:51
 * Author : Daniela Godinez
 */ 

#define F_CPU 16000000 // Oscilador de 16MHz

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/sfr_defs.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// DEFINICION DE MACROS
#define set_bit(sfr,bit)	(_SFR_BYTE(sfr) |= _BV(bit))	// Macro para setear el bit del puerto (0 logico)
#define clear_bit(sfr,bit)	(_SFR_BYTE(sfr) &= ~_BV(bit))	// Macro para resetear el bit del puerto

// MACROS INTERNAS DE LIBRERIA <avr/sfr_defs.h> -- SOLO Lectura de bits
// #define bit_is_set(sfr, bit) (_SFR_BYTE(sfr) & _BV(bit))

void SPI_init(){
	// SPI's PIN - OUTPUTS
	set_bit(DDRB, 3);		// MOSI - Output
	set_bit(DDRB, 5);		// SCK - Output
	set_bit(DDRB, 2);		// SS - Output
	// SPI's PIN - INPUTS
	clear_bit(DDRB, 4);		// MISO - Input
	// SPI's PIN - HIGH OUTPUTS
	set_bit(PORTB, 2);		// SS - Salida en alto
	// SPCR - SPI CONTROL REGISTER
	set_bit(SPCR, SPE);		// Enable SPI
	set_bit(SPCR, MSTR);	// Master mode activated
	// Clock frequency - 001 (Fosc/16)
	clear_bit(SPCR, SPI2X);
	clear_bit(SPCR, SPR1);
	clear_bit(SPCR, SPR0);
}

void SPI_send(uint8_t data){
	SPDR = data;					// Load data into the SPDR register
	while(!(SPSR & (1 << SPIF)));	// Wait until transmission is complete
}

uint8_t SPI_receive(void){
	SPDR = 0xFF;
	while(!(SPSR & (1 << SPIF)));	// Wait until transmission is complete
	return SPDR;					// Return into the SPDR register
}

void SPI_SS_OFF(void){
	set_bit(PORTB, 2);
}

void SPI_SS_ON(void){
	clear_bit(PORTB, 2);
}

void auto_addressing(void){
	for (uint8_t i = 0; i < 14; i++) {
		SPI_SS_ON();
		SPI_send(0x01);         // Enviar valor al primer esclavo
		_delay_ms(10);          // Esperar un poco
		//uint8_t lol = SPI_receive();
		SPI_SS_OFF();
		_delay_ms(10);          // Retraso para estabilidad
	}
}

int id = 1;
int fnc = 3;
int prm[2] = {123, 56};
char jsonout[50];
uint8_t lol1 = 0;
int main(void)
{
	set_bit(DDRD, 2);
	set_bit(DDRD, 3);
	set_bit(DDRD, 4);
	set_bit(DDRD, 5);
	SPI_init();
	_delay_ms(10);
    /* Replace with your application code */
	auto_addressing();
	uint8_t bandera = 1;
	if(lol1 == 4){
		set_bit(PORTD,2);
	}
    while (1) 
    {
		_delay_ms(10);
		if (bandera)
		{
			sprintf(jsonout, "{\"id\":%u, \"fnc\":%u, \"prm\":[%u, %u]}", id, fnc, prm[0], prm[1]);
			
			for(uint8_t i = 0; i < 46; i++) {
				SPI_SS_ON();
				SPI_send((uint8_t)jsonout[i]);
				_delay_ms(10); // Ajusta este delay según necesidad
				SPI_SS_OFF();
			}
			
			bandera = 0;
		}
    }
}

