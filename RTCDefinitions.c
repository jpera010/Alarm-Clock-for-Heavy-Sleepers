#include "RTCDeclarations.h"

void Init() {
	TWBR = BTR(TWSR = 0x00);
}	

char Start(char DestAddr) {
	char progress;
	
	TWCR = (1<<TWSTA)|(1<<TWEN)|(1<<TWINT);
	while (!(TWCR & (1<<TWINT)));
	progress = TWSR & 0xF8;
	
	if (progress != 0x08)	{	
		return 0;
	}
	
	TWDR = DestAddr;
	TWCR = (1<<TWEN)|(1<<TWINT);
	while (!(TWCR & (1<<TWINT)));
	
	progress = TWSR & 0xF8;	
	
	if (progress == 0x18)	{
		return 1;
	}			

	if (progress == 0x20) {									
		return 2;
	}												
	else {
		return 3;
	}										
}

char Start2(char SrcAddr) {
	char progress;	
			
	TWCR = (1<<TWSTA)|(1<<TWEN)|(1<<TWINT);				
	while (!(TWCR & (1<<TWINT)));					
	progress = TWSR & 0xF8;			
						
	if (progress != 0x10)	{								
		return 0;
	}
	
	TWDR = SrcAddr;									
	TWCR = (1<<TWEN)|(1<<TWINT);							
	while (!(TWCR & (1<<TWINT)));	
							
	progress = TWSR & 0xF8;	
									
	if (progress == 0x40) {										
		return 1;
	}
	if (progress == 0x20)	{									
		return 2;			
	}
	else {
		return 3;
	}
}

void Stop()	{
	TWCR=(1<<TWSTO)|(1<<TWINT)|(1<<TWEN);
	while(TWCR & (1<<TWSTO));
}

void Start3(char DestAddr) {
	
	char progress;										
	while (1) {
		TWCR = (1<<TWSTA)|(1<<TWEN)|(1<<TWINT);				
		while (!(TWCR & (1<<TWINT)));	
						
		progress = TWSR & 0xF8;
									
		if (progress != 0x08) {						
			continue;
		}
												
		TWDR = DestAddr;							
		TWCR = (1<<TWEN)|(1<<TWINT);					
		while (!(TWCR & (1<<TWINT)));	
		
		progress = TWSR & 0xF8;			
					
		if (progress != 0x18 ) { 
			Stop();									
			continue;									
		}
		
		break;											
	}
}

char Write(char data){
	char progress;	
											
	TWDR = data;										
	TWCR = (1<<TWEN)|(1<<TWINT);						
	while (!(TWCR & (1<<TWINT)));		
					
	progress = TWSR & 0xF8;		
								
	if (progress == 0x28) {									
		return 0;			
	}
	if (progress == 0x30){										
		return 1;			
	}
	else {
		return 2;
	}
}

char ReadWrite1()	{
	TWCR=(1<<TWEN)|(1<<TWINT)|(1<<TWEA);				
	while (!(TWCR & (1<<TWINT)));						
	return TWDR;	
}	

char ReadWrite2() {
	TWCR=(1<<TWEN)|(1<<TWINT);		
	while (!(TWCR & (1<<TWINT)));							
	return TWDR;										
}

bool CheckAMPM(char hr) {
	if(hr & (ChooseAMorPM)) {
		return 1;
	}
	else {
		return 0;
	}
}

bool AlarmCheckAMPM(char AlarmHr) {
	if(AlarmHr & (ChooseAMorPM)) {
		return 1;
	}
	else {
		return 0;
	}
}

void RdClck(char GetClkRead) {
	Start(WriteTo);				
	Write(GetClkRead);			
	Start2(ReadFrom);	

	second = ReadWrite1();	
	minute = ReadWrite1();	
	hour = ReadWrite2();		
	Stop();				
}

void RdCal(char GetCalRead) {
	Start(WriteTo);
	Write(GetCalRead);
	Start2(ReadFrom);

	day = ReadWrite1();				
	date = ReadWrite1();					
	month = ReadWrite1();					
	year = ReadWrite2();							
	Stop();								
}

void ClockSet(char hr, char min, char sec, char AMORPM) {
	Start(WriteTo);
	Write(0);
	Write(sec);
	Write(min);
	hr |= AMORPM;
	Write(hr);
	Stop();
}

void CalSet(char day, char m, char date, char y) {
	Start(WriteTo);
	Write(3);
	Write(day);
	Write(m);
	Write(date);
	Write(y);
	Stop();
}	