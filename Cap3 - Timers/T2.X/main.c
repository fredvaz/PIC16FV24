/*
 * File:   main.c
 * Author: Frederico Vaz & João Freitas
 *
 * Created on October 24, 2015, 6:45 PM
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
    TRISAbits.TRISA1 = 1;   // entrada (Echo)
    TRISAbits.TRISA0 = 0;   // saida (Trigger)

    ANSB = 0;               // Porto B digital
    TRISBbits.TRISB2 = 1;   // entrada (U1RX)
    TRISBbits.TRISB7 = 0;   // saida (U1TX)
}
// Configura Timer1 - 4us
inline void configTMR1(void) {

    T1CON = 0x8020; 		// T1 ON - 1:64
    PR1 = 25000-1;    		// Reset a contagem qd 100ms
}

int main(void) {

	  configCLK();
    configIO();
    configTMR1();

    UART1Init();

    printf("Interface with Sonar HC-SR04\n");

    unsigned int Start_time, Final_time;
    unsigned int distance;
    
    do{

    	TMR1 = 0;					// Reset a contagem Timer1


    	LATAbits.LATA0 = 1;			// Trigger 1 duracao do pulso 12us TRM1 = 3 (4us*3)
    	while(TMR1 < 3);
    		LATAbits.LATA0 = 0;		// Trigger 0

    	while(!PORTAbits.RA1);		// Espera pelo inicio do sinal Echo
  			Start_time = TMR1; 		

  		while(PORTAbits.RA1);		// Espera pelo fim do sinal Echo
  			Final_time = TMR1; 		


  		// Calcula distancia em cm - Velocidadde do Som = 340 m/s <=> 0,034cm/us

  		distance = ((Final_time - Start_time)*2)/29;	// 1/29 = 0,034 (p/ evitar as casas decimais)
  														// (Contagens*4us)/2 - o tempo que medimos é de ida e de volta, basta um, logo dividimos por 2


  		printf("Distance: %ucm\n", distance);

  		while(TMR1 < 24000); 		// Pausa de 60-100ms entre cada leitura para dar tempo ao pulso Echo
   

    }while(1);
   
    return 0;
}














