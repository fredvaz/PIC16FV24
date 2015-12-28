/*
 * File:   main.c
 * Author: fredericovaz
 *
 * Created on November 4, 2015, 7:49 PM
 */


#include "xc.h"
#include "config.h"
#include <stdio.h>
#include <libpic30.h>



#define BAUDRATE 19200U
#define BRG 51



// Inicia conexao com a Porta Serial
inline void UART1Init(void) {

 	U1BRG = BRG;
 	U1MODE = 0x8000;
 	U1STA = 0x0400;
}
// Configura o oscilador p/ 32Mhz
inline void configCLK(void) {

    CLKDIVbits.DOZE = 0;    // 1:1
    CLKDIVbits.RCDIV = 0;   // 8 MHz
}
// Configura os Pinos
inline void configIO(void) {

    ANSA = 0;               // Porto A digital
    TRISAbits.TRISA0 = 0;   // saida (LED Microstick)

    ANSB = 0;               // Porto B digital
    TRISBbits.TRISB2 = 1;   // entrada (U1RX)
    TRISBbits.TRISB7 = 0;   // saida (U1TX)
}
// Configura Timer1 - 4us
inline void configTMR1(void) {

    T1CON = 0x8030; 		// T1 ON - 1:256, 16us
    TMR1 = 0;
    PR1 = 62500-1;    		// Reset a contagem qd 1000ms e gera o Interrupt


    _T1IF = 0;
    _T1IE = 1; 				// Activa iterrupção do Timer1

}
// Rotina da Interrupcao
void _ISR _T1Interrupt(void) {


	printf("I'am the Interrupt Routine!\n");

	_T1IF = 0;				// Clear the interrupt flag
}


unsigned int pulses;			// nº de impulsos ocorridos

int main(void) {


	configCLK();
   	configIO();
   	configTMR1();

   	UART1Init();

    printf("Interrupt Timer1 Test!\n");
   
	do{


    	printf("I am main!\n");
     	__delay_ms(500);


	}while(1);

    return 0;
}



