/*
 * File:   main.c
 * Author: João
 *
 * Created on 27 de Novembro de 2015, 18:47
 */


#include "xc.h"
#include "config.h" 
#include <libpic30.h> //tem a funcao delay

#define BAUDRATE 19200U
#define BRG 51

inline void UART1Init(void) {

 	U1BRG = BRG;
 	U1MODE = 0x8000;
 	U1STA = 0x0400;
}

int main(void) {
    
    UART1Init();
    
    printf("start\n");
    
    while(1){
        __delay_ms(1e3);
        printf("oi\n");
    
    }
    return 0;
}
