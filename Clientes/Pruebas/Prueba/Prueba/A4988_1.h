/*
 * A4988.h
 *
 * Created: 17/06/2024 21:55:28
 *  Author: Daniela Godinez
 */ 


#ifndef A4988_H_
#define A4988_H_

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
	//     VARIABLES GLOBALES
	//-----------------------------
	uint16_t _frecuencia;
	uint16_t _frecuencia_inicial;
	uint16_t _frecuencia_final;
	uint8_t _ciclo_trabajo;
	uint8_t movimiento;
	
	//-----------------------------
	//        ENUMERACIONES
	//-----------------------------
	
	typedef enum Microstep_t{
		FULL_STEP,
		HALF_STEP,
		QUATER_STEP,
		EIGHTH_STEP,
		SIXTEENTH_STEP
		}Microstep_t;
		
	typedef enum Direccion_t{
		HORARIO,
		ANTIHORARIO
	}Direccion_t;
	
	//-----------------------------
	// FUNCIONES DE CONFIGURACION
	//-----------------------------
		
	void config_A4988(void);
	void config_direccion(enum Direccion_t sentido);
	void config_microstepping(enum Microstep_t micro);
	
	//-----------------------------
	//    FUNCIONES PARA USUARIO
	//-----------------------------
	void init_motor(enum Microstep_t tipo, enum Direccion_t giro, uint16_t frecuencia, uint8_t duty);
	void stop_motor(void);
	void incrementar_velocidad(enum Microstep_t tipo_, enum Direccion_t giro_, uint16_t frecuencia_i, uint16_t frecuencia_f, uint8_t duty);
	void decrementar_velocidad(enum Microstep_t tipo__, enum Direccion_t giro__, uint16_t frecuencia_o, uint16_t frecuencia_l, uint8_t duty);

#endif /* A4988_H_ */