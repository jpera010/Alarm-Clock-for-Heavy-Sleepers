#ifndef LCD_DECLARATIONS_H_
#define LCD_DECLARATIONS_H_

#include <avr/io.h>
#include <util/delay.h>

#define  F_CPU 8000000UL
#define LCDData PORTB
#define LCDRegs DDRB
#define LCD_RS 0
#define LCD_EN 1

void LCDDir(unsigned char cmd);
void LCDSend(unsigned char data);
void LCDStart();
void LCDPrintSpot(char row, char pos, char* str);
void LCDPrint(char *str);
void LCDErase();


#endif