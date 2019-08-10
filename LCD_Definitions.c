#include "LCD_Declarations.h"

void LCDDir(unsigned char cmd) {
	LCDData = (LCDData & 0x0f)|(cmd & 0xf0); //used to be (LCDData & 0x0f)
	LCDData &= ~ (1<<LCD_RS);
	LCDData |= (1<<LCD_EN);	
	_delay_us(1);
	LCDData &= ~(1<<LCD_EN);
	_delay_us(100);	
	
	LCDData = (LCDData & 0x0f)|(cmd << 4); //used to be (LCDData & 0x0f)
	LCDData |= (1<<LCD_EN);	
	_delay_us(1);	
	LCDData &= ~(1<<LCD_EN);
	_delay_us(2000);
}

void LCDSend(unsigned char data) {
	LCDData = (LCDData & 0x0f)|(data & 0xf0); //used to be (LCDData & 0x0f)
	LCDData |= (1<<LCD_RS);					
	LCDData |= (1<<LCD_EN);						
	_delay_us(1);								
	LCDData &= ~(1<<LCD_EN);					
	_delay_us(100);						
	
	LCDData = (LCDData & 0x0f)|(data << 4);	//used to be (LCDData & 0x0f)	
	LCDData |= (1<<LCD_EN);					
	_delay_us(1);				
	LCDData &= ~(1<<LCD_EN);				
	_delay_us(2000);					
}

void LCDStart() {
	LCDRegs = 0xFF;
	_delay_ms(200);								
	LCDDir(0x33);
	LCDDir(0x32);							
	LCDDir(0x28);							
	LCDDir(0x0C);					
	LCDDir(0x01);						
	LCDDir(0x82);			
}

void LCDPrint(char *str) {
	unsigned char i=0;
	
	while (str[i] |= 0) {
		LCDSend(str[i]);
		i++;
	}
}


void LCDPrintSpot(char row, char pos, char* str) {
	if (row == 0 && pos<16) {
		LCDDir((pos & 0x0F)|0x80);
	}
	else if (row == 1 && pos<16) {
		LCDDir((pos & 0x0F)|0xC0);
	}
	LCDPrint(str);
}

void LCDErase() {
	LCDDir(0x01);
	_delay_ms(2);
}