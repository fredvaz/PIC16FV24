/* 
 * File:   lab6t2_v2.c
 * Author: Tiago
 *
 * Montagem com ampop em transimpedância
 *
 * Created on 24 de Novembro de 2014, 15:40
 */

#include <stdlib.h>
#include <stdio.h>
#include <xc.h>
#include "config.h"
#include <libpic30.h>

//double lastValueFiltered = 0;
//double valueFiltered = 0;
//double alpha = 0.3;
int counter = 0;
unsigned int value = 0;

inline void ConfigTMR1(void){
    T1CON = 0x8030; // Enabled, prescaler 1:256, 16 us tck @ Fcy=16MHz
    TMR1 = 0;
    PR1 = 625 - 1;    // Overflow a cada segundo

    _T1IF = 0;
    _T1IE = 1;      // Activa interrupcoes
}

void _ISRFAST _T1Interrupt(void){
// Timer1 interrupt service routine




    _T1IF = 0;              // clear the interrupt flag
} //T1Interrupt

void _ISR _ADC1Interrupt(void){

    //lastValueFiltered = valueFiltered;

    value = ADC1BUF0;
    //valueFiltered = ((1 - alpha) * lastValueFiltered) + (alpha * value);
    
    counter++;

    IFS0bits.AD1IF = 0;
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
int main(int argc, char** argv) {
    //configurar oscilador para 32MHz
    CLKDIVbits.DOZE = 0;    // 1:1
    CLKDIVbits.RCDIV = 0;   // 8 MHz

    ConfigTMR1();

    UART1Init(19200);

    // Config OA1; OA1OUT: pin 7
    AMP1CONbits.AMPSLP = 1;     // Fast mode
    AMP1CONbits.PINSEL = 0b000; // Positive input connected to AVss (aka GND)
    AMP1CONbits.NINSEL = 0b001; // Negative input connected to OA1IND (pin 25)
    AMP1CONbits.AMPEN = 1;      // Enable Op Amp

    AD1CON1bits.MODE12 = 1;
    AD1CON1bits.FORM = 0;
    AD1CON1bits.SSRC = 0b0101;
    AD1CON1bits.ASAM = 1;
    AD1CON1bits.SAMP = 1;
    
    AD1CON2bits.PVCFG = 0;
    AD1CON2bits.NVCFG0 = 0;
    AD1CON2bits.CSCNA = 0;
    AD1CON2bits.SMPI = 0;
    AD1CON2bits.ALTS = 0;

    AD1CON3 = 0x000F;   //  Tad=16xTcy=1us

    AD1CHS = 5;

    IFS0bits.AD1IF = 0;
    IEC0bits.AD1IE = 1;

    AD1CON1bits.ADON = 1;

    ANSB = 0;
    ANSA = 2;

    TRISAbits.TRISA1 = 1;

    for(;;){
        if(counter > 9){
            counter = 0;
            printf("Value: %u\n", value);
        }
    }

    return (EXIT_SUCCESS);
}

