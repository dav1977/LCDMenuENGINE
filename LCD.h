#pragma once
#ifndef LCD_H
#define LCD_H
#include "CoreSettings.h"
#include "Types.h"

extern 	void taskLCD();
extern 	void LCD_set_Display(byte n);
extern  void LCD_manager();
extern void  reset_dopoltit_param();

extern 	bool wait_flash;
extern bool LCD_back;
extern bool lcd_dynamic_prepare;
extern byte CurrentSTROKA;
extern byte TypeDisplay;
extern byte LAST_TypeDisplay;
extern short NUMBERPAR;
extern int prokrut;
extern int max_position;
extern bool task_printON;
extern int koeff_parol;

extern bool LCD_lock_dyn;
extern uint timer_dynamic;
extern byte cursor_position;
extern int max_strok;
extern int CurrentMenu;

extern bool set_general_param1;
extern bool set_general_param2;
extern bool set_general_param3;
extern bool set_general_param4;
extern bool set_zavod;

char FuncNULL(unsigned char); 

 struct MenuItem
{
 int indexBACK; //индекс при нажатии кнопки влево
 int startCurrent; //ТЕКУЩИЙ ИНДЕКС ОКНА
 bool cursorENABLE ; //1-курсор включен 0 - выключен
 char text[21]; // а последний символ по стандарту Си должен быть нулем)
 int maxCURSOR; //макс.строка курсора
 int offsetENTER;//смещение при нажатии ввод
 char (*func)(unsigned char); //указатель на функцию, вызываемую при динамическом UPDATE
 short number_par;//номер параметра
};


extern struct MenuItem menu[];



enum 
{
	Display_NOT,
	Display_Start,
	Display_parol,
	Display_MENU,
	Display_current,
	Display_Wait
};




//--------------------
#endif
