/*
 * File:   main.c
 * Author: fredericovaz
 * 
 * Tarefa 1 - Timers
 * 
 *
 * Created on October 21, 2015, 9:45 PM
 */


#include "xc.h"
#include "config.h"
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
    TRISAbits.TRISA1 = 1;   // entrada (botao)
    TRISAbits.TRISA0 = 0;   // saida (LED da Microstick)

    ANSB = 0;               // U1RX e U1TX digitais
    TRISBbits.TRISB2 = 1;   // entrada (U1RX)
    TRISBbits.TRISB7 = 0;   // saida (U1TX)
}
// Configura Timer1
inline void ConfigTMR1(void) {

    T1CON = 0x8030; // T1 ON; 1:256
    PR1 = 625-1;    //10 ms
}
// Funcao teste
inline void wait10msTMR1(unsigned int ms10) {

	T1CON = 0x0000; 		// Para TMR1 e faz reset ao registo de controlo
	TMR1 = 1; 				// Inicializa/Reset o registo do Timer1
	// PR1 = 625*ms10; 			// Reset ao registo TMR1 quando chegar ao periodo de contagem 10ms

	ms10 *= 625; 			// 625 timer ticks 10ms

	T1CON = 0x8030; 		// Ligar timer config
	while(TMR1 < ms10);
}

int main() {

	configCLK();
    configIO();	
    UART1Init();
    //configTMR1();

	do{


		wait10msTMR1(100); // Espera um 1s
		
		printf("Hello World!\n"); // Debug


	} while (1);	

    return 0;
}










