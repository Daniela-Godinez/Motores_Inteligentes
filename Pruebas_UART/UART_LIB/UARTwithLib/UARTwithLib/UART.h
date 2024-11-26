/*
 * UART.h
 *
 * Created: 14/11/2024 19:54:20
 *  Author: Daniela Godinez
 */ 


#ifndef UART_H_
#define UART_H_

	#define F_CPU 16000000 // Oscilador de 16MHz
	
	#include <avr/io.h>
	#include <avr/sfr_defs.h>
	#include <avr/interrupt.h>
	#include <util/delay.h>
	#include <stdbool.h>
	#include <stdint.h>
	#include <stdlib.h>
	#include <stdio.h>
	#include <string.h>

	//-----------------------------
	//			  MACROS
	//-----------------------------
	#define set_bit(sfr,bit)	(_SFR_BYTE(sfr) |= _BV(bit))	// Macro para setear el bit del puerto (0 logico)
	#define clear_bit(sfr,bit)	(_SFR_BYTE(sfr) &= ~_BV(bit))	// Macro para resetear el bit del puerto

	// MACROS INTERNAS DE LIBRERIA <avr/sfr_defs.h> -- SOLO Lectura de bits
	// #define bit_is_set(sfr, bit) (_SFR_BYTE(sfr) & _BV(bit))
	// #define bit_is_clear(sfr, bit) (!(_SFR_BYTE(sfr) & _BV(bit)))
	
	#define BUFFER_SIZE 50

	extern char rx_buffer[BUFFER_SIZE];
	uint8_t rx_index;
	uint8_t receiving;
	uint8_t flag_error;
	
	//-----------------------------
	//       SETTING FUNCTION
	//-----------------------------
	void UART_init(unsigned int ubrr);
	char UART_receive(void);
	void UART_transmit(char data);
	void UART_transmit_string(const char *str);
	void UART_receive_delimited_string(void);
	uint8_t check_data_ranges(int data1, int data2, int data3, int data4);
	


#endif /* UART_H_ */