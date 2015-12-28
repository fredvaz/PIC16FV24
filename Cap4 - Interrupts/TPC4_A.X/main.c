/* 
 * File:   main.c
 * Author: Joao Freitas & Frederico Vaz
 *
 * [SI - PL1][Capitulo 4] Desafio 2 - Deslocamento de uma plataforma robotica
 *
 * Neste trabalho foi utilizado uma plataforma da Pololu, Encoders da Pololu com 24 counts per revolution 
 * Porém como o PIC fica indeferente ao modo Rising Edge e Falling Eddge, não sabendo qual a razão
 * só ultilizamos 12 counts per revolution
 *
 * Fizemos uso dos interrrupts externos INT1 na roda/Enconder Direito e INT2 na roda/Encoder Esquerdo
 *
 * Basicamente determinamos quantos Impulsos ocorrem em 100ms em cada Roda, que são mutiplicados
 * Pelo deslocamento correspondente a um só Impulso
 *
 * Created on 5 de Novembro de 2015, 15:08
 */


#include "xc.h"
#include "config.h"
#include <stdio.h>
#include <libpic30.h>
#include <math.h>



#define BAUDRATE 19200U
#define BRG 51



unsigned int countR = 0, countL = 0, r_pulses = 0, l_pulses = 0, calculate = 0;


// Inicia conexao com a Porta Serial
inline void UART1Init(void) {

    U1BRG = BRG;
    U1MODE = 0x8000;
    U1STA = 0x0400;
}
// Configura o oscilador p/ 32Mhz
inline void CLKconfig(void) {

    CLKDIVbits.DOZE = 0;                // 1:1
    CLKDIVbits.RCDIV = 0;               // 8 MHz
}
// Configura os Pinos
inline void IOconfig(void) {

    ANSA = 0;                           // Porto A digital
    TRISAbits.TRISA0 = 0;               // saida (LED Microstick)

    ANSB = 0;                           // Porto B digital
    TRISBbits.TRISB2 = 1;               // entrada (U1RX)
    TRISBbits.TRISB7 = 0;               // saida (U1TX)
    TRISBbits.TRISB14 = 1;              // Input INT1
    TRISBbits.TRISB12 = 1;              // Input INT2

}
// Configura Capture Module CCP3 como um Timer 16 bits - T = 100ms
inline void CCP1config(void) {

    CCP1CON1L = 0;                      // Reset CCP module
    CCP1CON1Lbits.CLKSEL = 0b011;       // LPRC (31 kHz source)
    CCP1CON1Lbits.TMRPS = 0b00;         // Preescaler 1:1 - T = 1/31K s
    
    CCP1TMRL = 0;                       // Iniciatilize buffer timer
    CCP1PRL = 3200-1;                   // 100ms (Reset buffer)

    CCP1CON1Lbits.CCPON = 1;            // Enable CCP

    _CCT1IF = 0;
    _CCT1IE = 1;                        // Enable CCP Timer1 Interrupt

}
// Configura Interrupt Externo INT1 - Capta Impulsos do Encoder Direito - Pino 25
inline void INT1config(void) {

   INTCON1 = 0;                         // Setup INT1
   
   _INT1IF = 0;                         // Reset INT1 interrupt flag
   _INT1IE = 1;                         // Enable INT1 Interrupt Service Routine

}
// Configura Interrupt Externo INT2 - Capta Impulsos do Encoder Esquerdo - Pino 23
inline void INT2config(void) {

   INTCON1 = 0;                         // Setup INT2
  
   _INT2IF = 0;                         // Reset INT2 interrupt flag
   _INT2IE = 1;                         // Enable INT2 Interrupt Service Routine
} 
// Determina nº de Impulsos em 100ms para as duas Rodas (Interrupcao gerada pelo Timer1 do modulo)
void _ISR _CCT1Interrupt(void) {

    
    countR = r_pulses;                  // Passa nova contagem - N Impulsos em 100ms
    countL = l_pulses;

    calculate = 1;                      // Manda calcular o Deslocamento e Velocidade

    r_pulses = 0;                       // Reset a contagem 
    l_pulses = 0;

    _CCT1IF = 0;                        //  Clear the CCP Timer1 flag
}
// Incrementa Impulsos Enconder Direito
void _ISR _INT1Interrupt(void){

    r_pulses++;

    _INT1IF= 0;                         // Clear the INT1 interrupt flag
}
// Incrementa Impulsos Enconder Esquerdo
void _ISR _INT2Interrupt(void){

    l_pulses++;

    _INT2IF= 0;                         // Clear the INT1 interrupt flag
}


int main(void) {


    CLKconfig();
    IOconfig();
    CCP1config();
    INT1config();
    INT2config();

    UART1Init();

    printf("Deslocamento e Velocidade!\n");

    int n = 11;                                             // 1 Impulso = 11mm
    int b = 90;

    double x = 0.0, y = 0.0, theta = 0.0;

    int velocidade = 0;

    do{

        if(calculate){

            int dr = n*countR;                              // deslocamento roda Direita
            int dl = n*countL;                              // deslocamento roda Esquerda

            int dm = (dr + dl)/2;

            theta = (double)(dr - dl)/(double)(b) + theta;  // angulo em Radinaos

            double graus = (theta*180)/3.14;

            x = dm*cos(theta) + x;                                                 
            y = dm*sin(theta) + y;

            velocidade = (dm/10)*10;                         // *10 = /0.01 mmm to cm deslocamento por 0.5s

            printf("X: %fmm Y: %fmm Theta: %f Velocidade: %dcm/s\n", x, y, graus, velocidade);
            calculate = 0;
        }
        
    }while(1);

    return 0;
}



