/*
 * File:   main.c
 * Author: fredericovaz
 *
 * Created on November 19, 2015, 4:48 PM
 */


#include "xc.h"
#include "config.h" 
#include <libpic30.h> // Delay




int main(void) {

    CLKDIVbits.DOZE = 0; 	// 1:1  
	CLKDIVbits.RCDIV = 0;	// 8MHz
    
	ANSB = 0x0000;
	//TRISB = 0x0000;
	TRISBbits.TRISB15 = 0;


	do{

		PORTBbits.RB15 = 1;
        __delay_ms(1000);
		PORTBbits.RB15 = 0;
		__delay_ms(1000);

	}while(1);

    return 0;
}

