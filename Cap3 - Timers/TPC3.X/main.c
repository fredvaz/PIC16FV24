/*
 * File:   main.c
 * Author: Frederico Vaz & João Freitas
 *
 * Created on October 22, 2015, 1:50 PM
 */

#include <xc.h>
#include "config.h"
#include <stdio.h>
#include <libpic30.h>
#include <stdbool.h>
 
#define Tmin 1


enum estados{ESPERA,PREMIDO,RELEASE,CONTADOR};


inline void ConfigCLK(void)
{
    //configurar oscilador para 32MHz
    CLKDIVbits.DOZE = 0;    // 1:1
    CLKDIVbits.RCDIV = 0;   // 8 MHz
}
 
inline void ConfigIO(void)
{
    ANSB = 0;               // Porto A digital
    TRISBbits.TRISB10 = 1;   // entrada (botao)
    TRISBbits.TRISB11 = 1;   // entrada (LED da Microstick)             // U1RX e U1TX digitais
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
    PR1 = 6250-1;    //100 ms
}




int main(){

    ConfigCLK();
    ConfigIO();
    UART1Init(19200);
 
    ConfigTMR1();
 
    printf("\nStart");
 
    //  enum estados estado=ESPERA;
    unsigned int time, lastTime, tOn;
    unsigned int tempo = 0, conta = 0;
    bool contar= false;
    enum estados estado=ESPERA;
    
    while(1){
       time = TMR1;
        if(time<lastTime){  // TMR1 rollover
            tempo++;        // Acumulador de centesimas de segundo
        }
        
        switch(estado){
                case ESPERA:
                    contar=false;
                    if(!PORTBbits.RB10){
                        TMR1 = 0;
                        tempo = 0;  // Inicia temporizacao
                        estado = PREMIDO;
                    }
                    if(PORTBbits.RB11){
                        conta = 0;
                    }
                    break;
                case PREMIDO:
                    if(PORTBbits.RB10){ // filtra transitorios
                        if(tempo<Tmin)
                            estado = ESPERA;
                        else{
                            estado = RELEASE;
                            }
                    }
                    break;
                case RELEASE:
                    if((tempo-tOn)<Tmin){
                        if(!PORTBbits.RB10) // filtra transitorios
                            estado = PREMIDO;
                    }
                    else{
                        if(contar)
                            estado = ESPERA;
                        else
                            estado = CONTADOR;                            
                    }
                    break;
                case CONTADOR:
                    tOn= tOn/100+tOn%10; // Calcula segundos e centesimas
                    if(time<lastTime){
                        conta++;
                        printf("\r%u.%u s", conta/10, conta%10);
                    }
                    contar=true;
                    if(!PORTBbits.RB10){
                        TMR1 = 0;
                        tempo = 0;  // Inicia temporizacao
                        estado = PREMIDO;
                    }
                    break;
        }
        
        lastTime=time;
    }
    
    
 return 0;   
}
     