#include "LCD.h"
#include "LCD_ST7070.h"
#include "set_param.h"
#include "mainLOGIC.h"
#include "main.h"
#include "TaskManager.h"
#include "SoftTimers.h"
#include "Convert.h"
#include "Convert.h"
#include "inputs.h"
#include "keys.h"
#include "objSECTION.h"

#define LEN_STR 40// c двойным запасом

char funNULL(unsigned char){ return 0; }; //

    static char stroka1[LEN_STR];
	static char stroka2[LEN_STR];
	static char stroka3[LEN_STR];
	static char stroka4[LEN_STR];
	static char strokaEND[LEN_STR]="          ";
	static char *str1=stroka1;
	static char *str2=stroka2;
	static char *str3=stroka3;
	static char *str4=stroka4;
	static char *strE=strokaEND;
	
byte CurrentSTROKA=1;
byte TypeDisplay=0;
byte LAST_TypeDisplay=0;
short NUMBERPAR=0;
int prokrut=0;
int max_position;
int koeff_parol=1;
bool LCD_lock_dyn=true;
uint timer_dynamic=0;
byte cursor_position=1;
int max_strok;///------------------------------ ГЛОБАЛЬН
int CurrentMenu=4;
bool LCD_tsk1 = false;
bool LCD_tsk2 = false;
bool LCD_tsk3 = false;
bool LCD_tsk4 = false;

bool set_general_param1=false;
bool set_general_param2=false;
bool set_general_param3=false;
bool set_general_param4=false;
bool set_zavod=false;


//1 - индекс при нажатии кнопки назад(ЗАПОЛНИЯТСЯ ДВИЖКОМ!!)
//2 - ТЕКУЩИЙ ИНДЕКС ОКНА
//3 - курсор 1-включен   0-выключен
//4 - строка
//5 - макс.строка курсора
//6 - смещение при нажатии ввод
//7 - указатель на функцию, вызываемую ....
//8 - номер редактируемого параметра (0 - нет параметров)
struct MenuItem menu[]={
	 {0, 1, 1, "      Строка 1      ",1,0, funNULL,0},
	 {0, 1, 1, "      Строка 2      ",4,0, funNULL,0},
	 {0, 1, 1, "      Строка 3      ",4,0, funNULL,0},
	 {0, 1, 1, "      Строка 4      ",4,0, funNULL,0},
	 
	 {
	 //-------------------------
	 // резерв max 8 строк
     {0, 999, 1, " ", 8,-1,  funNULL,0},//
     {0, 999, 1, " ", 8,-1, funNULL,0},//
     {0, 999, 1, " ", 8,-1,  funNULL,0},//
     {0, 999, 1, " ", 8,-1, funNULL,0},//
     {0, 999, 1, " ", 8,-1,  funNULL,0},//
     {0, 999, 1, " ", 8,-1, funNULL,0},//
     {0, 999, 1, " ", 8,-1,  funNULL,0},//
     {0, 999, 1, " ", 8,-1, funNULL,0},//
	 
};



//**********************************************************************************************************
//			MENU ENGINE
//**********************************************************************************************************
bool LCD_back=false;
void LCD_menu()  
{
   LCD_tsk4=true;
   if (CurrentSTROKA==1)
	 {		 
			LCD_stop_DRIVER();
			LCD_run_DRIVER();

      if (menu[CurrentMenu].cursorENABLE) {  CursorENABLE=true;} else CursorENABLE=false;
		 NUMBERPAR=menu[CurrentMenu].number_par; 
		 
		 StringToChar((char*) menu[CurrentMenu].text,  str1);
		 LCD_PRINTstr (1, stroka1, 0, vozv);		 
 
		 max_strok=menu[CurrentMenu].maxCURSOR;
		 if (max_strok>5)
		 {
			 max_position=menu[CurrentMenu].startCurrent+max_strok;
		 }
		
		 CurrentSTROKA++; 
		 CurrentMenu+= 1;
	 }

	 if (CurrentSTROKA==2)
	 {
		  if ( max_strok<2) StringToChar((char*) menu[1].text,  str2);
		  else
		   StringToChar((char*) menu[CurrentMenu].text,  str2);
	
			LCD_PRINTstr (2, stroka2, 0, vozv);
				
			
			if (CurrentSTROKA>=max_strok) 
				{		
					int cm =  menu[CurrentMenu].startCurrent;
					CurrentMenu=cm; cursor_position=1;
				}
			else 
			{
			  CurrentMenu+= 1;
			}
			
			CurrentSTROKA++;
	 }
	 

	 if (CurrentSTROKA==3)
	 {
			
		   if ( max_strok<3) StringToChar((char*) menu[1].text,  str3);
		  else
		 StringToChar((char*) menu[CurrentMenu].text,  str3);
			
	
			LCD_PRINTstr (3, stroka3, 0, vozv);
		
			 if (CurrentSTROKA>=max_strok) 
			{
 				int cm =  menu[CurrentMenu].startCurrent;
			  CurrentMenu=cm;  cursor_position=1;
			}
			else CurrentMenu+= 1;

			CurrentSTROKA++;
	 }
	 
	 
	 if (CurrentSTROKA==4)
	 { 
		  if ( max_strok<4) StringToChar((char*) menu[1].text,  str4);
		  else
		  StringToChar((char*) menu[CurrentMenu].text,  str4);
				
	
			LCD_PRINTstr (4, stroka4, 0, vozv);
	

			
			if (CurrentSTROKA>=max_strok || max_strok>=5)
			{		
				//if (!LCD_back)
				//{
				int cm =  menu[CurrentMenu].startCurrent;
 				CurrentMenu=cm;
			  	cursor_position=1;
				//}
				LCD_back=false;
			}
			else CurrentMenu+= 1;

	 }
}


//****************************************************
//  dynamic update
//****************************************************
	bool lcd_dynamic_prepare=false;

void YESNO(bool * p)
{
	if (*p) StringToChar((char*) "ДА ",  str4);
	else StringToChar((char*) "НЕТ",  str4);
}




	void LCD_UPDATE_DYNAMIC()//обновление динамических данных  //выполняется периодически по таймеру
	{
		//if (!lcd_dynamic_prepare)
		//{
		 if (LCD_tsk4)return;
		 if (LCD_lock_dyn)return;
		//}
	//------------------------
		   
		 LCD_stop_DRIVER();
		 LCD_run_DRIVER();

    bool std=false;
    std=false;
		if (NUMBERPAR!=0)
		{
			
			
		//	Измененние прикладных параметров
			switch(NUMBERPAR) 
		{				
			case 1:  					
				YESNO(&set_general_param1);
			break;
			

			//=======================================================
		}
		

		//----------
		if (!std)
		{
			StringToChar( strE,  vozv);
			LCD_PRINTstr (4, stroka4, 8, vozv);
		}
		else std=false;
		//----------
		}
		else //не режим установки
		{

		  switch(CurrentMenu)
		 {		
		


			break;

	     }
		
		 
	}
		
		 LCD_set_CURSOR(cursor_position);
	}

//**********************************************************************************************************
//			Остановка измерений дополнительных параметров
//**********************************************************************************************************
void reset_dopoltit_param()
{
	mrf.mode_read_power = false;
}
	
//**********************************************************************************************************
//			Дополнительные Дисплеи
//**********************************************************************************************************
		void LCD_start_Display()
	{

	  StringToChar((char*) "==============================",  str1);
		LCD_PRINTstr (1, stroka1, 0, vozv);

		StringToChar((char*) "  ЭКРАН ЗАПУСКА ",  str2);
		LCD_PRINTstr (2, stroka2, 0, vozv);

		StringToChar((char*) "                ",  str3);
		LCD_PRINTstr (3, stroka3, 0, vozv);

	  StringToChar((char*) "==============================",  str4);
		LCD_PRINTstr (4, stroka4, 0, vozv);
	}

		void LCD_WAIT()
		{

			CursorENABLE=false;
		  StringToChar((char*)   "====================",  str1);
			LCD_PRINTstr (1, stroka1, 0, vozv);

			StringToChar((char*) "     СОХРАНЕНИЕ    ",  str2);
				LCD_PRINTstr (2, stroka2, 0, vozv);

			StringToChar((char*) "     ПАРАМЕТРОВ    ",  str3);
			LCD_PRINTstr (3, stroka3, 0, vozv);

			  StringToChar((char*) "====================",  str4);
			LCD_PRINTstr (4, stroka4, 0, vozv);
		}

		void LCD_parol()
		{

			CursorENABLE=false;
		    StringToChar((char*) " ",  str1);
			LCD_PRINTstr (1, stroka1, 0, vozv);

			StringToChar((char*) "   ВВЕДИТЕ ПАРОЛЬ     ",  str2);
			LCD_PRINTstr (2, stroka2, 0, vozv);

			StringToChar((char*) " ",  str3);
			LCD_PRINTstr (3, stroka3, 0, vozv);

		  StringToChar((char*) " ",  str4);
			LCD_PRINTstr (4, stroka4, 0, vozv);
		}
	

//****************************************************
//     Установка дисплея
//****************************************************	
	void LCD_set_Display(byte n)	
 {
	    TypeDisplay=n;
	 
	 		if  (TypeDisplay!=LAST_TypeDisplay )  
		{
			    LCD_lock_dyn=true;
				LCD_stop_DRIVER();
			
			   if (TypeDisplay==Display_current) {  TypeDisplay=LAST_TypeDisplay;  }
			 
				if (TypeDisplay == Display_Start  ){ LCD_start_Display(); }
				if (TypeDisplay == Display_parol ) { LCD_tsk2=true; }
				if (TypeDisplay == Display_MENU  ) {LCD_menu();	}
				if (TypeDisplay == Display_Wait  ) {LCD_tsk1=true; }

				LAST_TypeDisplay=TypeDisplay;
		}
	 		 LCD_run_DRIVER();
} 

//****************************************************
//  task ВОЗВРАТА В МЕНЮ
//****************************************************
	void taskLCD()
	{
		if (LCD_is_PRINTING()) return;//пока печатается дополнительный дисплей

		if (LCD_tsk3)
		{
			LCD_tsk3=false;

		}
		else
		{
			CurrentSTROKA=1;
			LCD_set_Display(Display_MENU);
			LCD_tsk2=false;
			MODE_EDIT_PAROL=false;
			LCD_run_DRIVER();
			LCD_lock_dyn=true;
			TASK_DONE(tasks::LCD_Update);
		}

	}
//****************************************************
//					LCD MENU MANAGER
//****************************************************
#define TIMERDYNAMIC 5000//задержка появления динамических данных
 void LCD_manager()
 {
	 LCD_WORK();//драйвер

	 if (TASK__GET(tasks::LCD_Update)) return;

	 if (LCD_tsk1)//tsk ОЖИДАНИЕ
	 {
		 LCD_lock_dyn=true;
		 LCD_tsk3=true;
		 LCD_WAIT();
		 LCD_tsk1=false;
		 init_SECTIONS();
		 flash_start_write();
		 TASK_RUN(tasks::LCD_Update);
		 /*sysTIM13.reset();*/
		 return;
	 }

	 if (LCD_tsk2)//tsk ПАРОЛЬ
	 {
		 LCD_lock_dyn=true;
		 LCD_parol();
		 LCD_tsk2=false;
		 NUMBERPAR=999;
		 MODE_EDIT_PAROL=true;
		 return;
	 }

	 if (LCD_is_PRINTING()) return;//если идет прорисовка
	 else LCD_tsk4=false;

	 if (!flash_work_done) return;//если запись то не работаем

		 analiz_keys();

		if (sysTIM5.out && systemSTART )
		{
			LCD_UPDATE_DYNAMIC(); sysTIM5.reset();
			if (CurrentMenu<4) {CurrentMenu=4;  LCD_set_Display(Display_current);}
		}
				
	
				if (LCD_lock_dyn )
			 {
					timer_dynamic++;
					if (timer_dynamic<TIMERDYNAMIC) return;
					if (timer_dynamic>TIMERDYNAMIC){ timer_dynamic=TIMERDYNAMIC; LCD_lock_dyn=false; }
			 }

 }


	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	



