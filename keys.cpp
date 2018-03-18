#include "keys.h"
#include "LCD.h"
#include "LCD_ST7070.h"
#include "set_param.h"
#include "main.h"
#include "TaskManager.h"
#include "SoftTimers.h"
#include "Convert.h"


#define CORR1 0.01
#define CORR2 0.1
#define key_LEFT DI4
#define key_ENTER DI2
#define key_UP DI3
#define key_DN DI1

//****************************************************
//   обработка кнопок
//****************************************************
bool normal_speed=false;
bool lockKEY=false;
uint timer_key=0;
#define TIMERKEY 9
byte cursor_memory=0;

void set_normal_speed()
{
	sysTIM13.set(400); sysTIM12.reset();normal_speed=true;
}

void set_fast_speed()
{
	 sysTIM13.set(20); sysTIM12.reset();normal_speed=false;
}
//****************** KEY
void analiz_keys()
{
 if (parol_ok)	if (tm9.out) parol_ok=false;//СБРОС флга ввода пароля

  if (!normal_speed) if (!key_DN && !key_UP) set_normal_speed();
  if (!key_DN && !key_UP)  sysTIM12.reset();


	 if (sysTIM13.out )
{

//смена шага пароля
//if ((key_DN || key_UP) && key_ENTER)
//{
//	if (CurrentMenu==9 && !parol_ok)
//	{
//		if (koeff_parol>=100) koeff_parol=1;
//		else koeff_parol *=10;
//		sysTIM13.reset();
//		return;
//	}
//}

//---------
if (key_DN)
{
  if (sysTIM12.out) set_fast_speed();
  SWITH(-1);
  sysTIM13.reset();
  return;
}

if (key_UP)
{
  if (sysTIM12.out) set_fast_speed();
  SWITH(1);
  sysTIM13.reset();
  return;
}


}

 if (sysTIM11.out )
{

//-------------------------------------
 if (key_DN  && CursorENABLE)//DN
		{

	   if ( LAST_TypeDisplay==Display_MENU)
		 {

			 if (menu[CurrentMenu+1].offsetENTER!=-1)
		{
			 if (cursor_position<max_strok )
			 {
			 	cursor_position++;
			 	CurrentMenu++;
		 	}
			else
			{
					 cursor_position=1;
					 CurrentMenu=menu[CurrentMenu].startCurrent;
			}

			//---------------------
			if (prokrut!=0 && CurrentMenu+prokrut >=max_position ) //максимальная прокрутка
				{
					CurrentMenu--;
					cursor_position=4;
					sysTIM11.reset();
					return;
				}

				 //========================================= !!!
			   if ( max_strok>4 && cursor_position>4 )
				 {
					prokrut+=4;
				  CurrentMenu= menu[CurrentMenu].startCurrent  ;
				  cursor_position=1;
				  CurrentSTROKA=1;
				  LCD_set_Display( Display_current);
				 }

			}
			 else {  cursor_position=1;
			 CurrentMenu=menu[CurrentMenu].startCurrent;}

		  }

				sysTIM11.reset();
		 }


////-------------------------------------
		 if (key_UP  && CursorENABLE)//UP
		{


		if ( LAST_TypeDisplay==Display_MENU)
		 {
				cursor_position--;

			if (cursor_position<=0)
			{

				cursor_position=menu[CurrentMenu].maxCURSOR;
				byte temp=cursor_position;

				if (cursor_position>4)
					{
						cursor_position=1;

						if (prokrut<=0) { sysTIM11.reset(); return;}
						prokrut-=4;
						CurrentMenu= menu[CurrentMenu].startCurrent -4;

						cursor_position=4; LCD_back=true;
						CurrentSTROKA=1;
						LCD_set_Display( Display_current);

					}
				else
				{
					cursor_position=temp;
					CurrentMenu += cursor_position -1;
				}
			}
			else CurrentMenu--;



			sysTIM11.reset();
		 }
		}

////-------------------------------------
		 if (key_ENTER && !key_UP && !key_DN)//ENTER
		{
			 ERROR_PARAM=false;

			 if (!normal_speed) set_normal_speed();

			 tm9.reset();//время сброса флага ввода пароля

			if ( LAST_TypeDisplay==Display_MENU  ||  LAST_TypeDisplay==Display_parol ||  LAST_TypeDisplay==Display_Wait)
		 {

				//======================== PAROL
				if (CurrentMenu==9)
			   {

					if (!parol_ok)
					{
						if (!MODE_EDIT_PAROL)
						{
							LCD_set_Display(Display_parol); sysTIM11.reset(); return;
						}
						else
						{
							if (PAROL!=1235) {PAROL=0;sysTIM11.reset();return;}
							else
							{parol_ok=true; MODE_EDIT_PAROL=false; TASK_RUN(tasks::LCD_Update); }
						}

						sysTIM11.reset();
					}
					else MODE_EDIT_PARAM=true;
			   }




			 int s =menu[CurrentMenu].offsetENTER;
			 if (s!=0)
			 {
					 LCD_stop_DRIVER();
					 LCD_lock_dyn=true;


					 int back=menu[CurrentMenu].startCurrent;
					 CurrentMenu +=s;


					//	if(menu[CurrentMenu].indexBACK==0)
					//		{
								  //byte j=0;
							//   for (j=0; j<3; j++)
					int y=menu[CurrentMenu].startCurrent;

					//max 8 строк
					menu[y].indexBACK=back;
					menu[y+1].indexBACK=back;
					menu[y+2].indexBACK=back;
					menu[y+3].indexBACK=back;
					menu[y+4].indexBACK=back;
					menu[y+5].indexBACK=back;
					menu[y+6].indexBACK=back;
					menu[y+7].indexBACK=back;
					//	}
					  cursor_memory=cursor_position;
					  prokrut=0; cursor_position=1;CurrentSTROKA=1;
					  LCD_set_Display(Display_current);

			 }
				sysTIM11.reset();
		  }
		}


///-------------------------------------
		 if (key_LEFT)//LEFT
		{
			 reset_dopoltit_param();//Остановка измерений дополнительных параметров
			 lcd_dynamic_prepare=false;
			 if (!normal_speed)set_normal_speed();

		if (CurrentMenu<8) return;

			LCD_stop_DRIVER();
			ANALIZ_EXIT();


		 if ( (LAST_TypeDisplay==Display_MENU   && CurrentMenu>=4) ||  LAST_TypeDisplay==Display_parol ||  LAST_TypeDisplay==Display_Wait )
		 {
			 prokrut=0;
			 LCD_lock_dyn=true;

			 int s =menu[CurrentMenu].indexBACK;

			 //выход из режима пароля
             if ( LAST_TypeDisplay==Display_parol) { MODE_EDIT_PAROL=false; TASK_RUN(tasks::LCD_Update);sysTIM11.reset();CurrentMenu=8; return;}

             //сохранение параметров
             if (CurrentMenu>15 && CurrentMenu<20)  { MODE_EDIT_PARAM=false; if (parol_ok)LCD_set_Display(Display_Wait);  }//         save


			 CurrentMenu=s;


			 if (s!=0) {cursor_position=cursor_memory; LCD_back=true; CurrentSTROKA=1;}
			 LCD_set_Display(Display_current);

			 sysTIM11.reset();
		 }
			// else LCD_run_DRIVER();

		}
 }
}
