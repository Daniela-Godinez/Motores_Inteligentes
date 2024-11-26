/*
 * Prueba.c
 *
 * Created: 13/06/2024 13:49:09
 * Author : Daniela Godinez
 */ 

#define F_CPU 16000000 // Oscilador de 16MHz

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/sfr_defs.h>

#include "A4988.h"

// DEFINICION DE MACROS
#define set_bit(sfr,bit)	(_SFR_BYTE(sfr) |= _BV(bit))	// Macro para setear el bit del puerto (0 logico)
#define clear_bit(sfr,bit)	(_SFR_BYTE(sfr) &= ~_BV(bit))	// Macro para resetear el bit del puerto

// MACROS INTERNAS DE LIBRERIA <avr/sfr_defs.h> -- SOLO Lectura de bits
// #define bit_is_set(sfr, bit) (_SFR_BYTE(sfr) & _BV(bit))
// #define bit_is_clear(sfr, bit) (!(_SFR_BYTE(sfr) & _BV(bit)))

int main(void)
{	
	config_A4988();
	
	sei();
	
	/* Replace with your application code */
	while (1)
	{
		_delay_ms(10);
		//init_motor(FULL_STEP, ANTIHORARIO, 61, 50);
		//stop_motor();
		incrementar_velocidad(FULL_STEP, HORARIO, 20, 800, 50);
		//decrementar_velocidad(FULL_STEP, HORARIO, 800, 400, 50);
	}
}