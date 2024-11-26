/*
 * UART.c
 *
 * Created: 14/11/2024 19:54:11
 *  Author: Daniela Godinez
 */ 

#include "UART.h"

void UART_init(unsigned int ubrr) {
	UBRR0H = (unsigned char)(ubrr >> 8);
	UBRR0L = (unsigned char)ubrr;
	UCSR0B = (1 << RXEN0) | (1 << TXEN0);
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

// UART receive
char UART_receive(void) {
	while (!(UCSR0A & (1 << RXC0)));
	return UDR0;
}

// UART transmit
void UART_transmit(char data) {
	while (!(UCSR0A & (1 << UDRE0)));
	UDR0 = data;
}

// Send a string (for debugging/error messages)
void UART_transmit_string(const char *str) {
	while (*str) {
		UART_transmit(*str++);
	}
	UART_transmit('\r'); // Retorno de carro
	UART_transmit('\n'); // Nueva línea
}

// Receive a string between '(' and ')'
void UART_receive_delimited_string(void) {
	char received_char;
	rx_index = 0;
	receiving = 0;

	while (1) {
		received_char = UART_receive();

		if (received_char == '(') {
			receiving = 1;
			rx_index = 0;
		}
		else if (received_char == ')' && receiving) {
			rx_buffer[rx_index] = '\0'; // Null-terminate the string
			UART_transmit('\r'); // Envía "enter" al recibir ')'
			UART_transmit('\n');
			break;
		}
		else if (receiving && rx_index < BUFFER_SIZE - 1) {
			rx_buffer[rx_index++] = received_char;
		}
	}
}

uint8_t check_data_ranges(int data1, int data2, int data3, int data4) {
	flag_error = 0;
	
	if (data1 < 1 || data1 >= 2) {
		UART_transmit_string("Error: Ingresar dirección de motor existente.");
		flag_error = 1;
	}

	// Ranges for data3 and data4 depending on data2 value
	if(data2 == 1){
		if (data3 < 0 || data3 > 3)
		{
			UART_transmit_string("Error: El microstepping esta fuera de rango (0-3).");
			flag_error = 1;
		}
	}
	if (data2 == 4)
	{
		if (data3 < 0 || data3 > 255)
		{
			UART_transmit_string("Error: Rango de RPM incorrecto.");
			flag_error = 1;
		}
		if (data4 < 0 || data4 > 10)
		{
			UART_transmit_string("Error: Rango de tiempo incorrecto.");
			flag_error = 1;
		}
	}
	if (data2 == 6)
	{
		if (data3 < 0 || data3 > 255)
		{
			UART_transmit_string("Error: Rango de RPM incorrecto.");
			flag_error = 1;
		}
		if (data4 < -180 || data4 > 180)
		{
			UART_transmit_string("Error: Rango de ángulo incorrecto.");
			flag_error = 1;
		}
	}
	return flag_error;
}