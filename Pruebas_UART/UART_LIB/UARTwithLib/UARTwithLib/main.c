/*
 * UARTwithLib.c
 *
 * Created: 14/11/2024 19:49:32
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
#include <string.h>

#include "UART.h"

char rx_buffer[50];

int main(void) {
	UART_init(103); // Set UART baud rate to 9600
	int data1, data2, data3, data4;
	while (1) {
		UART_receive_delimited_string(); // Receive data in format "(data1,data2,data3,data4)"
		sscanf(rx_buffer, "%d,%d,%d,%d", &data1, &data2, &data3, &data4);
		
		uint8_t commandcorrect = check_data_ranges(data1, data2, data3, data4);
		
		if (!commandcorrect)
		{
			UART_transmit_string("El comando será ejecutado.");
		}
		if(commandcorrect){
			UART_transmit_string("Ingresar el comando de nuevo.");
		}
	}
}


