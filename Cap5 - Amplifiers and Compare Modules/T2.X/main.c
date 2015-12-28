/* 
 * File:   PL5_AnemPWM.c
 * Author: Lino
 *
 * Created on 29 de Outubro de 2014, 15:14
 */
 
/*
 *
 * DESCREVER A IDEIA IMPLEMENTADA
 *
 */
 
#include <xc.h>
#include "config.h"
#include <stdio.h>
 
/*
 * Prototipos de funушes
 */
inline void ConfigCLK(void);
inline void UART1Init(unsigned long int brg);
 
inline void TMR1Init(void);
void _ISR _T1Interrupt( void);
void _ISR _CompInterrupt(void);
 
/* Global vars */
volatile unsigned int Ton=0, Nvezes=0;
unsigned int tempo = 0;
 
int main(void) {
 
    ConfigCLK();
    UART1Init(19200);
 
    // Config IO
    ANSB = 0b1111111101110001;   // Delta, Dbg, RX1 e TX1 digitais;
    TRISB= 0b1111111101110101;
/*
    TRISBbits.TRISB0 = 1;   // entrada (C3IND)
    TRISBbits.TRISB1 = 0;   // Aquecimento
    TRISBbits.TRISB2 = 1;   // (U1RX)
    TRISBbits.TRISB3 = 0;   // (dbg)
    TRISBbits.TRISB7 = 0;   // (U1TX)
    TRISBbits.TRISB8 = 0;   // CMP3 out (dbg)
*/
 
    TMR1Init();
 
    // Config VREF
    CVRCON = 0b11010000;    // VREF = 16/32 * 5V; (Disable) Out (pin 25)
    CVRCONbits.CVR=12;
    // Config CMP3
    CM3CONbits.COE = 1;     // Enable output (pin 17)
    CM3CONbits.EVPOL=0b01;  // Gera evento na transicao de 0 para 1
    CM3CONbits.CREF= 0b00;  // +IN is C3INA (pin26)
    CM3CONbits.CCH = 0b10;  // -IN is C3IND (pin 4)
    CM3CONbits.CON = 1;     // Turn Comparator ON
    _CMIF=0;                // Limpa flag de interrupcao
    _CMIE=1;                // Activa interrupcao do comparador
 
    printf("\rStart");
 
    while(1) {
 
        if(Nvezes>50){
            printf("\r Tempo ao fim de 50 vezes: %u", tempo); // MOSTRA RESULTADO
        }
        printf("Tempo: %u\n", tempo);
 
    } // main loop
    return(1);
}
 
inline void TMR1Init(void){
    // init Timer 1
    // AJUSTAR CASO SEJA NECESSARIO
 
    TMR1 = 0;       // clear the timer
    PR1 = 64000-1;  // set the period register (64k * 500 ns = 32ms)
 
    // configure Timer1 module
    T1CON = 0x8010; // enabled, prescaler 8:1, internal clock
 
    // init the Timer 1 Interrupt, clear the flag, enable the source
    _T1IF = 0;   //Clear the Timer1 interrupt flag
    _T1IE = 1;   //Enable Timer1 interrupt
}
 
void _ISR _T1Interrupt( void) {

    LATBbits.LATB1 = 1;       // Liga aquecimento
    Nvezes++;
 
    _T1IF = 0; //Clear Timer1 interrupt status flag

} // T1Interrupt
// Interupcao do Comparador atingiu 
void _ISR _CompInterrupt(void) {

    LATBbits.LATB1 = 0;      // Desliga o aquecimento
    tempo = TMR1;            // Capta Tempo 

 
    _CMIE=0;
    _CMIF=0;
}
 
inline void ConfigCLK(void)
{
    //configurar oscilador para 32MHz
    CLKDIVbits.DOZE = 0;    // 1:1
    CLKDIVbits.RCDIV = 0;   // 8 MHz
}
 
inline void UART1Init(unsigned long int baud){
    U1BRG =  (FCY / (16 * baud)) -1; //BAUDRATEREG1;
    U1MODE = 0;
    U1MODEbits.BRGH = 0;
    U1STA = 0;
    U1MODEbits.UARTEN = 1;
    U1STAbits.UTXEN = 1;
    U1STAbits.URXISEL = 2;
    U1MODEbits.UEN = 3;
}
