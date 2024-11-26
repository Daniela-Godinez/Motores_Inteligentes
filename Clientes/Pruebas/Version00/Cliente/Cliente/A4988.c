/*
 * A4988.c
 *
 * Created: 03/07/2024 17:56:27
 *  Author: Daniela Godinez
 */ 

#include "A4988.h"

//---------------------------------
//     FUNCION DE CONFIGURACION
//---------------------------------
void config_A4988(){
	//-----------------------------
	//           SALIDAS
	//-----------------------------
	set_bit(DDRB, 1); // STEP - PWM
	set_bit(DDRB, 0); // MS1
	set_bit(DDRD, 7); // MS2
	set_bit(DDRD, 6); // MS3
	set_bit(DDRD, 5); // RST
	set_bit(DDRD, 4); // SLP
	set_bit(DDRD, 3); // DIR
	
	//-----------------------------
	//        SALIDAS EN ALTO
	//-----------------------------
	set_bit(PORTD, 5); // RST
	set_bit(PORTD, 4); // SLP
	
	//-----------------------------
	//       MODO CTC - 0100
	//-----------------------------
	// (TMR1 - MODO 4 - MAXVAL OCR1A)
	clear_bit(TCCR1B, WGM13);
	set_bit(TCCR1B, WGM12);
	clear_bit(TCCR1A, WGM11);
	clear_bit(TCCR1A, WGM10);
	
	//-----------------------------
	//      CONMUTA OC1x (01)
	//-----------------------------
	clear_bit(TCCR1A,COM1A1);
	set_bit(TCCR1A, COM1A0);
	
	//-----------------------------
	//  INTERRUPCION TMR1 - MASK
	//-----------------------------
	TIMSK1 |= (1<<OCIE1A);
	
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
	frecuencia = (rpm*tipo_step)/60;
	contador = (F_CPU/16/frecuencia)-1;
	return contador;
}

//---------------------------------
//     FUNCION DE USUARIO
//---------------------------------
void Microstepping(enum Microstep_t tipo){
	switch(tipo){
		case FULL_STEP:
		clear_bit(PORTB, 0); // MS1 - 0
		clear_bit(PORTD, 7); // MS2 - 0
		clear_bit(PORTD, 6); // MS3 - 0
		tipo_step = 200;
		break;
		case HALF_STEP:
		set_bit(PORTB, 0);   // MS1 - 1
		clear_bit(PORTD, 7); // MS2 - 0
		clear_bit(PORTD, 6); // MS3 - 0
		tipo_step = 400;
		break;
		case QUATER_STEP:
		clear_bit(PORTB, 0); // MS1 - 0
		set_bit(PORTD, 7);   // MS2 - 1
		clear_bit(PORTD, 6); // MS3 - 0
		tipo_step = 800;
		break;
		case EIGHTH_STEP:
		set_bit(PORTB, 0);   // MS1 - 1
		set_bit(PORTD, 7);   // MS2 - 1
		clear_bit(PORTD, 6); // MS3 - 0
		tipo_step = 1600;
		break;
		case SIXTEENTH_STEP:
		set_bit(PORTB, 0);   // MS1 - 1
		set_bit(PORTD, 7);   // MS2 - 1
		set_bit(PORTD, 6);   // MS3 - 1
		tipo_step = 3200;
		break;
	}	
}
	
void Horario(){
	clear_bit(PORTD, 3); // DIR
}

void Antihorario(){
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
	
	tipo_movimiento = 0;
}

void init_motor(uint16_t RPM){
	prescaler_activated();
	OCR1A = RPM2Counter(RPM);
}

void ramp_up(uint16_t RPM_i, uint16_t RPM_f, uint16_t tiempo){
	prescaler_activated();
	ru_maxval = RPM2Counter(RPM_f);
	ru_minval = RPM2Counter(RPM_i);
	uint16_t vf = (RPM_f*tipo_step)/60;
	ru_tasa = (ru_minval-ru_maxval)/(vf*tiempo);
	OCR1A = ru_minval;
	tipo_movimiento = 1;
}

void ramp_down(uint16_t RPM_i, uint16_t RPM_f, uint16_t tiempo){
	prescaler_activated();
	rd_maxval = RPM2Counter(RPM_f);
	rd_minval = RPM2Counter(RPM_i);
	uint16_t vf = (RPM_i*tipo_step)/60;
	rd_tasa = (rd_maxval-rd_minval)/(vf*tiempo);
	OCR1A = rd_minval;
	tipo_movimiento = 2;
}

void angle(uint16_t RPM, uint16_t angle){
	prescaler_activated();
	OCR1A = RPM2Counter(RPM);
}

//---------------------------------
//     FUNCION DE INTERRUPCION
//---------------------------------
ISR(TIMER1_COMPA_vect){
	if (tipo_movimiento==1)
	{
		if (ru_minval>ru_maxval)
		{
			ru_minval -= ru_tasa;
			if (ru_minval<ru_maxval)
			{
				ru_minval = ru_maxval;
			}
		}
		OCR1A = ru_minval;
	}else if (tipo_movimiento==2)
	{
		if (rd_minval<rd_maxval)
		{
			rd_minval += rd_tasa;
			if (rd_minval>rd_maxval)
			{
				rd_minval = rd_maxval;
			}
		}
		OCR1A = rd_minval;
	}
}
