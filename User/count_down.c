/****************************************************************************
 * @file     count_down.c
 * @version  V1.00
 * @brief    time count driver source file
 * @author	 fan
 * @note
 * Copyright (C) 2023 Creality 3D
*****************************************************************************/
#include "count_down.h"
#include <stdio.h>

Time_t CD={0,0,0,0,0,};

unsigned char Stime[6]={0xfb,0xfb,0xfb,0xfb,0xfb,0xfb};
unsigned char Stemp[2]={0xfb,0xfb};
unsigned char Shumi[2]={0xfb,0xfb};

uint16_t temp_switch=0;

#define TIME_END 1

/**
 * @brief    定时器2初始化
 *
 * @param    prescale时钟分频系数，arr定时器比较值
 *
 * @returns  None
 *
 * @details 
 *
 */
void Time2_Init(uint32_t prescale,uint32_t arr)
{
	timer_parameter_struct timer_initpara;
	rcu_periph_clock_enable(RCU_TIMER2);

    timer_deinit(TIMER2);
	
	 /* TIMER configuration */
    timer_initpara.prescaler         = prescale;//7199;
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = arr;//10000;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;
    timer_init(TIMER2,&timer_initpara);
	
	/* auto-reload preload enable */
    timer_auto_reload_shadow_enable(TIMER2);
	nvic_irq_enable(TIMER2_IRQn, 3);				//TIMER2中断设置，抢占优先级3，子优先级3 
	timer_interrupt_enable(TIMER2, TIMER_INT_UP); 	//更新中断
    /* auto-reload preload enable */
    timer_enable(TIMER2);
}

/**
 * @brief    
 *
 * @param    None
 *
 * @returns  None
 *
 * @details 
 *
 */
void Time_NumShow(unsigned char *t_array)
{
	t_array[0]=Smg[CD.hour/10];
	t_array[1]=Smg[CD.hour%10];
	t_array[2]=Smg[CD.min/10];
	t_array[3]=Smg[CD.min%10];
	t_array[4]=Smg[CD.sec/10];
	t_array[5]=Smg[CD.sec%10];
}
/**
 * @brief    定时器T0中断，0.5s产生一次
 *
 * @param    None
 *
 * @returns  None
 *
 * @details 
 *
 */
void TIMER2_IRQHandler(void)
{
	static uint8_t choose_now=0,choose_last=0,choose_cnt=0;
//	static uint8_t tichoose_now=0,tichoose_last=0,tichoose_cnt=0;
//	static uint8_t techoose_now=0,techoose_last=0,techoose_cnt=0;
	static int8_t twinkle_flag=0xff;
	static uint8_t time_count=0,show_flag=0;
	static int8_t fan_delay=40;
	if(SET == timer_interrupt_flag_get(TIMER2, TIMER_INT_FLAG_UP)){
		/* clear channel 0 interrupt bit */
//		adc_software_trigger_enable(ADC_REGULAR_CHANNEL);
		time_count++;
		twinkle_flag=~twinkle_flag;
		if(time_count>1)time_count=0;
		timer_interrupt_flag_clear(TIMER2, TIMER_INT_FLAG_UP);
		if(CD.start)
		{
			gpio_bit_write(GPIOA,GPIO_PIN_13,1);
			fan_delay=0;
			if(time_count==1)//1s进入一次
			{
				Time_NumShow(Stime);
//				printf("CountDown:%dH:%dM:%ds",CD.hour,CD.min,CD.sec);
				WriteAllData(0,Stime,6);
				if(CD.sec==0)
				{
					CD.sec=60;
					if(CD.min==0)
					{
						CD.min=60;
						
						if(CD.hour==0)
						{
							CD.finish=TIME_END;
							pid.TimerFlag=0;
							CD.start=0;
						}
						CD.hour--;
					}
					CD.min--;
				}
				CD.sec--;
			}
		}
		else
		{
			pid.TimerFlag =0;//20230909_new
			PWM_OUT(0);
			if(fan_delay<40)//20230909_new
			{
				fan_delay++;
			}
			else
			{
				gpio_bit_write(GPIOA,GPIO_PIN_13,0);
			}
		}
		
		if(ENCODE_t.encode_menu==menu_start && ENCODE_t.encode_cnt!=0)
		{
			show_flag=1;
			choose_now=ENCODE_t.encode_cnt;
			if(ENCODE_t.encode_cnt == menu_time)
			{
				
				if(twinkle_flag)
					Stime[0]|=0x01;
				else
					Stime[0]&=0xfe;
				Stemp[0]|=0x01;
				WriteOneData(temp_h,Stemp[0]);
				WriteOneData(0,Stime[0]);
			}
			else if(ENCODE_t.encode_cnt == menu_temp)
			{
				if(twinkle_flag)
					Stemp[0]|=0x01;
				else
					Stemp[0]&=0xfe;
				Stime[0]|=0x01;
				WriteOneData(0,Stime[0]);
				WriteOneData(temp_h,Stemp[0]);
//				WriteOneData(0,Stime[13]|=~twinkle_flag);
			}
		}
		else 
		{
			if(show_flag == 1)
			{
				show_flag=0;
				Stime[0]|=0x01;
				Stemp[0]|=0x01;
				WriteAllData(0,Stime,6);
				WriteOneData(temp_h,Stemp[0]);
				WriteOneData(temp_l,Stemp[1]);
			}
		}
		/**/
		if(choose_last!=choose_now )
		{
			choose_last=choose_now;
			choose_cnt=0;
		}
		else
		{
			if(show_flag==1)
			{
				choose_cnt++;
				if(choose_cnt>=20)
				{
					ENCODE_t.encode_menu=menu_start;
					ENCODE_t.encode_cnt=0;
					choose_cnt=0;
				}
			}
		}
		
		
		if(ENCODE_t.encode_menu==menu_time)
		{
//			show_flag=1;
			show_flag=1;
//			tichoose_now=ENCODE_t.encode_cnt;
			if(twinkle_flag)
			{
				WriteAllData(0,Stime,2);		
			}
			else
			{
				WriteAllData(0,Tab0,2);
			}
		}
		else if(ENCODE_t.encode_menu==menu_temp)
		{
//			show_flag=1;
			show_flag=1;
//			techoose_now=ENCODE_t.encode_cnt;
			if(twinkle_flag)
			{
				WriteOneData(temp_h,Stemp[0]);
				WriteOneData(temp_l,Stemp[1]);
			}
			else
			{
				WriteOneData(temp_h,Tab0[0]);
				WriteOneData(temp_l,Tab0[0]);
			}
		}
		/**/
//		if(tichoose_last!=tichoose_now )
//		{
//			tichoose_last=tichoose_now;
//			tichoose_cnt=0;
//		}
//		else
//		{
//			if(show_flag==1)
//			{
//				tichoose_cnt++;
//				if(tichoose_cnt>=20)
//				{
//					ENCODE_t.encode_menu=menu_start;
//					ENCODE_t.encode_cnt=0;
//					tichoose_cnt=0;
//					if(CD.hour>0 || CD.min>0 || CD.sec>0)
//					{
//						CD.start=1;
//					}
////					CD.start=1;
//				}
//			}
//		}
//		
//		if(techoose_last!=techoose_now )
//		{
//			techoose_last=techoose_now;
//			techoose_cnt=0;
//		}
//		else
//		{
//			if(show_flag==1)
//			{
//				techoose_cnt++;
//				if(techoose_cnt>=20)
//				{
//					ENCODE_t.encode_menu=menu_start;
//					ENCODE_t.encode_cnt=0;
//					techoose_cnt=0;
////					CD.start=1;
//				}
//			}
//		}
//		
		
		if(time_count==1)
		{
		
			if(ENCODE_t.encode_menu!=menu_temp)// && ENCODE_t.encode_menu!=menu_start )//&& ENCODE_t.encode_cnt!=2))
			{
				if(ENCODE_t.encode_menu==menu_start && ENCODE_t.encode_cnt!=2)
				{
					WriteOneData(temp_h,Stemp[0]);//1s刷新一次温湿度
					WriteOneData(temp_l,Stemp[1]);
				}
				
			}
			WriteOneData(humi_h,Shumi[0]);
			WriteOneData(humi_l,Shumi[1]);
			
//			temp_switch++;
//			if(temp_switch>10)
//				temp_switch=0;
		}
		
	}
}

