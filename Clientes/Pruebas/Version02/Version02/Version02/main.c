/*
 * Version02.c
 *
 * Created: 19/09/2024 20:36:53
 * Author : Daniela Godinez
 */ 

#define F_CPU 16000000 // Oscilador de 16MHz

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/sfr_defs.h>
#include <stdint.h>
#include <stdlib.h>

#include "A4988.h"
#include "ADC.h"

// DEFINICION DE MACROS
#define set_bit(sfr,bit)	(_SFR_BYTE(sfr) |= _BV(bit))	// Macro para setear el bit del puerto (0 logico)
#define clear_bit(sfr,bit)	(_SFR_BYTE(sfr) &= ~_BV(bit))	// Macro para resetear el bit del puerto

// MACROS INTERNAS DE LIBRERIA <avr/sfr_defs.h> -- SOLO Lectura de bits
// #define bit_is_set(sfr, bit) (_SFR_BYTE(sfr) & _BV(bit))
// #define bit_is_clear(sfr, bit) (!(_SFR_BYTE(sfr) & _BV(bit)))

void UART_init(unsigned int baud) {
	unsigned int ubrr = F_CPU/16/baud-1;
	UBRR0H = (unsigned char)(ubrr>>8);
	UBRR0L = (unsigned char)ubrr;
	UCSR0B = (1<<RXEN0)|(1<<TXEN0);  // Enable receiver and transmitter
	UCSR0C = (1<<UCSZ01)|(1<<UCSZ00); // Set frame: 8 data bits, 1 stop bit
}

void UART_transmit(unsigned char data) {
	while (!(UCSR0A & (1<<UDRE0)));  // Wait for empty transmit buffer
	UDR0 = data;                     // Put data into buffer, sends the data
}

void UART_printString(const char* str) {
	while (*str) {
		UART_transmit(*str++);
	}
}
char str[10];
float dato_adc;
float dato_angle;
float dato_speed;
int main(void)
{
	UART_init(9600);
	init_ADC();
	init_A4988();
	
	int BANDERA = 1;
	sei();
    /* Replace with your application code */
    while (1) 
    {
		if (BANDERA)
		{
			//Clockwise();
			Anticlockwise();
			Microstepping(FULL_STEP);
			//set_origin();
			_delay_ms(5000);
			//dato_move2angle = move2angle(10, 200);
			set_motor(20, 5);
			//set_motor(10, 5);
			//move2angle(6, 45);
			//stop_motor();
			BANDERA=0;
		}
		dato_angle = read_angle();
		dato_adc = read_current();
		dato_speed = read_speed();
		itoa(dato_angle, str, 10);	
		UART_printString("Angle:");	
		UART_printString(str);  // Send a string
		UART_printString("\r\n");
		_delay_ms(10);  // Delay for a second
		UART_printString("Corriente:");
		itoa(dato_adc, str, 10);
		UART_printString(str);  // Send a string
		UART_printString("\r\n");
		_delay_ms(10);  // Delay for a secod
		UART_printString("Velocidad:");
		itoa(dato_speed, str, 10);
		UART_printString(str);  // Send a string
		UART_printString("\r\n");
		_delay_ms(1000);  // Delay for a secod
    }
}

