/****************************************************************************
 * @file    
 * @version  V1.00
 * @brief    
 * @author
 * @note
 * Copyright (C) 2023 Creality 3D
*****************************************************************************/
#ifndef __COUNT_DOWN_H__
#define __COUNT_DOWN_H__

#include "gd32e23x.h"


typedef struct{

uint8_t  hour;
uint8_t  min;
uint8_t  sec;
uint8_t  start;		//start counting flag
uint8_t  finish;	//counting finish flag

}Time_t;

#define temp_h  12
#define temp_l  14
#define humi_h  16
#define humi_l  18

extern Time_t CD;
extern unsigned char Stime[6];
extern unsigned char Stemp[2];
extern unsigned char Shumi[2];
void Time_NumShow(unsigned char *t_array);
void Time2_Init(uint32_t prescale,uint32_t arr);

#endif //__COUNT_DOWN_H__
