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
	uint8_t TimerFlag;		//��ʱ����־λ
	uint16_t TimerCnt;		//��ʱ����ֵ
	uint16_t PWMCnt;		//PWM����ֵ
    uint8_t set_tmp;		//�趨�¶�
    uint8_t actual_tmp;		//ʵ���¶�
    float error;			//ƫ��  
    float error_next;		//��һ��ƫ��  
    float error_last;		//����һ��ƫ�� 
    float kp,ki,kd;			//������������֣�΢�ֲ���  
	float PIDOut;
	float PIDOutnext;
}PID_PARA;

extern PID_PARA pid;

void pid_init(void);
void Timer13_Init(void);

#endif		//__HEATER_H__

