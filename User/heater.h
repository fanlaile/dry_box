/****************************************************************************
 * @file     heater.h
 * @version  V1.00
 * @brief    heater driver header file
 * @author	 fan
 * @note	 
 * Copyright (C) 2023 Creality 3D
*****************************************************************************/
#ifndef		__HEATER_H__
#define		__HEATER_H__

#include "gd32e23x.h"

#define PWM_OUT(x)		gpio_bit_write(GPIOA,GPIO_PIN_4,x)

typedef struct {
	uint8_t TimerFlag;		//定时器标志位
	uint16_t TimerCnt;		//定时计数值
	uint16_t PWMCnt;		//PWM计数值
    uint8_t set_tmp;		//设定温度
    uint8_t actual_tmp;		//实际温度
    float error;			//偏差  
    float error_next;		//上一个偏差  
    float error_last;		//上上一个偏差 
    float kp,ki,kd;			//定义比例，积分，微分参数  
	float PIDOut;
	float PIDOutnext;
}PID_PARA;

extern PID_PARA pid;

void pid_init(void);
void Timer13_Init(void);

#endif		//__HEATER_H__

