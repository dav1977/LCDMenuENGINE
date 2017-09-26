#pragma once

#include "CoreSettings.h"
#include "Types.h"
	extern void SYSTICK(void);

	 typedef struct
    {
        uint ct;			//Переменная для отсчета таймера
				uint time;    //время отсчета до включения таймера
        bool out;   
        bool reset;  	//сброс ct			
		} SOFT_TIMER_TON;//ТАЙМЕР С ЗАДЕРЖКОЙ ВКЛЮЧЕНИЯ

		
//--- system timers
extern   SOFT_TIMER_TON t1;
extern   SOFT_TIMER_TON t2;
extern   SOFT_TIMER_TON t3;
extern   SOFT_TIMER_TON 	MRFtimer;
extern   SOFT_TIMER_TON OUTPUTtimer;
extern	SOFT_TIMER_TON TASKtimer;
extern		SOFT_TIMER_TON LCDtimerPRINT;
extern		SOFT_TIMER_TON LCDtimerWRITE;
		
class CLASStimers
{
public:
	bool out;
	bool changed;
	uint time;
	
	CLASStimers();
	void reset();
	void tick();
	void set(uint t);
	uint ct;

private:	
	bool last_out;
	
};


//####################### user timers ###############
	extern CLASStimers usertimer[];
extern CLASStimers systimer[];
//####################### user timers ###############





