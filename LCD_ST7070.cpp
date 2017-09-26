#include "LCD_ST7070.h"
#include "SoftTimers.h"

	#define NativePosition(x) (x<21)?x-1:  (x<41)?0x40+x-21:  (x<61)?0x14+x-41:  0x54+x-61
	lcd_io LCDprint={false,false,0,0, 0,0};
	lcd_io LCDWrite={false,false,0,0 ,0,0};
	
//===============================================================================
//								4х строчнй LCD	20символов в строке
//===============================================================================
bool CurrentPAGE=false;//СТРАНИЦА ТЕКУЩАЯ    LATIN- только на 0 странице;  CYRILLIC- на 1 странице
byte LCDtask=0;
byte CursorPosition=1;
bool CursorENABLE=false;

char *globSTR;
byte globPOS;
char *globSTRmass[4];
byte globPOSmass[4];

char CURSOR[]={'>',0};
char *pCURSOR;
char NOTCURSOR[]={' ',0};
char *pNOTCURSOR;

#define _40us 137

void delay_us(uint t)
{
	while(--t);
}

void delay_ms(uint t)
{
	t = t * 4000;
	while(--t);
}


void ControlSet(Control Cl)
{
	GPIOC->BSRR |= 1 << Cl;
}

void ControlClear(Control Cl)
{
	GPIOC->BSRR |= 1 << (Cl + 16);
}

void Reset(bool Active)
{
	if(Active) GPIOA->BSRR = 0x80000000;
	else GPIOA->BSRR = 0x8000;
}

void Bus(byte Data)
{
	GPIOD->MODER |= 0x5555;// Data bus out
	GPIOD->BSRR = 0xFF << 16;
	GPIOD->BSRR = Data;
}

void WriteCycle(byte Data)
{
	ControlClear(RW);//C11
	ControlSet(E);//C12
	delay_ms(1);
	Bus(Data);
	ControlClear(E);
}

void SendCommand(byte com)
{
	ControlClear(A0);//C10
	WriteCycle(com);
}

void SendData(byte data)
{
	ControlSet(A0);//C10
	WriteCycle(data);
}

void PageSet(bool page)
{
	SendCommand(0x0C | (byte)page);
	CurrentPAGE = page;
}


void CursorSet(byte Position)
{
	register byte temp = Position & 0x7F;
	if(temp < 0x68)
	{
		SendData(0x80 | Position);// 0 point
		Position = temp;
	}
}
void Set(byte Position)
{
	CursorSet( NativePosition(Position) );
}


void LoadCostumFont()
{
	SendCommand(0x40);//Set CGRAM address
	for(byte i = 0; i < 64; i++) SendData(CostumFont[i]);
}

void InitST7070()
{
	CurrentPAGE = 0;
	SendCommand(0x38);// 8 bit
	delay_us(_40us);
	SendCommand(0x0E);// Turn ON and cursor ON
	delay_us(_40us);
}

void ST7070_init()
{
	 pCURSOR = &CURSOR[0];
	 pNOTCURSOR = &NOTCURSOR[0];

	 LCDprint.TASKworkON=false;
	 LCDWrite.TASKworkON=false;

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOCEN | RCC_AHB1ENR_GPIODEN;
	GPIOA->MODER &= 0x3FFFFFFF;
	GPIOA->MODER |= MODERSet(15, ModeOut);//15-Reset;
	GPIOC->MODER |= MODERSet(10, ModeOut) | MODERSet(11, ModeOut) | MODERSet(12, ModeOut);// 10-A0; 11-R/W; 12-E
	Reset(true);
	delay_ms(40);
	Reset(false);
	InitST7070();
	LoadCostumFont();
	CursorSet(0);
	
}

	void LCD_STEPoffset(int8 s){	LCDprint.step +=s;	LCDprint.blockStep=false;}
	void LCD_Next_Step(){	LCDprint.step++;	LCDprint.blockStep=false;}
	void LCD_iniStepPrint() {  LCDprint.step=0; LCDprint.blockStep=false; }
	void LCD_NextStep_Delay(uint t){ LCDprint.delay=t;  LCDprint.step++; LCDprint.blockStep =false;	}
	
bool step_analiz_print()
	{
		static bool timerBLOCK=false;

		if (LCDprint.delay!=0)
		{
			LCDtimerPRINT.time=LCDprint.delay; LCDprint.delay=0; LCDtimerPRINT.ct=0;  LCDtimerPRINT.out=false;
			timerBLOCK=true;
		}
	
		if (LCDtimerPRINT.out)  timerBLOCK=false;
		if (timerBLOCK) return true;
		
		if (LCDprint.blockStep) return true;//уже идет выполнение шага	
		LCDprint.blockStep=true;
		return false;
	}

//----------------------------------------------------------------------------------------------------
void LCD_NextStepWrite(){	LCDWrite.step++;	LCDWrite.blockStep=false;}
void LCD_iniStepWrite() {  	LCDWrite.step=0; 	LCDWrite.blockStep=false; }
void LCD_NextStepWriteDelay(uint t){ LCDWrite.delay=t;  LCDWrite.step++; LCDWrite.blockStep =false;	}
	
bool step_analiz_Write()
	{
		static bool timerBLOCK=false;

			if (LCDWrite.delay!=0) 
			{
				LCDtimerWRITE.time = LCDWrite.delay;
				LCDWrite.delay=0;
				LCDtimerWRITE.ct=0;
				LCDtimerWRITE.out=false;
				timerBLOCK=true;
			}
	
		if ( LCDtimerWRITE.out )   timerBLOCK=false;
		if (timerBLOCK) return true;
		
		if (LCDWrite.blockStep) return true;//уже идет выполнение шага	
		LCDWrite.blockStep=true;
		return false;
	}

//===============================================================================
//								Функции записи
//===============================================================================
//--------------------------------------------------  TASK WRITE
void LCD_sendCHAR( byte Data)//инит задачи ВЫВОД СИМВОЛА
{
	LCDWrite.step=0;
	LCDWrite.wrData=Data;
	ControlSet(A0);//C10
	LCDWrite.TASKworkON=true;//СТАРТ ЗАДАЧИ, КОГДА ВЫПОЛНИТСЯ САМА СБРОСИТ ФЛАГ
}

void LCD_setCOMMAND( byte Data)//инит задачи КОММАНДА!!
{
	LCDWrite.step=0;
	LCDWrite.wrData=Data;
	ControlClear(A0);//C10
	LCDWrite.TASKworkON=true;//СТАРТ ЗАДАЧИ, КОГДА ВЫПОЛНИТСЯ САМА СБРОСИТ ФЛАГ
}

void LCD_TASK_WRITEcycle( byte Data)
{
	step_analiz_Write();		
		switch(LCDWrite.step)
		{	
			case 0:
				
						ControlClear(RW);//C11
						ControlSet(E);//C12
						LCD_NextStepWriteDelay(2);
			break;
			case 1:
						Bus(Data);
						ControlClear(E);
						LCDWrite.TASKworkON=false;
						LCD_NextStepWrite();
			break;
			case 2: 
							//LCDWrite.TASKworkON=false;
						
			break;
			case 3: //висим задача выполнена
			default: 									 
			break;
		}
										
		
}
//===============================================================================
//										TASK_PRINT
//===============================================================================

void LCD_TASK_PRINT()
{
	static byte temp; 
	static byte index, i;
	
	step_analiz_print();
	switch (LCDprint.step)
	{
			case 0: 
							index = 0;
							if(globPOS < 0x28 || (globPOS > 0x39 && globPOS < 0x68)){} else { LCDprint.TASKworkON=false; return;}
								
							temp = globPOS & 0x7F;//void CursorSet(byte Position)
							if(temp < 0x68)
							{
						    LCD_setCOMMAND(0x80 | globPOS);
								globPOS = temp;
								LCD_NextStep_Delay(5);
							}
							else { LCD_STEPoffset(5);}
			break;
			case 1: if (!LCDWrite.TASKworkON) LCD_Next_Step(); else {}//остаемся в  это м же шаге
				
			break;
			case 2:  i=0;  
							 if(((uint8)globSTR[index] >= 'А'/*0xBF*/ /*&& (uint8)globSTR[index] <= 0xFF*/) ||
										globSTR[index] == 'Ё' || globSTR[index] == 'ё'){ 	LCD_Next_Step();}
							 else { LCD_STEPoffset(5);/*PrintLatin*/ }
			break;  					 
			case 3:    //------------- цикл i  поиск символа в массиве  PrintCyrillic (РУССКИЕ) 	
									
									if(CyrillicFont.Alphabet[i].Symbol == globSTR[index])
									{	
														//проверка страницы (всего две 0 и 1) КИРИЛЛИЦА НА 1 странице
														if(CyrillicFont.Alphabet[i].Page != CurrentPAGE)
														{
																	CurrentPAGE = CyrillicFont.Alphabet[i].Page;
																	LCD_setCOMMAND(0x0C | (byte)CurrentPAGE );
																	globSTR[index]=CyrillicFont.Alphabet[i].Code;
																	LCD_NextStep_Delay(5);
																
														}
														LCD_STEPoffset(2);
									}
									else  
									{ 							
												i++; 
												if (i<SIZE_CYRILLIC) 
												{  
													LCD_STEPoffset(0);//остаемся в шаге продолжаем искать
												} 
												else 
												{  /* символ не найден в таблице*/ LCD_STEPoffset(4); } 								
									}
			break;								
			case 4:   	if (!LCDWrite.TASKworkON) LCD_Next_Step(); //else {}//остаемся в  это м же шаге
			break;
				      ///---------------------------------------------------------------
			case 5: 	globSTR[index]=CyrillicFont.Alphabet[i].Code;
						LCD_Next_Step();
			break;
			case 6:   	LCD_STEPoffset(3);
			break;
				
			case 7:	/* русский символ не найден в таблице*/
																	CurrentPAGE = 0;
																	LCD_setCOMMAND(0x0C | (byte)CurrentPAGE );
																	LCD_NextStep_Delay(10);
																		
			break;	
			case 8: 	 if (!LCDWrite.TASKworkON) LCD_Next_Step();// else {}//остаемся в  это м же шаге
			break;
				
			case 9:   	LCD_sendCHAR(globSTR[index]);
						LCD_NextStep_Delay(5);
			break;
			case 10: 	if (!LCDWrite.TASKworkON) LCD_Next_Step(); //else {}//остаемся в  это м же шаге
			break;				
					
					
							//---------------------------
			case 11: 	 index++;
								if((globPOS == 0x13 || globPOS == 0x53 || globPOS == 0x27) && globSTR[index])
							{
								switch(globPOS)
								{
									case 0x13: globPOS = 0x40; break;
									case 0x53: globPOS = 0x14; break;
									case 0x27: globPOS = 0x54; break;
									default: break;
								}
								 LCD_NextStep_Delay(2); 
							}
							else{ globPOS++; LCD_STEPoffset(3);  }
			break;

						
													
			case 12: temp = globPOS & 0x7F;
													if(temp < 0x68)
													{
														LCD_sendCHAR(0x80 | globPOS);			
														globPOS = temp;
														LCD_NextStep_Delay(10);
													}
													else {LCD_STEPoffset(2); }
									break;
			case 13:   if (!LCDWrite.TASKworkON) LCD_Next_Step(); //else {}//остаемся в  это м же шаге
									break;
									

		//--------------------------------------------		 
		 case 14: if(globPOS == 0x67) {globPOS = 0;}
													
							if (globSTR[index]==0) {LCDprint.TASKworkON=false;LCD_Next_Step();}//задача выполнена
			        else LCD_STEPoffset(-12);
		 break;
									
		 case 15:  //висим ждем иницилизации менеджера
		 default: 								 
		 break;			
	}	
}
//#################################################################################
// 								Подготовка TASK LCD
//#################################################################################
void LCD_start_TASK_print()
{
    LCDprint.step=0;
    LCDprint.TASKworkON=true;//СТАРТ ЗАДАЧИ, КОГДА ВЫПОЛНИТСЯ САМА СБРОСИТ ФЛАГ
}

bool LCD_is_PRINTING()
{
   if (LCDtask!=0) return true; else return false;
}
//#################################################################################
// 									CURSOR
//#################################################################################
void LCD_PRINT_CURSOR()
{
	if (!LCDprint.TASKworkON )
	{
	 LCDprint.iter++;
  	 if (LCDprint.iter==2) {if (CursorPosition==2 ) globSTR=pCURSOR; else  globSTR=pNOTCURSOR;
  	 	 	 	 	 	 	 globPOS=NativePosition(21); LCD_start_TASK_print();}
	 if (LCDprint.iter==3) {if (CursorPosition==3 ) globSTR=pCURSOR; else  globSTR=pNOTCURSOR;
	 	 	 	 	 	 	 globPOS=NativePosition(41); LCD_start_TASK_print();}
	 if (LCDprint.iter==4) {if (CursorPosition==4 ) globSTR=pCURSOR; else  globSTR=pNOTCURSOR;
	 	 	 	 	 	 	 globPOS=NativePosition(61); LCD_start_TASK_print(); }
	 if (LCDprint.iter>=5 ) { BITRES(LCDtask,7); LCDprint.iter=0; }
	}
}
//#################################################################################
// 							manager TASK_LCD
//#################################################################################
bool LCD_DRIVER_ENABLE=true;
void LCD_stop_DRIVER()
{
	LCD_DRIVER_ENABLE=false;
	LCDtask=0;
	LCDprint.step=0;
	LCDprint.TASKworkON=false;
	LCDprint.iter=0;
}
void LCD_run_DRIVER()
{
	LCD_DRIVER_ENABLE=true;
}

void LCD_WORK()
{
	if (!LCD_DRIVER_ENABLE) return;
	 if (LCDWrite.TASKworkON)  LCD_TASK_WRITEcycle(LCDWrite.wrData);
	 if (LCDprint.TASKworkON)  LCD_TASK_PRINT( );

	if (LCDprint.iter!=0){ LCD_PRINT_CURSOR(); return; }
	 
	//запуск задачи если надо напечатать строки
	if (LCDtask!=0 && !LCDprint.TASKworkON)  
	{
		LCD_start_TASK_print();
		if (!CursorENABLE && BITGET(LCDtask,7)) BITRES(LCDtask,7); 
		
		//обязательно выставлять ПЯТЫЙ бит!!!
		if (BITGET(LCDtask,1)) { globSTR=globSTRmass[0]; globPOS=globPOSmass[0]; BITRES(LCDtask,1); BITSET(LCDtask,5);return; }//сначала печатаем 1 строку
		else
		if (BITGET(LCDtask,2)) {globSTR=globSTRmass[1]; globPOS=globPOSmass[1]; BITRES(LCDtask,2);BITSET(LCDtask,5);return; }
		else
		if (BITGET(LCDtask,3)) {globSTR=globSTRmass[2]; globPOS=globPOSmass[2]; BITRES(LCDtask,3);BITSET(LCDtask,5);return; }
		else
		if (BITGET(LCDtask,4)) {globSTR=globSTRmass[3]; globPOS=globPOSmass[3]; BITRES(LCDtask,4); BITSET(LCDtask,5);return; }
		else
		if (BITGET(LCDtask,7)) { LCDprint.iter=1; if (CursorPosition==1 ) globSTR=pCURSOR; else  globSTR=pNOTCURSOR;
													globPOS= NativePosition(1); return; }
		else //больше нечего печатать
		{
			LCDtask=0;LCDprint.TASKworkON=false;
		}
	}

}

//#################################################################################
// 							 внешняя функция Установки задач
//#################################################################################
void LCD_PRINTstr( byte numstr,  char *str, byte offset, char *maxstr)
{
	if (numstr<1 || numstr>4) return;//НОМЕР СТРОКИ
	
	byte len=(byte)(maxstr-str);	
	byte last_position=offset+len;
	
	if (last_position>19) str[20-offset]=0;
	else
	{
	//заполнение остатка строки пробелами
				byte ostatok= 20-last_position;
				if (last_position<20)
				{
					byte i;
					for (i=20-ostatok;  i<20;  i++)  str[i]=' ';
				}

				if (offset==0) str[20]=0;  else str[len]=0;
	}


	byte Position=(numstr-1)*20 +1;
	//запоминаем указатели
	globSTRmass[numstr-1]=str;
	globPOSmass[numstr-1]= NativePosition(Position+ offset);

							
	//запуск задачи печати
	BITSET(LCDtask, numstr);
}

void LCD_set_CURSOR(byte pos)
{
    CursorPosition=pos;	
	BITSET(LCDtask, 7);
}

