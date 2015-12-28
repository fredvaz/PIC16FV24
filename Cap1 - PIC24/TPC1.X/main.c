/*
 * File:   main.c
 * Author: fredericovaz
 *
 * Created on October 22, 2015, 1:50 PM
 */


#include "xc.h"
#include "config.h" 
#include <libpic30.h>


#define BAUDRATE 19200U
#define BRG 51


// Configurar oscilador para 32MHz
inline void ConfigCLK() {
    
    CLKDIVbits.DOZE = 0;    // 1:1
    CLKDIVbits.RCDIV = 0;   // 8 MHz
}
// Configaracao dos Pinos
inline void ConfigIO() {

    ANSA = 0x00;            // Porto A digital
    TRISAbits.TRISA1 = 1;   // Entrada Botao Aumenta
    TRISAbits.TRISA2 = 1;   // Entrada Botao Diminui

    ANSB = 0x0000;          // Porto B digital     
    TRISBbits.TRISB15 = 0;  // Saida LED

}

int main() {

	ConfigCLK();
	ConfigIO();

    int _resolution = 8000; // Time
    int _tcycle = 80;
    
    int on = 0, off = _resolution;  // Tempo em segundos do LED ON e OFF (Dutty Cycle)

	do{

		PORTBbits.RB15 = 1; // RB15 1
		__delay_us(on);
		PORTBbits.RB15 = 0; // RB15 0
		__delay_us(off); 


        if (PORTAbits.RA1 && on <= _resolution && off >=0) // Aumenta o brilho - Aumenta o Tempo Ligado (certificamo-nos que não ultrapasaa 1000us)
           on += _tcycle; off -= _tcycle;
    

        if (PORTAbits.RA2 &&  on >= 0 && off <= _resolution) // Diminui o brilho - Diminui o Tempo Ligado, o resto do tempo passa Desligado
            on -= _tcycle; off += _tcycle;

    
    }while(1);

    return 0;
}











