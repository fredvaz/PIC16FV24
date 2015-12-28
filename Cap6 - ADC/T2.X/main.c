/*
 * File:   main.c
 * Author: fredericovaz & joaofreitas
 *
 * Capitulo 6 - T2 - Piranometro
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

  CLKDIVbits.DOZE = 0;               // 1:1
  CLKDIVbits.RCDIV = 0;              // 8 MHz
}
// Configura os Pinos
inline void IOconfig(void) {

  ANSA = 0;                          // Porto A digital
  TRISAbits.TRISA0 = 0;              // saida (LED Microstick)

  ANSB = 0xC000;                     // Porto B c/ RB15, RB14 Analogico

  TRISBbits.TRISB2 = 1;              // entrada (U1RX)
  TRISBbits.TRISB7 = 0;              // saida (U1TX)

  TRISBbits.TRISB14 = 1;             // Entrada do AMPOP
  TRISBbits.TRISB15 = 1;             // Saida do AMPOP e tb sera a entrada do ADC
}
// Init ADC
inline void ADCinit(void) {

  AD1CON1 = 0x0470;                   // 12-bit ADC, Internal counter ends sampling and starts conversion (auto-convert)
  AD1CON2 = 0x0000;                   // Configure A/D voltage reference and buffer fill modes, Vr+ and Vr- from AVdd and AVss
  AD1CON3 = 0x100F;                   // Sample time = 16 Tad, Tad = 16Tcy = 1us

  AD1CSSL = 0;                        // No inputs are scanned
  AD1CHS = 0x0009;                    // Select analog Input (AN9 - Pin 26)

  _AD1IF = 0;                         // Clear A/D conversion interrupt.
  _AD1IE = 0;                         // Disable A/D conversion interrupt

  AD1CON1bits.ADON = 1;
}
// Funcao de leitura do buffer do ADC
unsigned int readADC(unsigned int ch) {

    AD1CHS = ch;                      // Select analog input channel
    AD1CON1bits.SAMP = 1;             // start sampling, then go to conversion

    while (!AD1CON1bits.DONE);        // conversion done?
    return(ADC1BUF0);                 // yes then get ADC value
}
// Configuracao Ampop
inline void configAMPOP(void) {

  AMP2CON = 0;                        // Iniciliaze/Reset all to 0 
  AMP2CONbits.SPDSEL = 1;             // Higher power and bandwidth (faster response time)

  AMP2CONbits.NINSEL = 0b010;         // Negavative Input OAxIND pin 25
  AMP2CONbits.PINSEL = 0b000;         // Positive Input AVSS - GND

  AMP2CONbits.AMPEN = 1;              // Enable Ampop
}
//
int main(void) {


  CLKconfig();
  IOconfig();
  configAMPOP();

  ADCinit();
  UART1Init();

  printf("Piranometro\n");
  
  unsigned int valor;

	do{

    valor = readADC(9);             // The Channel 9 (AN9) is connected to AMPOP OUT - PIN 25

    printf("Intensidade: %u mW/cm3\n", valor);

    __delay_ms(500);                // Slow Debug


	}while(1);

    return 0;
}
