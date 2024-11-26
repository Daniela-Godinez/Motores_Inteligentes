/*
 * Cliente.c
 *
 * Created: 03/07/2024 17:52:32
 * Author : Daniela Godinez
 */ 

#define F_CPU 16000000 // Oscilador de 16MHz

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/sfr_defs.h>

#include "A4988.h"
#include "AS5600.h"

// DEFINICION DE MACROS
#define set_bit(sfr,bit)	(_SFR_BYTE(sfr) |= _BV(bit))	// Macro para setear el bit del puerto (0 logico)
#define clear_bit(sfr,bit)	(_SFR_BYTE(sfr) &= ~_BV(bit))	// Macro para resetear el bit del puerto

// MACROS INTERNAS DE LIBRERIA <avr/sfr_defs.h> -- SOLO Lectura de bits
// #define bit_is_set(sfr, bit) (_SFR_BYTE(sfr) & _BV(bit))
// #define bit_is_clear(sfr, bit) (!(_SFR_BYTE(sfr) & _BV(bit)))

int main(void)
{
	init_A4988();
	init_ADC();
	int BANDERA = 1;
	sei();
    /* Replace with your application code */
    while (1) 
    {
		if (BANDERA)
		{
			Clockwise();
			//Anticlockwise();
			Microstepping(FULL_STEP);
			set_motor(100, 10);
			//move2angle(100, 0);
			BANDERA=0;
		}
		
	}
}

