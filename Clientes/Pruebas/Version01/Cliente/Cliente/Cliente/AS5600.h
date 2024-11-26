/*
 * AS5600.h
 *
 * Created: 05/09/2024 20:43:08
 *  Author: Daniela Godinez
 */ 


#ifndef AS5600_H_
#define AS5600_H_

	#define F_CPU 16000000 // Oscilador de 16MHz
	
	#include <avr/io.h>
	#include <avr/sfr_defs.h>
	#include <avr/interrupt.h>
	#include <util/delay.h>
	#include <stdbool.h>
	
	//-----------------------------
	//			  MACROS
	//-----------------------------
	#define set_bit(sfr,bit)	(_SFR_BYTE(sfr) |= _BV(bit))	// Macro para setear el bit del puerto (0 logico)
	#define clear_bit(sfr,bit)	(_SFR_BYTE(sfr) &= ~_BV(bit))	// Macro para resetear el bit del puerto

	// MACROS INTERNAS DE LIBRERIA <avr/sfr_defs.h> -- SOLO Lectura de bits
	// #define bit_is_set(sfr, bit) (_SFR_BYTE(sfr) & _BV(bit))
	// #define bit_is_clear(sfr, bit) (!(_SFR_BYTE(sfr) & _BV(bit)))
	
	//-----------------------------
	//      GLOBAL VARIABLES
	//-----------------------------
	uint16_t new_zero;
	
	//-----------------------------
	//       SETTING FUNCTION
	//-----------------------------
	void init_ADC(void);
	uint16_t ADC_read(uint8_t canal);
	uint16_t ADC2angle(void);
	
	//-----------------------------
	//       USER's FUNCTIONS
	//-----------------------------
	uint16_t set_origin(void);
	uint16_t read_angle(void);
	uint16_t read_current(void);

#endif /* AS5600_H_ */