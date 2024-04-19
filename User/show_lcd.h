/****************************************************************************
 * @file     show_lcd.h
 * @version  V1.00
 * @brief    tm1621 lcd driver header file
 * @author	 fan
 * @note	 
 * Copyright (C) 2023 Creality 3D
*****************************************************************************/
#ifndef		__SHOW_LCD_H__
#define		__SHOW_LCD_H__

#include "gd32e23x.h"

#define uchar unsigned char				 //数据类型宏定义
#define uint unsigned int				 //数据类型宏定义
extern unsigned char Smg[10];
extern unsigned char Tab0[10];

#define CS(x)		gpio_bit_write(GPIOB,GPIO_PIN_1,x)
#define WRITE(x)	gpio_bit_write(GPIOA,GPIO_PIN_6,x)
#define DATA(x)		gpio_bit_write(GPIOA,GPIO_PIN_5,x)
#define READ(x)		gpio_bit_write(GPIOA,GPIO_PIN_7,x)

#define DATA_ask	gpio_input_bit_get(GPIOA,GPIO_PIN_6)


void lcd_pin_config(void);
void TM1621_init();

void WriteAllData(uchar Addr,uchar *p,uchar cnt);
void WriteOneData(uchar Addr, uchar Data);
#endif		//__SHOW_LCD_H__

