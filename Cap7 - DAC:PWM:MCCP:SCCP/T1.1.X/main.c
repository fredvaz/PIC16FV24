/*
 * File:   main.c
 * Author: fredericovaz & joaofreitas
 *
 * Created on December 16, 2015, 11:56 PM
 */


#include "xc.h"
#include "config.h"
#include <stdio.h>
#include <libpic30.h>



#define BAUDRATE 19200U
#define BRG 51


#define MAXPWM 7999							// Fcy/PS/MAXPWM = 16MHz/8000 = 2kHz 


// Inicia conexao com a Porta Serial
inline void UART1Init(void) {

 	U1BRG = BRG;
 	U1MODE = 0x8000;
 	U1STA = 0x0400;
}
// Configura o oscilador p/ 32Mhz
inline void CLKconfig(void) {

  	CLKDIVbits.DOZE = 0;                  	// 1:1
  	CLKDIVbits.RCDIV = 0;                 	// 8 MHz
}
// Configura os Pinos
inline void IOconfig(void) {

  	ANSA = 0;                             	// Porto A digital
  	TRISAbits.TRISA0 = 0;                 	// saida (LED Microstick)

  	ANSB = 0x0000;                        	// Porto B 
  	TRISBbits.TRISB2 = 1;                 	// entrada (U1RX)
  	TRISBbits.TRISB7 = 0;                 	// saida (U1TX)
  	TRISBbits.TRISB8 = 0;                	// saida do PWM

}
// Congigura a unidade MCCP1
inline void CCP1config(void) {

	// Set MCCP operating mode

	CCP1CON1L = 0;						  	// Coloca tudo a zero inicialmente
	CCP1CON1Lbits.MOD = 0b0101; 			// Set mode (Dual-Compare/PWM mode)
	
	// CCP1PRH=0; 							// Disable second timer

	CCP1CON1Lbits.TMRPS = 0b00; 			// Pre-scaler (1:1) 62.5ns 


	// Configure MCCP output for PWM signal

	CCP1CON2Hbits.OCBEN = 1; 				// Enable output signal (OC1B/RB8/pin17) CCP1CON3H = 0;

	CCP1TMRL = 0x0000; 						// Initialize timer cnt
	CCP1PRL = MAXPWM; 						// Configure timebase period
	CCP1RA = 0x1000; 							// Set the rising edge compare value
	CCP1RB = 0x8000; 							// Set the falling edge (=RA, no output)
	
	_CCT1IF=0;
	_CCT1IE=1; 								// Activa interrupcao do periodo
	
	CCP1CON1Lbits.CCPON=1; 					// Enable CCP
}
// Interrupcao do Timer L
void _ISRFAST _CCT1Interrupt(void){ 


	_CCT1IF=0; 
}

inline void CCP1Config(void){

	CCP1CON1L=0; // Reset CCP module: 16 bit timer/counter

	CCP1CON1Lbits.CLKSEL=0b000; // FCY 16MHz 
	CCP1CON1Lbits.MOD=0b0111; 				// frequency output mode 

	CCP1RA=; // Fout=40kHz

	//Configure MCCP output for PWM signal
	CCP1CON3Hbits.OUTM = 0b000; // Set Single Output mode 
	CCP1CON3Hbits.POLACE = 0; //Configure output polarity (Active High)
	CCP1CON2Hbits.OCBEN = 1; // Enable desired output signals (OC1B)

	CCP1CON1Lbits.CCPON=1; // Enable CCP 

}
// 
int main(void) {

	CLKconfig();
  	IOconfig();
  	//CCP1config();
  	CCP2Config();


  	do{




  	}while(1);

    return 0;
}
