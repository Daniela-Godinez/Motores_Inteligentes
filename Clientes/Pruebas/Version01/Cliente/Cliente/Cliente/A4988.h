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
	//	     GLOBAL VARIABLES
	//-----------------------------
	uint16_t frequency;			// Frequency calculated in RPM2Counter function
	uint16_t step_factor;		// Using to calculate the frequency RPM2Counter function
	uint16_t counter;			// Counter calculated in RPM2Counter function
	
	uint16_t previous_rpm;		// To save the value of the previous rpm, set in 0
	
	uint16_t max_counter;		// To save the maximum value of the counter register
	uint16_t min_counter;		// To save the minimum value of the counter register
	uint16_t delta_counter;		// To save the delta value from the difference between max and min values
	
	uint8_t type_movement;		// Indicates the type of movement of the motor. Start (ramp up and ramp down) and stop
	
	//-----------------------------
	//        ENUMERATIONS
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
	//      SETTING FUNCTION
	//-----------------------------
	void init_A4988(void);
	void prescaler_activated(void);
	uint16_t RPM2Counter(uint16_t _RPM);
	
	//-----------------------------
	//      USER's FUNCTIONS
	//-----------------------------
	void Microstepping(enum Microstep_t tipo);
	void Clockwise(void);
	void Anticlockwise(void);
	void stop_motor(void);
	void set_motor(uint16_t next_rpm, uint16_t ramp_time);
	void move2angle(uint16_t RPM, uint16_t angle);

#endif /* A4988_H_ */