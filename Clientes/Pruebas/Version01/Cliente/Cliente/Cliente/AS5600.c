/*
 * AS5600.c
 *
 * Created: 05/09/2024 20:42:55
 *  Author: Daniela Godinez
 */ 

#include "AS5600.h"

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
}

uint16_t ADC_read(uint8_t canal){
	canal &= 0b00001111;
	ADMUX = (ADMUX & 0xF0)|canal;
	set_bit(ADCSRA, ADSC);
	while((ADCSRA)&(1<<ADSC));
	return(ADC);
}

uint16_t ADC2angle(void){
	uint16_t _ADC = ADC_read(0);
	return((_ADC*360.0)/1023.0);
}

//-----------------------------
//      USER's FUNCTIONS
//-----------------------------
uint16_t set_origin(void){
	uint16_t new_origin = ADC2angle();
	return(new_origin);
}

uint16_t read_angle(void){
	if (new_zero == 0)
	{
		uint16_t angle = ADC2angle();
		return(angle);
		}else{
		uint16_t actual_angle = ADC2angle();
		if (actual_angle < new_zero)
		{
			uint16_t new_angle = actual_angle - new_zero;
			uint16_t angle = 360 + new_angle;
			return(angle);
			}else if(actual_angle == new_zero){
			uint16_t angle = 0;
			return(angle);
			}else if(actual_angle > new_zero){
			uint16_t new_angle = actual_angle - new_zero;
			uint16_t angle = new_angle;
			return(angle);
		}
	}
}

uint16_t read_current(void){
	uint16_t current = ADC_read(1);
	return((current*5.0)/1023.0);
}