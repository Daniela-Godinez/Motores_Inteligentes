/*
 * ADC.c
 *
 * Created: 03/08/2024 11:41:01
 * Author : Daniela Godinez
 */ 

/* Recibira un voltaje, una se?al anal?gica. Para convertirlo en se?al digital */

// Canal de ADC: A0 (PC0)
// Canal de ADC: A1 (PC1)

/*
Se configuran los dos registros del ADC. ADCH ^ ADCL -> para los bits parte alta y baja.
Sin embargo, se configurara para el lado derecho (Right Aligned) para que los bits menos significativos estar?n en la parte alta.
LSB --> ADCH
*/ 

#define F_CPU 16000000 // Oscilador de 16MHz

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/sfr_defs.h>
#include <stdio.h>

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
uint16_t dato_adc;

int main(void)
{
	UART_init(9600);
	ADC_init();
	int BANDERA = 1;
    /* Replace with your application code */
    while (1) 
    {
		//dato_adc = ADC_read(0);
		//itoa(dato_adc, str, 10);
		
		//if(BANDERA){
			//new_zero = set_origin();
			//BANDERA = 0;
		//}
		dato_adc = ADC_read(0);
		itoa(dato_adc, str, 10);
		//sprintf(str, "%.2f", dato_adc);	
				
		UART_printString(str);  // Send a string
		UART_printString("\r\n");
		_delay_ms(1000);  // Delay for a second
    }
}

