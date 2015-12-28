/*
 * File:   main.c
 * Author: fredericovaz
 *
 * Capitulo 6 - TPC6 - Oximetro
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


unsigned int amostra[100], i = 0, show = 0;


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

  ANSB = 0x8000;                     // Porto B c/ RB15 Analogico
  TRISBbits.TRISB2 = 1;              // entrada (U1RX)
  TRISBbits.TRISB7 = 0;              // saida (U1TX)
  TRISBbits.TRISB15 = 1;             // entrada analogica do Poteciometro

}
// Configuracao do Timer1
inline void ConfigTMR1(void) { 
    
  T1CON = 0x8030;                    // Enabled, prescaler 1:256, 16 us tck @ FCY=16MHz
  TMR1 = 0;
  PR1 = 625-1;                       // 10ms Taxa 10Hz

  _T1IF = 0;
  _T1IE = 1;                         // Activa interrupcoes
}
// Init ADC
inline void ADCinit(void) {

  AD1CON1 = 0x0470;                   // 12-bit ADC, Internal counter ends sampling and starts conversion (auto-convert)
  AD1CON2 = 0x0000;                   // Configure A/D voltage reference and buffer fill modes, Vr+ and Vr- from AVdd and AVss
  AD1CON3 = 0x1FFF;                   // Sample time = 31 Tad, Tad = 64Tcy = 4us

  AD1CSSL = 0;                        // No inputs are scanned
  AD1CHS = 0x0009;                    // Select analog Input (AN9 - Pin 26)

  _AD1IF = 0;                         // Clear A/D conversion interrupt.
  _AD1IE = 0;                         // Disable A/D conversion interrupt

  AD1CON1bits.ADON = 1;
}
// Timer1 - A cada 10ms faz aquisição (100 Amostras por 1s)
void _ISRFAST _T1Interrupt(void) {

  AD1CON1bits.SAMP = 1;               // Start sampling, then go to conversion

  while (!AD1CON1bits.DONE);          // conversion done?
    amostra[i] = ADC1BUF0;
  i++; 

  AD1CON1bits.SAMP = 0;               // Stop sampling (it's need?)
  _T1IF = 0;  // clear the interrupt flag
}
//
int main(void) {


	CLKconfig();
  IOconfig();
  ConfigTMR1();

  ADCinit();
  UART1Init();

  printf("100 Amostras por segundo!\n");

  float a = 0.5;                    // ?
  
  unsigned int val = 0;

	do{

    //unsigned int val = (1-a)*amostra[i-1] + a*amostra[i];
  
    val += amostra[i];
    val = val/(i+1);                  // Media (alternativa ao exercicio)

    if(show);                       // Mostra a taxa de aquisição de 10Hz
      printf("Valor: %u Ultimo: %u\n", val, amostra[i]);


    if(i > 99) {                     // Reset as Amostras
      i = 0;
      val = 0;
    }
	}while(1);

    return 0;
}
