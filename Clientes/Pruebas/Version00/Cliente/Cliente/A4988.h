/*
 * A4988.h
 *
 * Created: 03/07/2024 17:56:17
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
	uint8_t tipo_movimiento;
	uint16_t tipo_step;
	uint16_t frecuencia;
	uint16_t contador;
	uint16_t init_RPM;
	uint16_t ru_maxval;
	uint16_t ru_minval;
	uint16_t ru_tasa;
	uint16_t rd_maxval;
	uint16_t rd_minval;
	uint16_t rd_tasa;
	
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
	//  FUNCION DE CONFIGURACION
	//-----------------------------
	void config_A4988(void);
	void prescaler_activated(void);
	uint16_t RPM2Counter(uint16_t _RPM);
	
	//-----------------------------
	//    FUNCIONES DE USUARIO
	//-----------------------------
	void Microstepping(enum Microstep_t tipo);
	void Horario(void);
	void Antihorario(void);
	void stop_motor(void);
	void init_motor(uint16_t RPM);
	void ramp_up(uint16_t RPM_i, uint16_t RPM_f, uint16_t tiempo);
	void ramp_down(uint16_t RPM_i, uint16_t RPM_f, uint16_t tiempo);
	void angle(uint16_t RPM, uint16_t angle);

#endif /* A4988_H_ */