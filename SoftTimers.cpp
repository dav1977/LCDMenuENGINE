#include "SoftTimers.h"

//####################### user timers ###############
	#define MAXuserTIMERS 30
	#define MAXsystemTIMERS 30
//####################### user timers ###############
	
void CLASStimers::set(uint t)
{
  this->time=t; this->ct=0; this->out=false;
}
	
void CLASStimers::reset()
{
	this->ct=0; this->out=false;
	this->changed=false;
}

void CLASStimers::tick()
{
	if (this->time==0) return;
	if (this->ct > this->time)
		{ 
			this->out=true;  
			if (this->last_out != this->out)this->changed=true;
			this->last_out=this->out;
			return;
		}
	this->last_out=this->out;
	this->ct++; 
}

CLASStimers::CLASStimers()
{
 this->ct=0;
 this->time=0;
 this->out=false;
}

 	
//--- system timers
  SOFT_TIMER_TON t1;
  SOFT_TIMER_TON t2;
  SOFT_TIMER_TON t3;

	SOFT_TIMER_TON MRFtimer;
	SOFT_TIMER_TON OUTPUTtimer;
	SOFT_TIMER_TON TASKtimer;
	SOFT_TIMER_TON LCDtimerWRITE;
	SOFT_TIMER_TON LCDtimerPRINT;
//####################### user timers ###############
	 CLASStimers usertimer[MAXuserTIMERS ];
	 CLASStimers systimer[MAXsystemTIMERS ];
//####################### user timers ###############


void SYSTICK()
{
//####################### user timers ###############
	uint16 i;
	for (i=0; i<MAXuserTIMERS-1; i++ ) { usertimer[i].tick(); } 
		for (i=0; i<MAXsystemTIMERS-1; i++ ) { systimer[i].tick(); } 

//####################### user timers ###############

//
//	if (t1.ct>t1.time){  t1.out=true;}//таймер включился
//	else  t1.ct++;//
//	if (t1.reset) {t1.ct=0; t1.out=false; t1.reset=false; }//сброс таймера
//
//
//	if (t2.ct>t2.time){  t2.out=true;  }//таймер включился
//	else  t2.ct++;//
//	if (t2.reset) {t2.ct=0; t2.out=false; t2.reset=false; }//сброс таймера
//
//	if (t3.ct>t3.time){  t3.out=true;  }//таймер включился
//	else  t3.ct++;//
//	if (t3.reset) {t3.ct=0; t3.out=false; t3.reset=false; }//сброс таймера

	
	if (MRFtimer.ct>MRFtimer.time){  MRFtimer.out=true;  }//таймер включился
	else  MRFtimer.ct++;//
	if (MRFtimer.reset) {MRFtimer.ct=0; MRFtimer.out=false; MRFtimer.reset=false; }//сброс таймера
	
	if (OUTPUTtimer.ct>OUTPUTtimer.time){  OUTPUTtimer.out=true;  }//таймер включился
	else  OUTPUTtimer.ct++;//
	if (OUTPUTtimer.reset) {OUTPUTtimer.ct=0; OUTPUTtimer.out=false; OUTPUTtimer.reset=false; }//сброс таймера

		if (TASKtimer.ct>TASKtimer.time){  TASKtimer.out=true;  }//таймер включился
	else  TASKtimer.ct++;//
	if (TASKtimer.reset) {TASKtimer.ct=0; TASKtimer.out=false; TASKtimer.reset=false; }//сброс таймера

		if (LCDtimerWRITE.ct>LCDtimerWRITE.time){  LCDtimerWRITE.out=true;  }//таймер включился
	else  LCDtimerWRITE.ct++;//
	if (LCDtimerWRITE.reset) {LCDtimerWRITE.ct=0; LCDtimerWRITE.out=false; LCDtimerWRITE.reset=false; }//сброс таймера

		if (LCDtimerPRINT.ct>LCDtimerPRINT.time){  LCDtimerPRINT.out=true;  }//таймер включился
	else  LCDtimerPRINT.ct++;//
	if (LCDtimerPRINT.reset) {LCDtimerPRINT.ct=0; LCDtimerPRINT.out=false; LCDtimerPRINT.reset=false; }//сброс таймера

}



//----------------------------------
