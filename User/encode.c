/****************************************************************************
 * @file     encode.c
 * @version  V1.00
 * @brief    control driver source file
 * @author	 fan
 * @note
 * Copyright (C) 2023 Creality 3D
*****************************************************************************/
#include "encode.h"
#include "count_down.h"
#include "heater.h"
#include "systick.h"
#include <stdio.h>

encode_t ENCODE_t={0,0,4,0};

char *print_msg[]=
{
	"main menu",
	"time set",
	"temperature set",
};

/**
 * @brief    encode IO config
 *
 * @param    None
 *
 * @returns  None
 *
 * @details 
 *
 */
void encode_pin_config(void)
{
	
	/* enable the Tamper key GPIO clock */
    rcu_periph_clock_enable(RCU_GPIOA);
//	rcu_periph_clock_enable(RCU_GPIOF);
	
	
	rcu_periph_clock_enable(RCU_CFGCMP);
	
//	gpio_mode_set(RCU_GPIOA, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO_PIN_2 | GPIO_PIN_3 );
//	gpio_mode_set(RCU_GPIOF, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO_PIN_0 );
    /* enable and set EXTI interrupt */
    
//    
    /* connect EXTI line to GPIO pin */
    syscfg_exti_line_config(EXTI_SOURCE_GPIOA, EXTI_SOURCE_PIN2);

    /* configure EXTI line */
    exti_init(EXTI_2, EXTI_INTERRUPT, EXTI_TRIG_RISING);
    exti_interrupt_flag_clear(EXTI_2);
	nvic_irq_enable(EXTI2_3_IRQn, 1U);
	
	
	ENCODE_t.Previous_Output=!encode_A;
}
/**
 * @brief    menu selection (界面功能选择)
 *
 * @param    None
 *
 * @returns  None
 * 
 * @details menu_start，menu_time，menu_temp共三个可选界面
 *
 */
void Menu_Selection(void)
{
	static uint8_t pressure_flag=0;
	
	if(encode_S==1 && pressure_flag==0)//确认键按下
	{
		delay_1ms(5);
		if(encode_S==1)
		{
			pressure_flag=1;
//			printf("pressure\r\n");
			if(ENCODE_t.encode_menu == menu_start)
			{
				ENCODE_t.encode_menu = ENCODE_t.encode_cnt;
//				printf("%s\r\n",print_msg[ENCODE_t.encode_menu]);
				if(ENCODE_t.encode_menu == menu_time)//设置时间时，将之前剩余的时间清零
				{
					CD.start=0;
//					CD.hour=0;
//					CD.min=0;
//					CD.sec=0;
					
					ENCODE_t.encode_cnt=CD.hour;
					Stime[0]=Smg[ENCODE_t.encode_cnt/10];
					Stime[1]=Smg[ENCODE_t.encode_cnt%10];
					WriteAllData(0,Stime,2);
				}
				else if(ENCODE_t.encode_menu == menu_temp)
				{
					ENCODE_t.encode_cnt=pid.set_tmp;
					Stemp[0]=Smg[ENCODE_t.encode_cnt/10];
					Stemp[1]=Smg[ENCODE_t.encode_cnt%10];
					WriteOneData(temp_h,Stemp[0]);
					WriteOneData(temp_l,Stemp[1]);
				}
				
				
//				ENCODE_t.encode_cnt=0;
			}
			else if(ENCODE_t.encode_menu == menu_time)//确认时间，开始计时
			{
				ENCODE_t.encode_menu = menu_start;
				if(CD.hour==0)
				{
					Stime[0]=Smg[CD.hour/10];
					Stime[1]=Smg[CD.hour%10];
				}
//				ENCODE_t.encode_time = ENCODE_t.encode_cnt;
				
//				CD.start=1;
//				pid.TimerFlag =1;
				CD.hour=ENCODE_t.encode_cnt;
				CD.min=0;
				CD.sec=0;
				if(CD.hour>0)//20230909_new
				{
					CD.start=1;
					pid.TimerFlag =1;
				}
				Time_NumShow(Stime);//fanzq_20230911
				WriteAllData(0,Stime,6);
//				CD.start=1;
//				printf("time set = %d\r\n",ENCODE_t.encode_time);
//				printf("%s\r\n",print_msg[ENCODE_t.encode_menu]);
				ENCODE_t.encode_cnt=0;
			}
			else if(ENCODE_t.encode_menu == menu_temp)//确认温度
			{
				ENCODE_t.encode_menu = menu_start;
				
				WriteOneData(temp_h,Stemp[0]);
				WriteOneData(temp_l,Stemp[1]);
				pid.set_tmp = ENCODE_t.encode_cnt;
				if(CD.start==1)
				{
					pid.TimerFlag =1;
				}
//				ENCODE_t.encode_temp = ENCODE_t.encode_cnt;
//				printf("temperature = %d\r\n",ENCODE_t.encode_temp);
//				printf("%s\r\n",print_msg[ENCODE_t.encode_menu]);
				ENCODE_t.encode_cnt=0;
			}
		}
	}
	else if(encode_S==0 && pressure_flag==1)
	{
		pressure_flag=0;
	}
}


/**
 * @brief    encode driver
 *
 * @param    None
 *
 * @returns  None
 *
 * @details  旋钮数值范围框定
 *
 */
void ENCODE_LOOP(void)
{
	
	
	if(encode_A != ENCODE_t.Previous_Output)
	{
//		ENCODE_t.Previous_Output=encode_A;
		if(encode_B == ENCODE_t.Previous_Output)
		{
			if(ENCODE_t.encode_menu == menu_start)
			{
				if(ENCODE_t.encode_cnt<2)
					ENCODE_t.encode_cnt++;
			}
			else if(ENCODE_t.encode_menu == menu_time)//时间范围0~24
			{
				if(ENCODE_t.encode_cnt<LCD_MAX_TIME)
					ENCODE_t.encode_cnt++;
				
			}
			else if(ENCODE_t.encode_menu == menu_temp)//温度范围45~65
			{
				if(ENCODE_t.encode_cnt<LCD_MIN_TEMP)
					ENCODE_t.encode_cnt=LCD_MIN_TEMP;
				else if(ENCODE_t.encode_cnt<LCD_MAX_TEMP)
					ENCODE_t.encode_cnt++;
			}
		}
		else
		{
			if(ENCODE_t.encode_menu == menu_start)
			{
				if(ENCODE_t.encode_cnt>1)
					ENCODE_t.encode_cnt--;
			}
			else if(ENCODE_t.encode_menu == menu_temp)
			{
				if(ENCODE_t.encode_cnt>LCD_MIN_TEMP)
					ENCODE_t.encode_cnt--;
				else
					ENCODE_t.encode_cnt=LCD_MIN_TEMP;
			}
			else 
			{
				if(ENCODE_t.encode_cnt>LCD_MIN_TIME)
					ENCODE_t.encode_cnt--;
			}
		}
		if(ENCODE_t.encode_menu == menu_time)//刷新设置的时间参数
		{
			Stime[0]=Smg[ENCODE_t.encode_cnt/10];
			Stime[1]=Smg[ENCODE_t.encode_cnt%10];
			WriteAllData(0,Stime,2);
		}
		else if(ENCODE_t.encode_menu == menu_temp)//刷新设置的温度参数
		{
			Stemp[0]=Smg[ENCODE_t.encode_cnt/10];
			Stemp[1]=Smg[ENCODE_t.encode_cnt%10];
			WriteOneData(temp_h,Stemp[0]);
			WriteOneData(temp_l,Stemp[1]);
		}
//		printf("ENCODE_t.encode_cnt=%d,%d\r\n",ENCODE_t.encode_cnt,ENCODE_t.encode_menu);
	}
	else{
		ENCODE_t.Previous_Output=!encode_A;
	}

}

/*!
    \brief      this function handles external lines 2 to 3 interrupt request
    \param[in]  none
    \param[out] none
    \retval     none
*/
void EXTI2_3_IRQHandler(void)
{
    if(RESET != exti_interrupt_flag_get(EXTI_2)){
//		printf("aaa");
		ENCODE_LOOP();
        exti_interrupt_flag_clear(EXTI_2);
    }
}


