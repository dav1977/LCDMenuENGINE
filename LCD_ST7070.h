﻿#pragma once
#include "CoreSettings.h"
#include "Types.h"


extern "C"
	{
		void ST7070_init(void);
		void LCD_WORK(void); //work
		void LCD_PRINTstr( byte numstr,  char *str, byte offset, char *lastSYMB);
		void LCD_set_CURSOR(byte pos);
		void LCD_stop_DRIVER(void);
		void LCD_run_DRIVER(void);
		bool LCD_is_PRINTING(void);
	}
	
 typedef struct 
	{		
		bool TASKworkON;
		bool blockStep;
		uint delay;
		byte step;
		byte iter;
		byte wrData;
	} lcd_io;

	
extern bool CursorENABLE;

	
	
	enum Control
	{
		A0 = 10,
		RW,
		E
	};
	enum Array
	{
		Left = 0xFA,
		Right
	};



const char BigValue[][6]=
{
	{0x07, 0x00, 0x01, 0x02, 0x03, 0x04}, // 0
	{0x00, 0x01, 0x20, 0x03, 0xFF, 0x03}, // 1
	{0x05, 0x05, 0x01, 0xFF, 0x06, 0x06}, // 2
	{0x00, 0x05, 0x01, 0x03, 0x06, 0x04}, // 3
	{0x02, 0x03, 0xFF, 0x20, 0x20, 0xFF}, // 4
	{0xFF, 0x05, 0x05, 0x06, 0x06, 0x04}, // 5
	{0x07, 0x05, 0x05, 0x02, 0x06, 0x04}, // 6
	{0x00, 0x05, 0x04, 0x20, 0xFF, 0x20}, // 7
	{0x07, 0x05, 0x01, 0x02, 0x06, 0x04}, // 8
	{0x07, 0x05, 0x01, 0x06, 0x06, 0x04}, // 9
};

const char BigSymbol[][6]=
{
	{0x07, 0x00, 0x01, 0xFF, 0x00, 0xFF}, // A - 0
	{0xFF, 0x00, 0x01, 0xFF, 0x06, 0x04}, // B - 1
	{0x00, 0xFF, 0x00, 0x20, 0xFF, 0x20}, // T - 2
	{0xFF, 0x20, 0xFF, 0x02, 0x03, 0x04}, // U - 3
	{0x07, 0x00, 0x01, 0x02, 0x03, 0x04}, // O - 4
	{0x00, 0x05, 0x01, 0x03, 0x06, 0x04}, // З - 5
	{0x07, 0x00, 0x01, 0xFF, 0x20, 0xFF}, // Л - 6
	{0xFF, 0x00, 0xFF, 0xFF, 0x20, 0xFF}  // П - 7
};

const char CostumFont[64] = 
{
	0x1F, 0x1F, 0x1F, 0x00, 0x00, 0x00, 0x00, 0x00, // char 1 
	0x18, 0x1C, 0x1E, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, // char 2 
	0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x0F, 0x07, 0x03, // char 3 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x1F, 0x1F, // char 4 
	0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1E, 0x1C, 0x18, // char 5 
	0x1F, 0x1F, 0x1F, 0x00, 0x00, 0x00, 0x1F, 0x1F, // char 6 
	0x1F, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x1F, 0x1F, // char 7 
	0x03, 0x07, 0x0F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F  // char 8 
};

struct CyrillicSymbol
{
	char Symbol;
	unsigned char Code;
	bool Page;
};

#define SIZE_CYRILLIC 66
struct Cyrillic
{
	CyrillicSymbol Alphabet[SIZE_CYRILLIC];
};

const Cyrillic CyrillicFont=
{
	'А',0x41,0,
	'Б',0xBD,1,
	'В',0x42,0,
	'Г',0xBE,1,
	'Д',0xBF,1,
	'Е',0x45,0,
	'Ё',0x1F,0,
	'Ж',0xC0,1,
	'З',0xC1,1,
	'И',0xC2,1,
	'Й',0xC3,1,
	'К',0xC4,1,
	'Л',0xC5,1,
	'М',0x4D,0,
	'Н',0x48,0,
	'О',0x4F,0,
	'П',0x86,1,
	'Р',0x50,0,
	'С',0x43,0,
	'Т',0x54,0,
	'У',0xC7,1,
	'Ф',0xC8,1,
	'Х',0x58,0,
	'Ц',0xC9,1,
	'Ч',0xCA,1,
	'Ш',0xCB,1,
	'Щ',0xCC,1,
	'Ъ',0xCD,1,
	'Ы',0xCE,1,
	'Ь',0xCF,1,
	'Э',0xD0,1,
	'Ю',0xD1,1,
	'Я',0xD2,1,
	'а',0x61,0,
	'б',0xD3,1,
	'в',0xD4,1,
	'г',0xD5,1,
	'д',0xD6,1,
	'е',0x65,0,
	'ё',0x1F,1,
	'ж',0xD7,1,
	'з',0xD8,1,
	'и',0xD9,1,
	'й',0xDA,1,
	'к',0xDB,1,
	'л',0xDC,1,
	'м',0xDD,1,
	'н',0xDE,1,
	'о',0x6F,0,
	'п',0xDF,1,
	'р',0x70,0,
	'с',0x63,0,
	'т',0xE0,1,
	'у',0x79,0,
	'ф',0xE1,1,
	'х',0x78,0,
	'ц',0xE2,1,
	'ч',0xE3,1,
	'ш',0xE4,1,
	'щ',0xE5,1,
	'ъ',0xE6,1,
	'ы',0xE7,1,
	'ь',0xE8,1,
	'э',0xE9,1,
	'ю',0xEA,1,
	'я',0xEB,1
};

//-----------

