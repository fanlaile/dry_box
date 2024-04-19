/****************************************************************************
 * @file    
 * @version  V1.00
 * @brief    
 * @author
 * @note
 * Copyright (C) 2023 Creality 3D
*****************************************************************************/
#ifndef __TEMPERATURE_H__
#define __TEMPERATURE_H__

#include "gd32e23x.h"


#define IO3V3(x)		gpio_bit_write(GPIOF,GPIO_PIN_1,x)

extern uint16_t ADC_Value_Inject[2];


void ADC_Init(void);
void adc_pin_config(void);
void adc_config(void);
void ADC_DMAConfig(void);
uint16_t Read_temperature(void);
uint16_t Read_humidity(void);
uint8_t ADC_TO_TEMP(uint16_t AD_value);
uint8_t Hmd_calculate(void);
void Temp_Hum_To_LCD(void);
#endif //__TEMPERATURE_H__




