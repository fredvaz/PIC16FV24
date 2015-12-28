/* 
 * File:   Ton.c
 * Author: Lino
 *
 * Created on 6 de Outubro de 2014, 11:37
 *
 * Programa para medir o tempo que um botсo ligado a RA1 ж premido
 */
 
#include <xc.h>
#include "config.h"
#include <stdio.h>
 
#define Tmin 3
 
/*
 * Prototipos de funушes
 */
inline void ConfigCLK(void);
inline void ConfigIO(void);
inline void UART1Init(unsigned long int brg);
inline void ConfigTMR1(void);
 
enum estados{ESPERA,PREMIDO,RELEASE,COMM};
 
int main(void) {
 
    ConfigCLK();
    ConfigIO();
    UART1Init(19200);
 
    ConfigTMR1();
 
    printf("\nStart");
 
    enum estados estado=ESPERA;
    unsigned int time, lastTime, tempo, tOn;
    while(1)
    {
        time = TMR1;
        if(time<lastTime){  // TMR1 rollover
            tempo++;        // Acumulador de centesimas de segundo
        }
        lastTime=time;
 
        switch(estado){
                case ESPERA:
                    if(!PORTAbits.RA1){
                        TMR1 = 0;
                        tempo = 0;  // Inicia temporizacao
                        estado = PREMIDO;
                    }
                    break;
                case PREMIDO:
                    if(PORTAbits.RA1){ // filtra transitorios
                        if(tempo<Tmin)
                            estado = ESPERA;
                        else{
                            tOn=tempo;
                            estado = RELEASE;
                            }
                    }
                    break;
                case RELEASE:
                    if((tempo-tOn)<Tmin){
                        if(!PORTAbits.RA1) // filtra transitorios
                            estado = PREMIDO;
                    }
                    else
                        estado = COMM;
                     
                    break;
                case COMM:
                    tempo = tOn/100; // Calcula segundos
                    tOn %= 100;      // Calcula centesimas
                    printf("\r%u.%u s", tempo, tOn);
                    estado=ESPERA;
        }
    }
    return (1);
}
 
inline void ConfigCLK(void)
{
    //configurar oscilador para 32MHz
    CLKDIVbits.DOZE = 0;    // 1:1
    CLKDIVbits.RCDIV = 0;   // 8 MHz
}
 
inline void ConfigIO(void)
{
    ANSA = 0;               // Porto A digital
    TRISAbits.TRISA1 = 1;   // entrada (botao)
    TRISAbits.TRISA0 = 0;   // saida (LED da Microstick)
    ANSB = 0;               // U1RX e U1TX digitais
    TRISBbits.TRISB2 = 1;   // entrada (U1RX)
    TRISBbits.TRISB7 = 0;   // saida (U1TX)
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
    //IEC0bits.U1TXIE = 1;        // enable U1TX ISR
}
 
inline void ConfigTMR1(void)
{
    T1CON = 0x8030; // T1 ON; :256
    PR1 = 625-1;    //10 ms
}