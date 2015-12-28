/*
 * File:   main.c
 * Author: João
 *
 * Created on 27 de Novembro de 2015, 16:16
 */


#include "xc.h"

int main(void) {
    
	ANSB = 0x0000;
	TRISB = 0x0000;
	TRISBbits.TRISB15 = 0;


	do{

		PORTBbits.RB15 = 1;
      //  __delay_ms(1000);
		PORTBbits.RB15 = 0;
	//	__delay_ms(1000);

	}while(1);   
    
    
    
    return 0;
}
