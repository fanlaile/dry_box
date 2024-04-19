/****************************************************************************
 * @file    	main.c
 * @version  	V1.00
 * @brief    	
 * @author		FAN
 * @note	
 * Copyright (C) 2023 Creality 3D
*****************************************************************************/

#include "gd32e23x.h"
#include "gd32e230f_start.h"
#include "systick.h"
#include <stdio.h>
#include <string.h>

void gpio_config(void);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    /*  configure SysTick */
    systick_config();
	delay_1ms(1000);
    /* configure EXTI, TIMER */
    gpio_config();
	Time2_Init(7199,10000/2);
	Timer13_Init();
	ADC_DMAConfig();
	adc_config();
//	Usart_Init();
	pid_init();
	TM1621_init();
//	printf("hello ");
    
    while(1)
	{
//		UART_TEST_HANDLE();			//uart debug
		Menu_Selection();			//Function Selection
		Temp_Hum_To_LCD();
		
//		WriteAllData(0,Tab0,10);
//		delay_1ms(1000);
//		WriteAllData(0,Smg,10);
//		delay_1ms(1000);
    }
}




/*! 
    \brief      configure the GPIO ports
    \param[in]  none
    \param[out] none
    \retval     none
  */
void gpio_config(void)
{
    rcu_periph_clock_enable(RCU_GPIOA);
	rcu_periph_clock_enable(RCU_GPIOF);
	
    /* configure PA13(SWDIO) as output */
    gpio_mode_set(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_13);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,GPIO_PIN_13);
//    gpio_af_set(GPIOA, GPIO_AF_0, GPIO_PIN_13);
	gpio_mode_set(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_4);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,GPIO_PIN_4);
	
	/* configure encode touchkey */
	gpio_mode_set(GPIOF, GPIO_MODE_INPUT, GPIO_PUPD_PULLDOWN, GPIO_PIN_0);
//	gpio_mode_set(RCU_GPIOA, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO_PIN_3);
	
	
	encode_pin_config();
	
	lcd_pin_config();
	
	adc_pin_config();
	
	com_gpio_init();
}

