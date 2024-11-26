/*
 * ADC.h
 *
 * Created: 27/10/2024 18:35:12
 *  Author: Daniela Godinez
 */ 


#ifndef ADC_H_
#define ADC_H_

	#define F_CPU 16000000 // Oscilador de 16MHz
	
	#include <avr/io.h>
	#include <avr/sfr_defs.h>
	#include <avr/interrupt.h>
	#include <util/delay.h>
	#include <stdbool.h>
	#include <stdint.h>
	#include <stdlib.h>
	
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
	extern uint16_t new_zero;
	#define adc_resolution 1023.0
	#define full_circle_degree 360.0
	#define max_voltage 5.0
	#define channel_mask 0b00001111
	#define channel_shift_mask 0xF0
	
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

#endif /* ADC_H_ */