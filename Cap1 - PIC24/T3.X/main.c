/*
 * File:   main.c
 * Author: fredericovaz
 * 
 * Tarefa 2
 * 
 *
 * Created on October 21, 2015, 9:45 PM
 */


#include "xc.h"
#include "config.h" 
#include <libpic30.h>



int main() {

	CLKDIVbits.DOZE = 0; 	// 1:1  
	CLKDIVbits.RCDIV = 0;	// 8MHz

	ANSB = 0x0000; 				// Configura o PORTB para Digital
	TRISB = 0x0800;    		// PORTB como saida, menos a RB11 que sera a entrada do botao

	int freq = 200; 		// Comutacao de 0,2s

	do{ // e correcto colocar as outras entradas a 0? Que nao sao usadas ou nao fazer nada?

		if(PORTBbits.RB11){ // Da esquerda para a direita

			PORTB = 0x8000; 	// RB15 1, 0 restantes RB14... RB0 
			__delay_ms(freq);
			PORTB = 0x4000;
			__delay_ms(freq);
			PORTB = 0x2000;
	        __delay_ms(freq);
			PORTB = 0x1000;
			__delay_ms(freq);

		}else{ 				// Da direita para a esquerda

			PORTB = 0x1000;
			__delay_ms(freq);
			PORTB = 0x2000;
	        __delay_ms(freq);
	        PORTB = 0x4000;
			__delay_ms(freq);
			PORTB = 0x8000; 	// RB15 1, 0 restantes RB14... RB0 
			__delay_ms(freq);

		}

	} while (1);	

    return 0;
}
