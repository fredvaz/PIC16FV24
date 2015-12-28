/*
 * File:   main.c
 * Author: FredVaz
 * 
 * KeyBoard 
 *
 * Created on October 16, 2015, 9:15 PM
 */

#include <xc.h>
#include "config.h" 
#include <libpic30.h>
#include <stdio.h>
#include <stdbool.h>



#define BAUDRATE 19200U
#define BRG 51



// 0 ... F Hexadecimal
unsigned char *number[] = { 0xC000, 0xF900, 0xA400, 0xB000, 0x9900, 0x9200, 0x8200, 0xF800, 0x8000, 0x9000, 0x8800, 0x8300, 0xC600, 0xA100, 0x8600, 0x8E00};

bool S1 = false, S2 = false, S3 = false, S4 = false, on = false;

// Inicia conexao com a Porta Serial
inline void UART1Init() {

 	U1BRG = BRG;
 	U1MODE = 0x8000;
 	U1STA = 0x0400;
 }
// wait for a new character to arrive to the UART1 serial port
char getChar() {

    if(U1STAbits.URXDA)
 		return U1RXREG;
 	return '\0';
}
// Fica a espera de comandos via Porta Serial
bool listening() {

	// Verifica cmd's vindos da porta serial
	char c = getChar();

	if (c == '1')
		return 1;
	if(c != '1' && c != '\0')
		return 0;

	return on; // Garante que não altera o estado do ponto
}
// Le estado dos Botoes
void _read() {

	// 1ª linha on c/ a 2a off
	PORTA = 0x0E;

	__delay_ms(50); // Bounce - Solucao nao ideal
	if(!PORTAbits.RA3)
	 	S4 = true;

	__delay_ms(50); // Bounce
	if(!PORTAbits.RA2)
  		S3 = true;

	// 2ª linha on c/ a 1a off
	PORTA = 0x0D;

	__delay_ms(50); // Bounce
	if(!PORTAbits.RA3)
 	 	S2 = true;

 	__delay_ms(50); // Bounce
	if(!PORTAbits.RA2)
  		S1 = true;

}
// Le estado dos botoes
int _transform() {

	char byte = ~0x00; // Palavra binária 4 bits - (0000) 0000 - 0 (char 1 byte 8 bits)
	char aux_byte[4] = {~0x00, ~0x00, ~0x00, ~0x00}; // necessario inicializar para não conter lixo

	if(S4)
		aux_byte[3] = byte & ~0x08; // 1000 - bit mais significativo
	if(S3)
		aux_byte[2] = byte & ~0x04; // 0100 - ~ -> 1011 (invertido, porque no fim é um AND logico, ver rascunhos)
	if(S2)
		aux_byte[1] = byte & ~0x02; // 0010 - ~ -> 1101 (invertido)
	if(S1)
		aux_byte[0] = byte & ~0x01; // 0001 - bit menos significativo

	// Resultado Final - Combinacao de todos
	byte = aux_byte[0] & aux_byte[1] & aux_byte[2] & aux_byte[3];

	switch(byte){ // Traduz a plavra binaria num nº decimal

		case ~0x00:
			return 0;
		break;

		case ~0x01:
			return 1;
		break;

		case ~0x02:
			return 2;
		break;

		case ~0x03:
			return 3;
		break;

		case ~0x04:
			return 4;
		break;

		case ~0x05:
			return 5;
		break;

		case ~0x06:
			return 6;
		break;

		case ~0x07:
			return 7;
		break;

		case ~0x08:
			return 8;
		break;

		case ~0x09:
			return 9;
		break;

		case ~0x0A:
			return 10;
		break;

		case ~0x0B:
			return 11;
		break;

		case ~0x0C:
			return 12;
		break;

		case ~0x0D:
			return 13;
		break;

		case ~0x0E:
			return 14;
		break;

		case ~0x0F:
			return 15;
		break;
	}
}
// Program
int main(void) {
    
    CLKDIVbits.DOZE = 0; 	// 1:1  
	CLKDIVbits.RCDIV = 0;	// 8MHz

	ANSA = 0;         		// Configura Port A para digital
	TRISAbits.TRISA0 = 0; 	// Saida Linha 1
	TRISAbits.TRISA1 = 0; 	// Saida Linha 2
	TRISAbits.TRISA2 = 1;   // Entrada Coluna 2
	TRISAbits.TRISA3 = 1;   // Entrada Coluna 1
	TRISAbits.TRISA4 = 0; 	// Saida Ponto

	ANSB = 0;         		// Configura Port B para digital
	TRISB = 0x0004;    		// PORTB como saida menos a RB2 = U1RX entrada

	UART1Init();

	printf("\r\nStart");

	int i = 0;

    do{
 
  		_read(); 			// Le estados dos botoes

    	//if(S1 || S2 || S3 || S4) // Só muda o digito se algun dos botões for carregado
  			i = _transform();

  		PORTB = number[i];	// Coloca resultado no Display

  		S1 = false; S2 = false; S3 = false; S4 = false; // Reset

  		on = listening();

    	if (on)
    		PORTBbits.RB15 = 0; // Activa o unicamente o ponto
    	else
    		PORTBbits.RB15 = 1;// Desactiva o unicamente o ponto


    }while(1); // Run program forever
}















