/*
 * A4988.c
 *
 * Created: 17/06/2024 21:56:14
 *  Author: Daniela Godinez
 */ 


#include "A4988.h"

//---------------------------------
//    FUNCIONES DE CONFIGURACION
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
	//       FAST PWM - 1100
	//-----------------------------
	// (TMR1 - MODO 14 - MAXVAL ICR1)
	TCCR1B |= (1<<WGM13);
	TCCR1B |= (1<<WGM12);
	TCCR1A |= (1<<WGM11);
	TCCR1A &= ~(1<<WGM10);
	
	//-----------------------------
	//       PWM NO INVERTIDA
	//-----------------------------
	TCCR1A |= (1<<COM1A1);
	TCCR1A &= ~(1<<COM1A0);
	
	//-----------------------------
	//  INTERRUPCION TMR1 - MASK 
	//-----------------------------
	TIMSK1 |= (1<<OCIE1A);
}

void init_motor(enum Microstep_t tipo, enum Direccion_t giro, uint16_t frecuencia, uint8_t duty){
	
	//-----------------------------
	//    PRESCALER - 1024 (101)
	//-----------------------------
	TCCR1B |= (1<<CS12);
	TCCR1B &= ~(1<<CS11);
	TCCR1B |= (1<<CS10);
	
	config_microstepping(tipo);
	config_direccion(giro);
	_frecuencia = frecuencia;
	_ciclo_trabajo = duty;
	movimiento = 1;
}

void config_direccion(enum Direccion_t sentido){
	switch(sentido){
		case HORARIO:
			clear_bit(PORTD, 3); // DIR
			break;
		case ANTIHORARIO:
			set_bit(PORTD, 3); // DIR
		break;			
	}
}

void config_microstepping(enum Microstep_t micro){
	switch(micro){
		case FULL_STEP:
			clear_bit(PORTB, 0); // MS1 - 0
			clear_bit(PORTD, 7); // MS2 - 0
			clear_bit(PORTD, 6); // MS3 - 0
		break;
		case HALF_STEP:
			set_bit(PORTB, 0);   // MS1 - 1
			clear_bit(PORTD, 7); // MS2 - 0
			clear_bit(PORTD, 6); // MS3 - 0
		break;
		case QUATER_STEP:
			clear_bit(PORTB, 0); // MS1 - 0
			set_bit(PORTD, 7);   // MS2 - 1
			clear_bit(PORTD, 6); // MS3 - 0
		break;
		case EIGHTH_STEP:
			set_bit(PORTB, 0);   // MS1 - 1
			set_bit(PORTD, 7);   // MS2 - 1
			clear_bit(PORTD, 6); // MS3 - 0
		break;
		case SIXTEENTH_STEP:
			set_bit(PORTB, 0);   // MS1 - 1
			set_bit(PORTD, 7);   // MS2 - 1
			set_bit(PORTD, 6);   // MS3 - 1
		break;
	}
}

void stop_motor(void){
	//-----------------------------
	//    PRESCALER - 1024 (101)
	//-----------------------------
	TCCR1B &= ~(1<<CS12);
	TCCR1B &= ~(1<<CS11);
	TCCR1B &= ~(1<<CS10);
	
	TCNT1 = 0;
	
	movimiento = 0;
}

void incrementar_velocidad(enum Microstep_t tipo_, enum Direccion_t giro_, uint16_t frecuencia_i, uint16_t frecuencia_f,uint8_t duty){
	//-----------------------------
	//    PRESCALER - 1024 (101)
	//-----------------------------
	TCCR1B |= (1<<CS12);
	TCCR1B &= ~(1<<CS11);
	TCCR1B |= (1<<CS10);
	
	config_microstepping(tipo_);
	config_direccion(giro_);
	_frecuencia_inicial = frecuencia_i;
	_frecuencia_final = frecuencia_f;
	_ciclo_trabajo = duty;
	
	movimiento = 2;
}

void decrementar_velocidad(enum Microstep_t tipo__, enum Direccion_t giro__, uint16_t frecuencia_o, uint16_t frecuencia_l, uint8_t duty){
	//-----------------------------
	//    PRESCALER - 1024 (101)
	//-----------------------------
	TCCR1B |= (1<<CS12);
	TCCR1B &= ~(1<<CS11);
	TCCR1B |= (1<<CS10);
	
	config_microstepping(tipo__);
	config_direccion(giro__);
	_frecuencia_inicial = frecuencia_o;
	_frecuencia_final = frecuencia_l;
	_ciclo_trabajo = duty;
	
	movimiento = 3;
}


ISR(TIMER1_COMPA_vect){
	// VARIABLES
	uint16_t maxval;
	
	if (movimiento == 1)
	{
		maxval = (F_CPU/1024/_frecuencia)-1;
		ICR1 = maxval;
		OCR1A = (ICR1*_ciclo_trabajo)/100;
	}else if(movimiento == 2){
		uint16_t maxval_i = (F_CPU/1024/_frecuencia_inicial)-1;
		uint16_t maxval_f = (F_CPU/1024/_frecuencia_final)-1;
		
		while(true){
			if (maxval_i>maxval_f)
			{
				ICR1 = maxval_i;
				OCR1A = (ICR1*_ciclo_trabajo)/100;
				maxval_i--;
				_delay_ms(600);
			}
		}
		
	}else if (movimiento==3){
		uint16_t maxval_i = (F_CPU/1024/_frecuencia_inicial)-1;
		uint16_t maxval_f = (F_CPU/1024/_frecuencia_final)-1;
		
		while(true){
			if (maxval_i<maxval_f)
			{
				ICR1 = maxval_i;
				OCR1A = (ICR1*_ciclo_trabajo)/100;
				maxval_i++;
				_delay_ms(600);
			}
		}
	}
	
}
