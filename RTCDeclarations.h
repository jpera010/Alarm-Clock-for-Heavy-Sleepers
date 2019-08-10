#ifndef RTCDECLARATIONS_H_				
#define RTCDECLARATIONS_H_

////libraries used below /// 
#include <avr/io.h>
#include <util/delay.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>


/// all of the definitions and variables used ///////////////////
#define F_CPU 8000000UL //atmega freq
#define SCL_CLK 100000L	 //scl freq
#define BTR(TWSR)	((F_CPU/SCL_CLK)-16)/(2*pow(4,(TWSR&((1<<TWPS0)|(1<<TWPS1))))) //Bitrate
#define WriteTo	0xD0 
#define ReadFrom 0xD1
#define ChooseAMorPM 0x20
#define AM 0x40
#define PM 0x60
int second,minute,hour,day,date,month,year;
char arr[20];
char* week[7]= {"Sun","Mon","Tue","Wed","Thu","Fri","Sat"};


//function declarations ///////////////
void Init();
char  Start(char DestAddr);
char  Start2(char SrcAddr);
void Stop();
void Start3(char DestAddr);
char  Write(char data);
char ReadWrite1(); 
char ReadWrite2();
bool CheckAMPM(char hr);
bool AlarmCheckAMPM(char AlarmHr);
void RdClck(char GetClkRead);
void RdCal(char GetCalRead);
void ClockSet(char hr, char min, char sec, char AMORPM);
void CalSet(char day, char date, char m, char y);


#endif