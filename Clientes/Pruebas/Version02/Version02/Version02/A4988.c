/*
 * A4988.c
 *
 * Created: 06/10/2024 20:52:09
 *  Author: Daniela Godinez
 */ 

#include "A4988.h"
#include "ADC.h"

uint16_t previous_rpm = 6;
volatile uint16_t target_counter = 0;
volatile uint16_t ramping_activate = 0;
uint16_t step_count = 0;

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
	//previous_rpm = 0;
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
void Microstepping(enum Microstep_t _type){
	switch(_type){
		case FULL_STEP:
		clear_bit(PORTB, 0); // MS1 - 0
		clear_bit(PORTD, 7); // MS2 - 0
		clear_bit(PORTD, 6); // MS3 - 0
		step_factor = 200;
		microstep_factor = 1;
		break;
		case HALF_STEP:
		set_bit(PORTB, 0);   // MS1 - 1
		clear_bit(PORTD, 7); // MS2 - 0
		clear_bit(PORTD, 6); // MS3 - 0
		step_factor = 400;
		microstep_factor = 2;
		break;
		case QUATER_STEP:
		clear_bit(PORTB, 0); // MS1 - 0
		set_bit(PORTD, 7);   // MS2 - 1
		clear_bit(PORTD, 6); // MS3 - 0
		step_factor = 800;
		microstep_factor = 4;
		break;
		case EIGHTH_STEP:
		set_bit(PORTB, 0);   // MS1 - 1
		set_bit(PORTD, 7);   // MS2 - 1
		clear_bit(PORTD, 6); // MS3 - 0
		step_factor = 1600;
		microstep_factor = 8;
		break;
		case SIXTEENTH_STEP:
		set_bit(PORTB, 0);   // MS1 - 1
		set_bit(PORTD, 7);   // MS2 - 1
		set_bit(PORTD, 6);   // MS3 - 1
		step_factor = 3200;
		microstep_factor = 16;
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
	//previous_rpm = 6;
}

void set_motor(uint16_t next_rpm, uint16_t ramp_time){
	
	while(ramping_activate){
		//return;
	}
	
	prescaler_activated();
	uint16_t frequency_f;
	max_counter = RPM2Counter(next_rpm);
	min_counter = RPM2Counter(previous_rpm);
	
	if (previous_rpm < next_rpm)
	{
		_nextrpm = next_rpm;
		frequency_f = (next_rpm*step_factor)/60;
		delta_counter = (min_counter-max_counter)/(frequency_f*ramp_time);
		target_counter = max_counter;
		OCR1A = min_counter;
		type_movement = 1;
	}else{
		_nextrpm = next_rpm;
		frequency_f = (previous_rpm*step_factor)/60;
		delta_counter = (max_counter-min_counter)/(frequency_f*ramp_time);
		target_counter = max_counter;
		OCR1A = min_counter;
		type_movement = 2;
	}
	ramping_activate = 1;
}

uint16_t move2angle(uint16_t RPM, uint16_t angle){
	uint16_t previous_angle = read_angle();
	_delay_ms(1000);
	int16_t angle_difference = abs(angle - previous_angle);

	if (angle > previous_angle)
	{
		Clockwise();
	}else if (angle < previous_angle)
	{
		Anticlockwise();
	}
	
	float steps_per_degree = (float)(microstep_factor)/motor_step_angle;
	steps_needed = (uint16_t)(angle_difference*steps_per_degree)*2;
	
	step_count = 0;
	step_flag = 1;
	
	
	OCR1A = RPM2Counter(RPM);
	prescaler_activated();
	type_movement = 3;

	return angle_difference;

}

uint16_t read_speed(void){
	uint16_t current_counter = OCR1A;
	uint16_t frequency = F_CPU / (16 * (current_counter + 1));
	uint16_t rpm = (frequency * 60) / step_factor;
	return rpm;
}


void step(uint16_t RPM){
	OCR1A = RPM2Counter(RPM);
	prescaler_activated();
}

//---------------------------------
//			INT. FUNCTIONS
//---------------------------------
ISR(TIMER1_COMPA_vect){
	if (ramping_activate)
	{
		if (type_movement==1)
		{
			if (min_counter>target_counter)
			{
				min_counter -= delta_counter;
				if (min_counter<target_counter)
				{
					min_counter = target_counter;
					previous_rpm = _nextrpm;
					ramping_activate = 0;
				}
			}
			OCR1A = min_counter;
		}else if (type_movement==2)
		{
			if (min_counter<target_counter)
			{
				min_counter += delta_counter;
				if (min_counter>target_counter)
				{
					min_counter = target_counter;
					previous_rpm = _nextrpm;
					ramping_activate = 0;
				}
			}
			OCR1A = min_counter;
		}
	}
	if (type_movement == 3 && step_flag) {
		step_count++;
		if (step_count >= steps_needed) {
			step_flag = 0; // Disable stepping to prevent further increments
			stop_motor(); // Stop motor within ISR once target steps are reached
		}
	}
}
