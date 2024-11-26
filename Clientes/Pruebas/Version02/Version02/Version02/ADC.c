/*
 * ADC.c
 *
 * Created: 25/10/2024 22:14:06
 *  Author: Daniela Godinez
 */ 

#include "ADC.h"

uint16_t new_zero = 0;

//---------------------------------
//	       SETTING FUNCTION
//---------------------------------
void init_ADC(){
	// VOLTAGE REFERENCE - AVcc (01)
	clear_bit(ADMUX, REFS1);
	set_bit(ADMUX, REFS0);
	// ADC RIGTH ADJUST RESULT
	clear_bit(ADMUX, ADLAR);
	// PRESCALER SELECT (128 = 111)
	set_bit(ADCSRA, ADPS0);
	set_bit(ADCSRA, ADPS1);
	set_bit(ADCSRA, ADPS2);
	// ADC ENABLE
	set_bit(ADCSRA, ADEN);
	// ENCODER DIRECTION
	set_bit(DDRD, 2);
	set_bit(PORTD, 2);
}

uint16_t ADC_read(uint8_t canal){
	canal &= channel_mask;
	ADMUX = (ADMUX & channel_shift_mask)|canal;
	set_bit(ADCSRA, ADSC);
	while((ADCSRA)&(1<<ADSC));
	return(ADC);
}

uint16_t ADC2angle(void){
	uint16_t _ADC = ADC_read(0);
	return((_ADC*full_circle_degree)/adc_resolution);
}

//-----------------------------
//      USER's FUNCTIONS
//-----------------------------
uint16_t set_origin(void){
	return ADC2angle();
}

uint16_t read_angle(void){
	uint16_t actual_angle = ADC2angle();

	if (new_zero == 0) {
		return actual_angle;
	} else {
		// Calculate adjusted angle relative to new_zero
		int16_t adjusted_angle = actual_angle - new_zero;

		// Normalize angle within 0 to 360 degrees
		if (adjusted_angle < 0) {
			adjusted_angle += full_circle_degree;
		}
		return (uint16_t)adjusted_angle;
	}
}

uint16_t read_current(void){
	uint16_t current = ADC_read(1);
	return((current*max_voltage)/adc_resolution);
}