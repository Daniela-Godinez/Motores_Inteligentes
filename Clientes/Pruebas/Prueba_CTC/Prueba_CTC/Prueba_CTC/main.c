/*
 * Prueba_CTC.c
 *
 * Created: 26/06/2024 17:48:28
 * Author : Daniela Godinez
 */

#define F_CPU 16000000 // Oscilador de 16MHz

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/sfr_defs.h>
#include <stdbool.h>

// DEFINICION DE MACROS
#define set_bit(sfr,bit)	(_SFR_BYTE(sfr) |= _BV(bit))	// Macro para setear el bit del puerto (0 logico)
#define clear_bit(sfr,bit)	(_SFR_BYTE(sfr) &= ~_BV(bit))	// Macro para resetear el bit del puerto

// MACROS INTERNAS DE LIBRERIA <avr/sfr_defs.h> -- SOLO Lectura de bits
// #define bit_is_set(sfr, bit) (_SFR_BYTE(sfr) & _BV(bit))
// #define bit_is_clear(sfr, bit) (!(_SFR_BYTE(sfr) & _BV(bit)))

volatile uint16_t rpm_f = 20;
volatile uint16_t rpm_i = 10;
volatile uint16_t maxval;
volatile uint16_t minval;
volatile uint16_t timpo_s = 5;

volatile uint16_t tiempo_m;

volatile int16_t frecuencia_i;
volatile int16_t frecuencia_f;

volatile int16_t rango_aceleracion;

int main(void)
{
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
	
	set_bit(DDRD,0);
	
	clear_bit(PORTB, 0); // MS1
	clear_bit(PORTD, 7); // MS2
	clear_bit(PORTD, 6); // MS3
	
	set_bit(PORTD, 5);
	set_bit(PORTD, 4);
	set_bit(PORTD, 3);
	
	//-----------------------------
	//       MODO CTC - 0100
	//-----------------------------
	// (TMR1 - MODO 4 - MAXVAL OCR1A)
	clear_bit(TCCR1B, WGM13);
	set_bit(TCCR1B, WGM12);
	clear_bit(TCCR1A, WGM11);
	clear_bit(TCCR1A, WGM10);
	
	//-----------------------------
	//     PRESCALER - 8 (010)
	//-----------------------------
	clear_bit(TCCR1B, CS12);
	set_bit(TCCR1B, CS11);
	clear_bit(TCCR1B, CS10);
	
	//-----------------------------
	//      CONMUTA OC1x (01)
	//-----------------------------
	clear_bit(TCCR1A,COM1A1);
	set_bit(TCCR1A, COM1A0);
	
	//-----------------------------
	//  INTERRUPCION TMR1 - MASK
	//-----------------------------
	TIMSK1 |= (1<<OCIE1A);
	
	
	
	tiempo_m = timpo_s/60;
	
	frecuencia_i = (rpm_i*200)/60;
	frecuencia_f = (rpm_f*200)/60;
	
	maxval = (F_CPU/16/frecuencia_i)-1;
	minval = (F_CPU/16/frecuencia_f)-1;
	
	//rango_aceleracion = (maxval-minval)/(tiempo_m*(rpm_f-rpm_i));
	rango_aceleracion = ((maxval-minval)*rpm_f)/(tiempo_m*(rpm_f-rpm_i));

	OCR1A = maxval;	
	sei();
    /* Replace with your application code */
    while (1) 
    {
    }
}

ISR(TIMER1_COMPA_vect){
	if (maxval > minval)
	{
		maxval -= 45;
		if (maxval < minval) {
			maxval=minval;
			set_bit(PORTD,0);
		}
	}
	OCR1A = maxval;
}

