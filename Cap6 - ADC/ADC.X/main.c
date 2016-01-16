/*
 * File:   main.c
 * Author: fredericovaz & joaofreitas
 *
 * Projecto - ADC 12 bits, 5 V / 4096 = 1.22mV de Resolução
 *
 * Como o nosso sinal a ser lido têm um 3 vezes a freq do de entrada então vamos adquerir
 * 768 Samples (3*256) de modo a que garantimos a aquisição de um sinal consiso 
 *
 * Mas 1º vamos ver o que resulta com 256
 * 
 * Definir o período de aquisção de 256 samples através do TimerL da MCCP1:
 * 
 * 30Hz = 0,033(3) s
 * 0,033(3) / 256 = 13 us por sample
 * PR1 = 13 us / 4 us (Pre-scaler 1:64, 4us)
 * 
 * Samples/Dados enviados pela UART! 
 *
 * Created on November 15, 2015, 3:27 PM
 */

#include "xc.h"
#include "config.h"
#include <stdio.h>
#include <libpic30.h>



#define BAUDRATE 19200U
#define BRG 51
#define N_SAMPLES 512


unsigned int sample[N_SAMPLES];
unsigned int print = 0;



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
  TRISBbits.TRISB2 = 1;                 // Entrada (U1RX)
  TRISBbits.TRISB7 = 0;                 // Saida (U1TX)
  TRISBbits.TRISB15 = 1;                // Entrada analogica do Sinal - PIN 26 AN9

}
// // Configuração da MCCP1
// inline void configMCPP1(void) {

//   CCP1CON1L = 0x0000;                   // Coloca tudo a zero inicialmente
  
//   CCP1CON1Lbits.TMRPS = 0b00;           // Pre-scaler (1:64) 4us

//   CCP1TMRL = 0x0000;                    // Initialize timer cnt
//   CCP1PRL = 98-1;                       // Configure timebase period
  
//   _CCT1IF=0;
//   _CCT1IE=1;                            // Activa interrupcao do periodo
  
//   CCP1CON1Lbits.CCPON=1;                // Enable CCP

// }
// // Interrupcao do Timer do MCCP1
// void _ISRFAST _CCT1Interrupt(void){ 
//   // Nada para fazer, apenas sinaliza o acontecimento da interrupcção para o ADC
//   _CCT1IF=0; 
// }

// Configuracao do Timer1
inline void ConfigTMR1(void) { 
    
  T1CON = 0x8030;                       // Enabled, prescaler 1:256, 16 us tck @ FCY=16MHz
  //T1CONbits.TCKPS = 0b10;                // 1:64 4us 
  TMR1 = 0;
  PR1 = 24-1;                           // Taxa 10Hz

  _T1IF = 0;
  _T1IE = 1;                            // Activa interrupcoes
}
// Timer1 
void _ISRFAST _T1Interrupt(void) {
  // Nada para fazer, apenas sinaliza o acontecimento da interrupcção para o ADC

  // AD1CON1bits.SAMP = 1;                 // start sampling, automatic

  // while(!AD1CON1bits.DONE);
    _T1IF = 0;                          // clear the interrupt flag
}

// Configuração do ADC
inline void ADCinit(void) {

  AD1CON1 = 0x0000;                     // 12-bit ADC, Internal counter ends sampling and starts conversion (auto-convert)
  AD1CON1bits.MODE12 = 1;
  AD1CON1bits.SSRC = 0b0101;            // Internal counter (0b0111), TMR1 event (0b0101), ends sampling and starts conversion MCCP1 (0b0010)
  AD1CON1bits.ASAM = 1;

  AD1CON2 = 0x0000;                     // Configure A/D voltage reference and buffer fill modes, Vr+ and Vr- from AVdd and AVss
  
  AD1CON3 = 0x0000;                     // Sample time = 31 Tad, Tad = 64Tcy = 4us
  AD1CON3bits.SAMC = 0b01111;
  AD1CON3bits.ADCS = 0b00010000;

  AD1CSSL = 0;                          // No inputs are scanned
  AD1CHS = 0x0009;                      // Select analog Input (AN9 - Pin 26)

  AD1CON1bits.ADON = 1;

  _AD1IF = 0;                           // Clear A/D conversion interrupt.
  _AD1IE = 1;                           // Enable A/D conversion interrupt
}
// Sempre que capta uma sample gera um Interrupt do ADC
void _ISR _ADC1Interrupt(void){

  static unsigned int i = 0;            // 2 bytes, 16 bits de operação

  // Aquisição e Conversão imediata para Tensão

  sample[i] = ADC1BUF0;

  //double tensao =  (5.0/4095.0)*sample[i]; // Não esquecer de por 2.0 1.0 etc se não a conta é sempre 0!
  //printf("%d, ", sample[i]);
    
  //i &= N_SAMPLES-1;
  i++;

  if(i == N_SAMPLES-1){                   // Mecanismo para que o printf evite consumir tempo
    
    print = 1;
    printf("Hello!\n");
    _AD1IE = 0;                      // Desactiva iterrupcção

  }

  _AD1IF = 0;                           // IFS0bits.AD1IF = 0;
}
//
int main(void) {

  CLKconfig();
  IOconfig();
  ConfigTMR1();

  ADCinit();
  UART1Init();


	do{

    if(print == 1) {

      int j = 0;

        for(j = 0; j < N_SAMPLES; j++){
            printf("%d, ", sample[j]);

        }

      print = 0;
      printf("FIM!!\n");
    }
                     

	}while(1);

  return 0;
}















