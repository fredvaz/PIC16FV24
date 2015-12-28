/*
 * File:   main.c
 * Author: FredVaz
 * 
 * Cronometro
 * 
 * Created on October 7, 2015, 4:21 PM
 */

#include <xc.h>
#include "config.h" 
#include <libpic30.h>
#include <stdio.h>
#include <stdbool.h>



#define BAUDRATE 19200U
#define BRG 51 

// 0 ... 9 Decimal
unsigned char *number[10] = { 0xC000, 0xF900, 0xA400, 0xB000, 0x9900, 0x9200, 0x8200, 0xF800, 0x8000, 0x9000};
	
int freq = 1; // Taxa de actualização dos digitos
char c;

bool on = false;
	
int D4=0, D3=0, D2=0, D1=0, i=0;


inline void UART1Init() {

 	U1BRG = BRG;
 	U1MODE = 0x8000;
 	U1STA = 0x0400;
 }
// wait for a new character to arrive to the UART1 serial port
char getChar() {

    if(U1STAbits.URXDA)
 		return U1RXREG;
 	return '0';

}

void listening() {

	// Verifica cmd's vindos da porta serial
	c = getChar();

	if (c == 'S'){
		on = true;
	}
	if (c == 'P'){
		on = false;
	}
	if (c == 'R'){
		D4=0; D3=0; D2=0; D1=0; i=0;
	}
}

void refresh() {

	PORTA = 0x00E;
    PORTB = number[D4];
    __delay_ms(freq);
    
    PORTA = 0x00D;
    PORTB = number[D3];
    __delay_ms(freq);
    
    PORTA = 0x00B;
    PORTB = number[D2];
    __delay_ms(freq);
    
    PORTA = 0x007;
    PORTB = number[D1];
    __delay_ms(freq);

}

int main() {

	CLKDIVbits.DOZE = 0; 		// 1:1  
	CLKDIVbits.RCDIV = 0;		// 8MHz

	ANSA=0;         	// Configura Port A para digital
	ANSB=0;         	// Configura Port B para digital
	TRISA = 0; 			// PORTA tudo como saida, RB7 = U1TX saÃ­da
	TRISB = 0x0004;    	// PORTB como saída menos a RB2 = U1RX entrada
    
	UART1Init();

	printf("\r\nStart");
    
	do{

       	// Algortimo Cronometro - Utilizamos um ciclo for porque no Cap2 ainda nÃ£o abordamos Timers

	    for(i=0; i < 250; i++){ // Ao fim de 250 iter temos: 250x4ms(4 digitos*freq) = 1s
	       	
	       	listening(); // Escuta comandos da Serial Port
	       	
	       	refresh();	// Mantêm digitos actualizados e faz o varreamento

	    }
	    if (on){ // Start and Stop
	    
		    D4++; // t < 9s

		    if (D4 > 9){ 

		    	D4=0;
		    	D3++; // 00:09 < t < 00:59 (D4 a 9 - incrementa D3)

		    	if (D3 > 5){ 
		    		
		    		D3=0;
		    		D2++; // 01:00 < t < 60:00 (D3 a 6 - incrementa D2)

		    		if (D2 > 8){

		    			D2=0;
		    			D1++;

		    			if(D1 == 6){
		    				on = false; // Para em 60:00
		    			}
		    		}
		    	}
		    }
		}
     
	}while(1); // Ciclo infinito a correr o programa 
}








