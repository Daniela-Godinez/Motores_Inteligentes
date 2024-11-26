/*
 * A4988.c
 *
 * Created: 03/07/2024 17:56:27
 *  Author: Daniela Godinez
 */ 

#include "A4988.h"
#include "AS5600.h"

//---------------------------------
//	       SETTING FUNCTION
//---------------------------------
void init_A4988(){
	//-----------------------------
	//           OUTPUTS
	//-----------------------------
	set_bit(DDRB, 1); // STEP - PWM
	set_bit(DDRB, 0); // MS1
	set_bit(DDRD, 7); // MS2
	set_bit(DDRD, 6); // MS3
	set_bit(DDRD, 5); // RST
	set_bit(DDRD, 4); // SLP
	set_bit(DDRD, 3); // DIR
	
	//-----------------------------
	//        HIGH OUTPUTS
	//-----------------------------
	set_bit(PORTD, 5); // RST
	set_bit(PORTD, 4); // SLP
	
	//-----------------------------
	//      CTC MODE - 0100
	//-----------------------------
	// (TMR1 - MODE 4 - MAXVAL OCR1A)
	clear_bit(TCCR1B, WGM13);
	set_bit(TCCR1B, WGM12);
	clear_bit(TCCR1A, WGM11);
	clear_bit(TCCR1A, WGM10);
	
	//-----------------------------
	//      TOGGLE OC1x (01)
	//-----------------------------
	clear_bit(TCCR1A,COM1A1);
	set_bit(TCCR1A, COM1A0);
	
	//-----------------------------
	//   TMR1 INTERRUPT - MASK
	//-----------------------------
	TIMSK1 |= (1<<OCIE1A);
	
	//-----------------------------
	//    VARIABLES ASSIGNED 0
	//-----------------------------
	previous_rpm = 0;
}

void prescaler_activated(){
	//-----------------------------
	//     PRESCALER - 8 (010)
	//-----------------------------
	clear_bit(TCCR1B, CS12);
	set_bit(TCCR1B, CS11);
	clear_bit(TCCR1B, CS10);
}

uint16_t RPM2Counter(uint16_t rpm){
	frequency = (rpm*step_factor)/60;
	counter = (F_CPU/16/frequency)-1;
	return counter;
}

//---------------------------------
//		   USER's FUNCTIONS
//---------------------------------
void Microstepping(enum Microstep_t tipo){
	switch(tipo){
		case FULL_STEP:
		clear_bit(PORTB, 0); // MS1 - 0
		clear_bit(PORTD, 7); // MS2 - 0
		clear_bit(PORTD, 6); // MS3 - 0
		step_factor = 200;
		break;
		case HALF_STEP:
		set_bit(PORTB, 0);   // MS1 - 1
		clear_bit(PORTD, 7); // MS2 - 0
		clear_bit(PORTD, 6); // MS3 - 0
		step_factor = 400;
		break;
		case QUATER_STEP:
		clear_bit(PORTB, 0); // MS1 - 0
		set_bit(PORTD, 7);   // MS2 - 1
		clear_bit(PORTD, 6); // MS3 - 0
		step_factor = 800;
		break;
		case EIGHTH_STEP:
		set_bit(PORTB, 0);   // MS1 - 1
		set_bit(PORTD, 7);   // MS2 - 1
		clear_bit(PORTD, 6); // MS3 - 0
		step_factor = 1600;
		break;
		case SIXTEENTH_STEP:
		set_bit(PORTB, 0);   // MS1 - 1
		set_bit(PORTD, 7);   // MS2 - 1
		set_bit(PORTD, 6);   // MS3 - 1
		step_factor = 3200;
		break;
	}	
}
	
void Clockwise(){
	clear_bit(PORTD, 3); // DIR
}

void Anticlockwise(){
	set_bit(PORTD, 3); // DIR
}

void stop_motor(){
	//-----------------------------
	//    PRESCALER - 1024 (101)
	//-----------------------------
	TCCR1B &= ~(1<<CS12);
	TCCR1B &= ~(1<<CS11);
	TCCR1B &= ~(1<<CS10);
	
	TCNT1 = 0;
	
	type_movement = 0;
}

void set_motor(uint16_t next_rpm, uint16_t ramp_time){
	uint16_t frequency_f;
	
	prescaler_activated();
	
	max_counter = RPM2Counter(next_rpm);
	min_counter = RPM2Counter(previous_rpm);
	
	if (previous_rpm < next_rpm)
	{
		frequency_f = (next_rpm*step_factor)/60;
		delta_counter = (min_counter-max_counter)/(frequency_f*ramp_time);
		OCR1A = min_counter;
		type_movement = 1;
	}else{
		frequency_f = (previous_rpm*step_factor)/60;
		delta_counter = (max_counter-min_counter)/(frequency_f*ramp_time);
		OCR1A = min_counter;
		type_movement = 2;
	}
	
	previous_rpm = next_rpm;
}

//void ramp_up(uint16_t RPM_i, uint16_t RPM_f, uint16_t tiempo){
	//prescaler_activated();
	//ru_maxval = RPM2Counter(RPM_f);
	//ru_minval = RPM2Counter(RPM_i);
	//uint16_t vf = (RPM_f*step_factor)/60;
	//ru_tasa = (ru_minval-ru_maxval)/(vf*tiempo);
	//OCR1A = ru_minval;
	//type_movement = 1;
//}
//
//void ramp_down(uint16_t RPM_i, uint16_t RPM_f, uint16_t tiempo){
	//prescaler_activated();
	//rd_maxval = RPM2Counter(RPM_f);
	//rd_minval = RPM2Counter(RPM_i);
	//uint16_t vf = (RPM_i*step_factor)/60;
	//rd_tasa = (rd_maxval-rd_minval)/(vf*tiempo);
	//OCR1A = rd_minval;
	//type_movement = 2;
//}

void move2angle(uint16_t RPM, uint16_t angle){
	uint16_t degrees = ADC2angle();
	while (degrees != angle)
	{
		prescaler_activated();
		OCR1A = RPM2Counter(RPM);
	}
	stop_motor();
}

//---------------------------------
//			INT. FUNCTIONS
//---------------------------------
ISR(TIMER1_COMPA_vect){
	if (type_movement==1)
	{
		if (min_counter>max_counter)
		{
			min_counter -= delta_counter;
			if (min_counter<max_counter)
			{
				min_counter = max_counter;
			}
		}
		OCR1A = min_counter;
	}else if (type_movement==2)
	{
		if (min_counter<max_counter)
		{
			min_counter += delta_counter;
			if (min_counter>max_counter)
			{
				min_counter = max_counter;
			}
		}
		OCR1A = min_counter;
	}
}
