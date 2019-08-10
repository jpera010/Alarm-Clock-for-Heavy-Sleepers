#include "RTCDefinitions.c"
#include "LCD_Definitions.c"
#include "bit.h"
#include "timer.h"
#include "pwm.h"
#include <util/delay.h>
#include <avr/eeprom.h>

enum States {StartSM, DisplayCalendar, SetAlarmHr, AlarmOff, SelectHrsOn, SelectHrsOff,
			 SetAlarmMins, SelectMinsOn, SelectMinsOff, SetAMPM, DispAM, DispPM, FinalAlarm} state;

//variables for alarm clock tick function
int AlarmAM = 0x40;
#define AlarmPM 0x60
char AlarmAMorPM = 0x00;
char message[20];
unsigned char cntr = 0;
unsigned char cnt2 = 0;
unsigned char Soundtickflag = 0;
unsigned long tick_elapsedTime = 20;

int EEMEM hrs = 0x01;
int EEMEM mins = 0x00;
int EEMEM AM_PM_Value = 0x40; 
unsigned char MinSMCounter = 0;
unsigned char HR_SMCounter = 0;
	
void tick() {
	switch(state) { //transitions
		case StartSM:
		state = DisplayCalendar;
		break; 
		
		case DisplayCalendar:
		if(GetBit(PINA,0) == 1) {
			state = DisplayCalendar; 
		}
		else if(GetBit(PINA,0) == 0) {
			state = SetAlarmHr;
		}
		break; 
		
		case SetAlarmHr:
		if(GetBit(PINA,1) == 1) {
			state = SetAlarmHr; 
		}
		else if(GetBit(PINA,1) == 0) {
			state = SelectHrsOn;	
		}
		else if(GetBit(PINA,0) == 0) {
			state = AlarmOff;
		}
		break;
		
		case AlarmOff:
		if(cntr == 4) {
			state = DisplayCalendar;
		}
		else {
			state = AlarmOff;	
		}
		break;
		
		case SelectHrsOn:
		 if(GetBit(PINA,0) == 0) {
			state = AlarmOff;
		}
		else {
			state = SelectHrsOff;
		}
		break;
		
		case SelectHrsOff:
		if(GetBit(PINA,2) == 0){
			state = SelectHrsOn;
		}
		else if(GetBit(PINA,2) == 1) {
			state = SelectHrsOff;
		}
		else if(GetBit(PINA,0) == 0) {
			state = AlarmOff;
		}
		else if(GetBit(PINA,1) == 0) {
			state = SetAlarmMins;
		}
		break;
		
		case SetAlarmMins:
		if(GetBit(PINA,1) == 1) {
			state = SetAlarmMins;
		}
		else if(GetBit(PINA,1) == 0) {
			state = SelectMinsOn;	
		}
		else if(GetBit(PINA,0) == 0) {
			state = AlarmOff; 
		}
		break;
		
		case SelectMinsOn:
		if(GetBit(PINA,0) == 0) {
			state = AlarmOff;
		} 
		else {
			state = SelectMinsOff;
		}
		break;
		
		case SelectMinsOff:
		if(GetBit(PINA,2) == 1) {
			state = SelectMinsOff;
		}
		else if(GetBit(PINA,2) == 0) {
			state = SelectMinsOn;
		}
		else if(GetBit(PINA,1) == 0) {
			state = SetAMPM;	
		}
		else if(GetBit(PINA,0) == 0) {
			state = AlarmOff;
		}
		break;
		
		case SetAMPM:
		if(GetBit(PINA,1) == 1) {
			state = SetAMPM;	
		}
		else if((eeprom_read_byte(&AM_PM_Value) == AlarmAM) && (GetBit(PINA,1) == 0)) {
			state = DispAM;	
		}
		else if(eeprom_read_byte(&AM_PM_Value) == AlarmPM && (GetBit(PINA,1) == 0)){
			state = DispPM;	
		}
		else if(GetBit(PINA,0) == 0) {
			state = AlarmOff;	
		}
		break;
		
		case DispAM:
		if(GetBit(PINA, 1) == 0){
			state =FinalAlarm;	
		}
		else if(GetBit(PINA,2) == 1) {
			state = DispAM;
		}
		else if(GetBit(PINA,2) == 0) {
			state = DispPM;	
		}
		else if(GetBit(PINA,0) == 0) {
			state = AlarmOff;
		}
		break;
		
		case DispPM:
		if(GetBit(PINA,2) == 1) {
			state = DispPM;	
		}
		else if(GetBit(PINA,2) == 0) {
			state = DispAM;
		}
		else if(GetBit(PINA,1) == 0) {
			state = FinalAlarm;
		}
		else if(GetBit(PINA,0) == 0) {
			state = AlarmOff;
		}
		break;
		
		case FinalAlarm:
		if (cnt2 == 10) {
			state = DisplayCalendar;
		}
		else {
			state = FinalAlarm;
		}
		break;
		
		default:
		state = StartSM; 
		break;
			
	} //transitions
	
	switch(state) { ///actions
		case StartSM:
		break; 
		
		case DisplayCalendar:
		RdCal(3);
		sprintf(arr, "%02x/%02x/%02x %3s    ", date, month, year,week[day-1]);
		LCDPrintSpot(1,0,arr);
		cntr = 0;
		break;
		
		case SetAlarmHr:
		sprintf(message, "Set Alarm Hour  ");
		LCDPrintSpot(1,0,message);
		HR_SMCounter = 0;
		break;
		
		case AlarmOff:
		cntr = cntr+1;
		sprintf(message, "Alarm Off       ");
		LCDPrintSpot(1,0,message);
		eeprom_write_byte(&hrs,0x01);
		eeprom_write_byte(&mins,0x00);
		break;
		
		case SelectHrsOn:
		if(HR_SMCounter == 0) {
			HR_SMCounter++;	
		}
		else if(HR_SMCounter != 0 && (eeprom_read_byte(&hrs) + 0x01) < 0x13){
			if((eeprom_read_byte(&hrs) + 0x01) == 0x0A) {
				eeprom_write_byte(&hrs,0x10);
			}
			else {
			eeprom_write_byte(&hrs,(eeprom_read_byte(&hrs) + 0x01));
			}
		}
		else{
			eeprom_write_byte(&hrs,0x01);
		}
		break;
		
		case SelectHrsOff:
			sprintf(message, "%02x:            ", eeprom_read_byte(&hrs));
			LCDPrintSpot(1,0,message);	
		break;
		
		case SetAlarmMins: 
		sprintf(message, "Set Alarm Minute");
		LCDPrintSpot(1,0,message);
		MinSMCounter = 0;
		break;	
		
		case SelectMinsOn:
		if(MinSMCounter == 0) {
			MinSMCounter++;
		}
		else if(MinSMCounter != 0 && (eeprom_read_byte(&mins) + 0x01) < 0x5A){
			if((eeprom_read_byte(&mins) + 0x01) == 0x0A) {
				eeprom_write_byte(&mins,0x10);
			}
			else if ((eeprom_read_byte(&mins) + 0x01) == 0x1A) {
				eeprom_write_byte(&mins,0x20);
			}
			else if((eeprom_read_byte(&mins) + 0x01) == 0x2A) {
				eeprom_write_byte(&mins,0x30);
			}
			else if((eeprom_read_byte(&mins) + 0x01) == 0x3A) {
				eeprom_write_byte(&mins,0x40);
			}
			else if((eeprom_read_byte(&mins) + 0x01) == 0x4A) {
				eeprom_write_byte(&mins,0x50);
			}
			else {
				eeprom_write_byte(&mins,(eeprom_read_byte(&mins) + 0x01));
			}
		}
		else{
			eeprom_write_byte(&mins,0x00);
		}	
		break;
		
		case SelectMinsOff:
		sprintf(message, "%02x:%02x           ", eeprom_read_byte(&hrs), eeprom_read_byte(&mins));
		LCDPrintSpot(1,0,message);
		break;
		
		case SetAMPM:
		sprintf(message, "Set AM or PM    ");
		LCDPrintSpot(1,0,message);
		break;
		
		case DispAM:
		sprintf(message, "%02x:%02x     ", eeprom_read_byte(&hrs), eeprom_read_byte(&mins));
		strcat(message, "AM");
		LCDPrintSpot(1,0,message);
		eeprom_write_byte(&AM_PM_Value, AlarmAM);
		cnt2 = 0;
		break;
		
		case DispPM:
		sprintf(message, "%02x:%02x     ", eeprom_read_byte(&hrs), eeprom_read_byte(&mins));
		strcat(message, "PM");
		LCDPrintSpot(1,0,message);
		eeprom_write_byte(&AM_PM_Value, AlarmPM);
		cnt2 = 0;
		break;
		
		case FinalAlarm:
		sprintf(message, "Alarm Set       ");
		LCDPrintSpot(1,0,message);
		++cnt2;
		_delay_ms(250);
		Soundtickflag = 1;
		break;
		
		default:
		break;
			
	} ///actions
}

/////////////speaker sm ////////////////////
enum SoundState {SoundStart, SoundInit, SoundPM, SoundAM, Snooze, ButtonHold, AlarmShutDown} Soundstate;
unsigned int i = 0;

void Soundtick() {
	
	switch(Soundstate) { //transitions
		case SoundStart:
		Soundstate = SoundInit;
		break;
		
		case SoundInit:
		if((eeprom_read_byte(&hrs) == (hour & 0b00011111)) && (eeprom_read_byte(&mins) == minute) && CheckAMPM(hour) && (AlarmPM == eeprom_read_byte(&AM_PM_Value))) {
				Soundstate = SoundPM;
		}
		else if ((eeprom_read_byte(&hrs) == (hour & 0b00011111)) && (eeprom_read_byte(&mins) == minute) && (!CheckAMPM(hour)) && (AlarmAM == eeprom_read_byte(&AM_PM_Value))) {
			Soundstate = SoundAM;	
		}
		else {
			Soundstate = SoundInit;
		}
		break;
		
		case SoundPM:
		if( (eeprom_read_byte(&hrs) == (hour & 0b00011111)) &&
		(eeprom_read_byte(&mins) == minute) &&
		CheckAMPM(hour) && 
		(AlarmPM == eeprom_read_byte(&AM_PM_Value))
		&& GetBit(PINA,3) == 1) {
				Soundstate = SoundPM;
		}
		else if(GetBit(PINA,7) == 0 && GetBit(PINA,3) == 0 ){
				Soundstate = Snooze;
				
		}
		break;
		
		case SoundAM:
		if ((eeprom_read_byte(&hrs) == (hour & 0b00011111)) && (eeprom_read_byte(&mins) == minute) && (!CheckAMPM(hour)) && (AlarmAM == eeprom_read_byte(&AM_PM_Value)) && GetBit(PINA,3) == 1) {
			Soundstate = SoundAM;	
		}
		else if(GetBit(PINA,7) == 0 && GetBit(PINA,3) == 0) {
			Soundstate = Snooze; 	
		}
		break;
		
		case Snooze:
		if(i >= 500 && GetBit(PINA,7) == 0 && GetBit(PINA,3) == 0){
			Soundstate = AlarmShutDown;	
		}
		else if (GetBit(PINA,7) == 0 && GetBit(PINA,3) == 1) {
			Soundstate = SoundInit;	
		}
		break;
		
		case AlarmShutDown:
			set_PWM(0);
			eeprom_write_byte(&hrs, 0x01);
			eeprom_write_byte(&mins,0x00);
			eeprom_write_byte(&AM_PM_Value,AlarmAM);
			Soundtickflag = 0; 
			Soundstate = SoundInit;
		break;
		
		default:
		Soundstate = SoundStart;
		break;
	}
	
	switch(Soundstate){ //actions
		case SoundStart:
		break;
		
		case SoundInit:
		set_PWM(0);
		i = 0;
		break;
		
		case SoundPM:
		set_PWM(261.63);
		i = 0;
		break;
		
		case SoundAM:
		set_PWM(261.63);
		i = 0;
		break;
		
		case Snooze:
		i++;
		break;
		
		default:
		break;
	}
}
/////////////////////////////speaker sm above////////////////////////////////////////

int main(void) {
	DDRA = 0x00; //input
	PORTA = 0xFF; //input
	DDRD = 0xFF; //output
	PORTD = 0x00; //output
	
	Init();
	//ClockSet(0x11, 0x17, 0x30, AM); //need to change this every time I reprogram the atmega
	//CalSet(0x04, 0x06, 0x06, 0x18); // need to change this every time I reprogram the atmega 
	//(char day, char month, char date, char y)
	//also, need to comment it out so when device is turned off, time does not reset
	LCDStart();
	
	TimerSet(2);
	TimerOn();
	PWM_on();

    while(1) {
		RdClck(0);
		sprintf(arr, "%02x:%02x:%02x  ", (hour & 0b00011111), minute, second);
		
		if(CheckAMPM(hour)) {
			strcat(arr, "PM");
		}
		else {
			strcat(arr, "AM");
		}
		LCDPrintSpot(0,0,arr);
		
		if(GetBit(PINA,7) == 0) {
			PORTD = 0x01;	
		}
		else {
			PORTD = 0x00;	
		}
		
		if (tick_elapsedTime >= 20) {
			tick(); // Execute one tick of the synchSM
			tick_elapsedTime = 0;          
        }
		if(Soundtickflag == 1) {
			Soundtick();
		}
		while (!TimerFlag);
		TimerFlag = 0;
		tick_elapsedTime +=1;
    }
}