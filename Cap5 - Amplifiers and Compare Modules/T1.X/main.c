/*
 * File:   main.c
 * Author: fredericovaz
 *
 * Capitulo 5 - T2
 *
 *
 * Created on November 15, 2015, 3:27 PM
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
inline void CLKconfig(void) {

  CLKDIVbits.DOZE = 0;                   // 1:1
  CLKDIVbits.RCDIV = 0;                  // 8 MHz
}
// Configura os Pinos
inline void IOconfig(void) {

  ANSA = 0;                              // Porto A digital
  TRISAbits.TRISA0 = 0;                  // saida (LED Microstick)

  ANSB = 0xC000;                         // Porto B RB15, RB14 Analalogico
  TRISBbits.TRISB2 = 1;                  // entrada (U1RX)
  TRISBbits.TRISB7 = 0;                  // saida (U1TX)

  TRISBbits.TRISB14 = 1;	               // -IN -> entrada do Ampop
  TRISBbits.TRISB15 = 1;	               // +IN -> entrada do Ampop
  TRISBbits.TRISB8 = 0;	                 // output -> saida do Ampop

}
// Configura VREF
inline void VREFconfig(void) {

  CVRCON = 0b11010000;                   // VREF = 16/32 * 5V; Enable Out (pin25)
}
// Configura Comparador 3
inline void CM3config(void) {

	CM3CONbits.COE = 1;		                 // Enable output (pin 17)
	CM3CONbits.CREF= 0;		                 // +IN is VREF (pin 26)
	CM3CONbits.CCH = 0;		                 // -IN is C3INB (pin 25)

	CM3CONbits.CON = 1;		                 // Turn Comparator ON   
}

int main(void) {


  CLKconfig();
  IOconfig();
  VREFconfig();
  CM3config();

  UART1Init();

  printf("COMPARADOR\n");
   
	do{

    	if (CM3CONbits.COUT) // Quando utrapassar VDD/2 -> COUT = 1 (ou _CMIF = 1, se activarmos a iterrupcao)
    		printf("VIN+ > VIN-\n");


	}while(1);

    return 0;
}
