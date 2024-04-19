/****************************************************************************
 * @file     encode.h
 * @version  V1.00
 * @brief    control driver header file
 * @author	 fan
 * @note	 
 * Copyright (C) 2023 Creality 3D
*****************************************************************************/
#ifndef		__ENCODE_H__
#define		__ENCODE_H__

#include "gd32e23x.h"

#define encode_A	gpio_input_bit_get(GPIOA,GPIO_PIN_2)
#define encode_B	gpio_input_bit_get(GPIOA,GPIO_PIN_3)
#define encode_S	gpio_input_bit_get(GPIOF,GPIO_PIN_0)


#define LCD_MAX_TEMP 65
#define LCD_MIN_TEMP 45

#define LCD_MAX_TIME 24
#define LCD_MIN_TIME 0

typedef struct
{
//	uint8_t encode_time;
//	uint8_t encode_temp;
	uint8_t encode_menu;
	uint8_t Previous_Output;
	uint8_t encode_cnt_max;
	int8_t encode_cnt;
}encode_t;

enum {
	menu_start=0,
//	menu_choice,
	menu_temp,
	menu_time,
};

extern encode_t ENCODE_t;


void encode_pin_config(void);
void ENCODE_LOOP(void);
void Menu_Selection(void);
#endif		//__ENCODE_H__

