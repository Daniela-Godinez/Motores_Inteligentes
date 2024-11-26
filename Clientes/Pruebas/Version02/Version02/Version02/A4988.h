/*
 * A4988.h
 *
 * Created: 06/10/2024 20:52:24
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
	
	#define motor_step_angle 1.8
	
	//-----------------------------
	//	     GLOBAL VARIABLES
	//-----------------------------
	uint16_t frequency;					// Frequency calculated in RPM2Counter function
	uint16_t step_factor;				// Using to calculate the frequency RPM2Counter function
	uint16_t microstep_factor;
	uint16_t step_count;
	uint16_t counter;					// Counter calculated in RPM2Counter function
	uint8_t type_movement;				// Indicates the type of movement of the motor. Start (ramp up and ramp down) and stop
	volatile uint16_t target_counter;	// Flag to indicate that the target counter has been reached
	volatile uint16_t ramping_activate; // Flag to indicate ramping is in progress
	extern uint16_t previous_rpm;		// To save the value of the previous rpm, set in 0
	uint16_t _nextrpm;					// To save the value of the previous rpm, set in 0
	uint16_t max_counter;				// To save the maximum value of the counter register
	uint16_t min_counter;				// To save the minimum value of the counter register
	uint16_t delta_counter;				// To save the delta value from the difference between max and min values
	int step_flag;
	uint16_t steps_needed;

	//-----------------------------
	//        ENUMERATIONS
	//-----------------------------
	typedef enum Microstep_t{
		FULL_STEP,						// 1 microstepping, the motor driver will move in 1 smaller steps
		HALF_STEP,						// 1/2 microstepping, the motor driver will move in 2 smaller steps
		QUATER_STEP,					// 1/4 microstepping, the motor driver will move in 4 smaller steps
		EIGHTH_STEP,					// 1/8 microstepping, the motor driver will move in 8 smaller steps
		SIXTEENTH_STEP					// 1/16 microstepping, the motor driver will move in 16 smaller steps
	}Microstep_t;

	//-----------------------------
	//      SETTING FUNCTION
	//-----------------------------
	void init_A4988(void);					// Configures and activates the driver A4988
	void prescaler_activated(void);			// Configures and activates the prescaler for the timer 1
	uint16_t RPM2Counter(uint16_t _RPM);	// Converts the given RPM value to a timer counter value

	//-----------------------------
	//      USER's FUNCTIONS
	//-----------------------------
	void Microstepping(enum Microstep_t _type);				// Set the microstepping mode for the motor driver
	void Clockwise(void);									// Motor will rotate clockwise (CW)
	void Anticlockwise(void);								// Motor will rotate anticlockwise (CCW)
	void stop_motor(void);									// Stops the movement of the motor
	void set_motor(uint16_t next_rpm, uint16_t ramp_time);	// Set the speed and the ramping acceleration
	uint16_t move2angle(uint16_t RPM, uint16_t angle);			// Specifies the target angle for the motor to rotate
	uint16_t read_speed(void);
	void step(uint16_t RPM);

#endif /* A4988_H_ */