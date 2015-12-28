/*
 * File:   main.c
 * Author: Frederico Vaz & Joao Freitas
 *
 * [SI - PL1][Capitulo 4] T2 - Conta rotacoes
 *
 * Created on October 29, 2015, 9:48 PM
 */


#include "xc.h"
#include "config.h"
#include <stdio.h>
#include <libpic30.h>



#define BAUDRATE 19200U
#define BRG 51



unsigned int pulses;			// nº de impulsos ocorridos



// Inicia conexao com a Porta Serial
inline void UART1Init(void) {

 	U1BRG = BRG;
 	U1MODE = 0x8000;
 	U1STA = 0x0400;
}
// Configura o oscilador p/ 32Mhz
inline void CLKconfig(void) {

    CLKDIVbits.DOZE = 0;    			// 1:1
    CLKDIVbits.RCDIV = 0;   			// 8 MHz
}
// Configura os Pinos
inline void IOconfig(void) {

    ANSA = 0;               			// Porto A digital
    TRISAbits.TRISA0 = 0;   			// saida (LED Microstick)

    // ANSB = 0;               			// Porto B digital
    // TRISBbits.TRISB2 = 1;  				// entrada (U1RX)
    // TRISBbits.TRISB7 = 0;   			// saida (U1TX)
    // TRISBbits.TRISB15 = 1;				// Input TCKIA
    ANSB = ~0x8084;         // RB15, RX1 e TX1 digitais, restantes analogicos;
    TRISB= 0b1111111101111111;
}
// Configura Capture Module CCP1 - Gera de 1s em 1s um Interrupt
inline void CCP1config(void) {

	CCP1CON1L = 0;						// Reset CCP module
	CCP1CON1Lbits.CLKSEL = 0b011;		// LPRC (31 kHz source)
	CCP1CON1Lbits.TMRPS = 0b00;			// Preescaler 1:1 - T = 1/31K s
	
	CCP1TMRL = 0;						// Iniciatilize buffer timer
	CCP1PRL = 15500-1;					// 500ms - 2Hz (Reset buffer)

	CCP1CON1Lbits.CCPON = 1;			// Enable CCP

	_CCT1IF = 0;
	_CCT1IE = 1; 						// Enable CCP Timer1 Interrupt
}
// Configura Capture Module CCP2 - Recebe evento/impulso no pino 26 - TCKIA
inline void CCP2config(void) {

	CCP2CON1L = 0; 						// Reset CCP module: 16 bit counter, 1:1
	CCP2CON1Lbits.CLKSEL = 0b111;		// TCLKIA input - External "Clock" is the Enconder
	CCP2CON1Lbits.CCSEL = 1;			// Input Capture mode
	CCP2CON1Lbits.MOD = 0b0011;			// Capture every rising and falling edge

	CCP2TMRL = 0;						// Buffer to count impulses
	CCP2PRL = 0xFFFF;					// Buffer reset when Max

	CCP2CON1Lbits.CCPON = 1;			// Enable CCP1

    _CCT2IF = 0; 
    _CCT2IE = 0;  						// Disable interrupt of CCP Timer2		
}
// Rotina da Interrupcao - Lê nº impulsos/eventos
void _ISR _CCT1Interrupt(void) {

	// Vamos buscar o nº de implusos dentro do CCP2, utilizado o CCP1 para gerar esta interrupção
	
	static unsigned int count,lastCount=0;

    count=CCP2TMRL;  

    if(count<lastCount){
        pulses=0xFFFF-lastCount+count;
    }
    else{
        pulses=count-lastCount;
    }
    lastCount=count;

	_CCT1IF = 0;
}
// Interrucao do ?? CCT1 e CCP1 ??
void _ISR _CCP1Interrupt(void){

	// Nao e' necessaria nem utilizada
    _CCP1IF=0;
}


int main(void) {


	CLKconfig();
    IOconfig();
    CCP1config();
    CCP2config();

    UART1Init();

    printf("RPM with Enconder!\n");

    int rpm;

    do{

    	rpm = 10*pulses;			// Recta linear y = mx + b;

    	printf("%d RPM\n", rpm);
    	

    }while(1);

    return 0;
}

















