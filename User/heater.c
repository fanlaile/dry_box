/****************************************************************************
 * @file     heater.c
 * @version  V1.00
 * @brief    heater driver source file
 * @author	 fan
 * @note
 * Copyright (C) 2023 Creality 3D
*****************************************************************************/
#include "heater.h"
#include "encode.h"
#include <stdio.h>

PID_PARA pid;




void Timer13_Init(void)
{
	timer_parameter_struct timer_initpara;
	rcu_periph_clock_enable(RCU_TIMER13);

    timer_deinit(TIMER13);
	
	 /* TIMER configuration */
    timer_initpara.prescaler         = 71;//10ms//7199 1000ms
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = 10000;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;
    timer_init(TIMER13,&timer_initpara);
	
	/* auto-reload preload enable */
    timer_auto_reload_shadow_enable(TIMER13);
	nvic_irq_enable(TIMER13_IRQn, 3);				//TIMER13�ж����ã���ռ���ȼ�3�������ȼ�3 
	timer_interrupt_enable(TIMER13, TIMER_INT_UP); 	//�����ж�
    /* auto-reload preload enable */
    timer_enable(TIMER13);
}

void pid_init(void)
{
    pid.set_tmp = 0;
    pid.actual_tmp = 0;
    pid.error = 0.0;
    pid.error_next = 0.0;
    pid.error_last = 0.0;
    //�ɵ���PID ������ʹ�������������ӽ���Ծ����200.0 // 
    pid.kp = 80;//40
    pid.ki = 1.2;//0.8;
    pid.kd = 0.8;
	pid.PIDOut=0;
	pid.PIDOutnext=0;
	pid.PWMCnt=0;
	pid.TimerFlag=0;
	pid.TimerCnt=0;
}


void pid_realise(float temperature)//ʵ��pid  
{
//	static uint8_t t_flag=0;
	
	int16_t Kpout,Tiout,Tdout,Pidout;
	
    pid.set_tmp = temperature;//����Ŀ���ٶ�  
	if(pid.set_tmp<45)//20230909_new
	{
		pid.TimerFlag=0;
	}
    pid.error = pid.set_tmp - pid.actual_tmp;
	if(pid.error>=0)//fanzq_20231007
	{
		if(pid.error>10)
		{
			pid.PIDOut = 100;
		}
		else
		{
			//�������
			Kpout = pid.kp * (pid.error - pid.error_next);
			//�������
			Tiout = pid.ki * pid.error;
			//΢�����
			Tdout = pid.kd*(pid.error-2*pid.error_next+pid.error_last);//�������㹫ʽ 

			//����PID���
			Pidout = Kpout + Tiout + Tdout + pid.PIDOutnext;	
			
			//�������
			if(Pidout < 0)								{pid.PIDOut = 0;}
			else if(Pidout > 100)						{pid.PIDOut = 100;}
			else										{pid.PIDOut = Pidout;}
			
			
			
			pid.error_last = pid.error_next;//��һ�ε���  
			pid.error_next = pid.error;
			pid.PIDOutnext = pid.PIDOut;
		}
	}
	else//fanzq_20231007
	{
		pid.PIDOut=0;
	}
//    printf("PIDOut = %.2f , set_tmp = %d , actual_tmp = %d\r\n",pid.PIDOut,pid.set_tmp,pid.actual_tmp);
	
//	if(pid.TimerFlag == 1)
//	{
//		if(pid.set_tmp - pid.actual_tmp > 10 && t_flag==0)
//		{
//			t_flag=1;
//			PWM_OUT(1);
//		}
//		if((pid.actual_tmp - pid.set_tmp) > 0 && t_flag==1)
//		{
//			t_flag=2;
//			PWM_OUT(0);
//		}
//		if(pid.set_tmp - pid.actual_tmp > 5)
//		{
//			t_flag=1;
//			PWM_OUT(1);
//		}
//	}
//	else
//	{
//		t_flag=0;
//		PWM_OUT(0);
//	}

}

/**
 * @brief    ��ʱ��T1�жϣ�1s����һ��
 *
 * @param    None
 *
 * @returns  None
 *
 * @details 
 *
 */
void TIMER13_IRQHandler(void)
{
	static uint8_t sec_cnt=0;
//	P24 = ~P24 ;
	//PID����
	if(SET == timer_interrupt_flag_get(TIMER13, TIMER_INT_FLAG_UP)){
		if(pid.TimerFlag == 1 )
		{
			
			if(pid.TimerCnt >= 100) //1s����һ��
			{
//				pid_realise(ENCODE_t.encode_temp);//ֱ�Ӽ���
				pid_realise(pid.set_tmp);
				pid.TimerCnt = 0;
			}
			else
			{
				pid.TimerCnt++;
			} 
		}
		
		if(pid.TimerFlag == 1)
		{
			//PWM���
			PWM_OUT((pid.PWMCnt < pid.PIDOut) ? 1:0);
			//����ֵ����
			pid.PWMCnt = (pid.PWMCnt < 100) ? pid.PWMCnt+1:0;
		}
		else
		{
			//�ر�PWM���
			PWM_OUT(0);
		}
//		sec_cnt++;
//		if(sec_cnt==60 )
//		{
//			pid_realise(pid.set_tmp);
//			sec_cnt=0;
//		}
		
		
		/* clear channel 0 interrupt bit */
		timer_interrupt_flag_clear(TIMER13, TIMER_INT_FLAG_UP);
	}
}



