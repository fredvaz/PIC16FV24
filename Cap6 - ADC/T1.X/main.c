/*
 * File:   main.c
 * Author: fredericovaz & joaofreitas
 *
 * Capitulo 6 - T1 - Potenciometro
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


unsigned int amostra[100];


double lastValueFiltered = 0;
double valueFiltered = 0;
double alpha = 0.3;
int n_samples = 0;
unsigned int value = 0;


// Inicia conexao com a Porta Serial
inline void UART1Init(void) {

 	U1BRG = BRG;
 	U1MODE = 0x8000;
 	U1STA = 0x0400;
}
// Configura o oscilador p/ 32Mhz
inline void CLKconfig(void) {

  CLKDIVbits.DOZE = 0;                  // 1:1
  CLKDIVbits.RCDIV = 0;                 // 8 MHz
}
// Configura os Pinos
inline void IOconfig(void) {

  ANSA = 0;                             // Porto A digital
  TRISAbits.TRISA0 = 0;                 // saida (LED Microstick)

  ANSB = 0x8000;                        // Porto B c/ RB15 Analogico
  TRISBbits.TRISB2 = 1;                 // entrada (U1RX)
  TRISBbits.TRISB7 = 0;                 // saida (U1TX)
  TRISBbits.TRISB15 = 1;                // entrada analogica do Poteciometro

}
// Configuracao do Timer1
inline void ConfigTMR1(void) { 
    
  T1CON = 0x8030;                       // Enabled, prescaler 1:256, 16 us tck @ FCY=16MHz
  TMR1 = 0;
  PR1 = 625-1;                          // 10ms Taxa 10Hz

  _T1IF = 0;
  _T1IE = 1;                            // Activa interrupcoes
}
// Init ADC
inline void ADCinit(void) {

  AD1CON1 = 0x0470;                     // 12-bit ADC, Internal counter ends sampling and starts conversion (auto-convert)
  AD1CON2 = 0x0000;                     // Configure A/D voltage reference and buffer fill modes, Vr+ and Vr- from AVdd and AVss
  AD1CON3 = 0x1FFF;                     // Sample time = 31 Tad, Tad = 64Tcy = 4us

  AD1CSSL = 0;                          // No inputs are scanned
  AD1CHS = 0x0009;                      // Select analog Input (AN9 - Pin 26)

  _AD1IF = 0;                           // Clear A/D conversion interrupt.
  _AD1IE = 0;                           // Disable A/D conversion interrupt

  AD1CON1bits.ADON = 1;
}
// Timer1 - A cada 10ms faz aquisição (100 Amostras por 1s)
void _ISRFAST _T1Interrupt(void) {


  lastValueFiltered = valueFiltered;    // Save last Value Filtered

  AD1CON1bits.SAMP = 1;                 // Start sampling, then go to conversion

  while (!AD1CON1bits.DONE);            // Wait for conversion

  value = ADC1BUF0;                     // Read the Buffer

  valueFiltered = ((1 - alpha) * lastValueFiltered) + (alpha * value);
    
  n_samples++;                          // Increment number of samples

  AD1CON1bits.SAMP = 0;                 // Stop sampling (it's need?)
  _T1IF = 0;                            // clear the interrupt flag
}
//
int main(void) {

  CLKconfig();
  IOconfig();
  ConfigTMR1();

  ADCinit();
  UART1Init();

  printf("100 Amostras por segundo!\n");

	do{

    // Mostra a taxa de aquisição de 10Hz, ou seja, ao fim de 10 Samples
                       
    if(n_samples > 9){ 
        n_samples = 0;
        printf("Last Sample: %u\nValue filtered: %f\n", value, valueFiltered);
    }

	}while(1);

  return 0;
}















