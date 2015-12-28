/* 
 * File:   main.c 
 * Author: Joao Freitas & Frederico Vaz
 *
 *Exercício: TPC4
 *
 * Algoritmo utilizado:
 Para atingir os objectivos propostas foi necessário utilizar um encoder, e determinar
 a posição angular equivalente a cada pulso. Após determinar esse valor foram feitos
 calculos para determinar a posição angular relativamente à posição inicial do enconder.
 Para determinarmos os momentos aquando o encoder alterava a sua posição, utilizamos duis
 interrupções externas, nomeadamente, as associadas aos pin 23 e 25. A maior dificudade
 deste trabalho foi determinar o sentido do desclocamento angular, após alguma investigação
 descobriu-se que o sentido é possível determinar devido a um desfasamento  temporal entre 
 as duas interrupções, com esta informação criamos um algortimo que também fornece o sentido
 *
 * Created on 5 de Novembro de 2015, 15:08
 */

#include "xc.h"
#include "config.h"
#include <stdio.h>
#include <libpic30.h>
#include <math.h>
#include <string.h>



#define BAUDRATE 19200U
#define BRG 51



int count= 0, pulses = 0, _INT1 = 0, _INT2 = 0, calculate = 0;


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
// Configura Interrupt Externo INT1 - Capta Impulsos do Encoder Output A - Pino 25
inline void INT1config(void) {

   INTCON1 = 0;                         // Setup INT1 interupt on falling edge
   
   _INT1IF = 0;                         // Reset INT1 interrupt flag
   _INT1IE = 1;                         // Enable INT1 Interrupt Service Routine

}
// Configura Interrupt Externo INT1 - Capta Impulsos do Encoder Output B - Pino 23
inline void INT2config(void) {

   INTCON1 = 0;                         // Setup INT2 interupt on falling edge
   
   _INT2IF = 0;                         // Reset INT2 interrupt flag
   _INT2IE = 1;                         // Enable INT2 Interrupt Service Routine

}
// Determina nº de Impulsos em 100ms para as duas Rodas (Interrupcao gerada pelo Timer1 do modulo)
void _ISR _CCT1Interrupt(void) {

    count = pulses;                     // Passa nova contagem - N Impulsos em 100ms

    calculate = 1;                      // Manda calcular o Deslocamento e Velocidade

    if (pulses>24)                      // Ao 25º impulso representa 0º "reset a contagem"
        pulses = 0;

    _CCT1IF = 0;                        // Disable interrupt of CCP Timer3
}
//
void _ISR _INT1Interrupt(void){

    _INT1 = 1;
    if(_INT2) {                         // Se já ocorreu o 2º interrupt Direccao p/ Frente
      
        pulses--;
        _INT1 = 0;
        _INT2 = 0;
    }else {
        pulses++;
    }

    _INT1IF = 0;                         // Clear the INT1 interrupt flag
}
void _ISR _INT2Interrupt(void){
     
    _INT2 = 1;                                
    if(_INT1) {                          // Se já ocorreu o 1º interrupt Direccao p/ Frente
       
        pulses++;
        _INT1 = 0;
        _INT2 = 0;
    }else {
        pulses--;
    }

    _INT2IF = 0;                         // Clear the INT2 interrupt flag
}
//
int main(void) {


    CLKconfig();
    IOconfig();
    CCP1config();
    INT1config();
    INT2config();

    UART1Init();

    printf("Posicao Incremental do Enconder\n");

    int n = 15;                                             // 1 Impulso = 15º (24 counts per revolution)
    int pos, aux_pos = 0;
    char dir;

    do{

        // Calcula Deslocamento Incremental
        if(calculate){

            pos = n*count;  

            if (pos>360 || pos < -360)
                count = 0;

            if(aux_pos < pos)
                dir = '+';
            else if (aux_pos > pos)
                dir = '-';
            else
                dir = '0';

            printf("Posicao Relativa: %dº Direccao: %c\n", pos, dir);

            aux_pos = pos;                                  // Guarda ultima posicao
            calculate = 0;
        }
        
    }while(1);

    return 0;
}
