/****************************************************************************
 * @file     uart_debug.c
 * @version  V1.00
 * @brief    uart command source file
 * @author	 fan
 * @note
 * Copyright (C) 2023 Creality 3D
*****************************************************************************/
#include "uart_debug.h"
#include "temperature.h"
#include "count_down.h"
#include "heater.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/*---------------------------------------------------------------------------------------------------------*/
/* Global variables                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
uint8_t transmitter_buffer[128];
uint8_t receiver_buffer[128];
uint8_t transfersize = 128;
uint8_t receivesize = 128;
__IO uint8_t txcount = 0; 
__IO uint16_t rxcount = 0; 
uint8_t Rx_flag = 0; 
int32_t str_to_num;

/*---------------------------------------------------------------------------------------------------------*/
/* usart cmd call back function                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
int8_t _helpf(uint8_t* s);
int8_t _set_timef(uint8_t* s);
int8_t _fanonf(uint8_t* s);
int8_t _fanofff(uint8_t* s);
int8_t _tempf(uint8_t* s);
int8_t _ctf(uint8_t* s);
int8_t _hmdf(uint8_t* s);
int8_t _heatonf(uint8_t* s);
int8_t _heatofff(uint8_t* s);
int8_t _sftf(uint8_t* s);
int8_t _sfatf(uint8_t* s);
/*---------------------------------------------------------------------------------------------------------*/
/* usart cmd struct                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
typedef struct {
	uint8_t priority;
	char *cmd;
	int8_t (*f)(uint8_t* s);
}cmd_T;

cmd_T CMD_T[] = {
	{0,	"help\r\n"		,_helpf			},
//	{1,	"set time:"		,_set_timef		},
//	{0,	"fan on\r\n"	,_fanonf		},
//	{0,	"fan off\r\n"	,_fanofff		},
	{0,	"temp?\r\n"		,_tempf			},
	{0,	"CD\r\n"		,_ctf			},
	{0,	"hmd?\r\n"		,_hmdf			},
//	{0,	"heat on\r\n"	,_heatonf		},
//	{0,	"heat off\r\n"	,_heatofff		},
//	{1,	"set_false_T="	,_sftf			},
//	{1,	"set_false_AT="	,_sfatf			},
};
uint8_t cmd_num = sizeof(CMD_T)/sizeof(CMD_T[0]);


/**
 * @brief    retarget the C library printf function to the USART
 *
 * @param    None
 *
 * @returns  None
 *
 * @details 
 *
 */
int fputc(int ch, FILE *f)
{
    usart_data_transmit(USART0, (uint8_t) ch);
    while(RESET == usart_flag_get(USART0, USART_FLAG_TBE));
    return ch;
}
/**
 * @brief    串口IO初始化
 *
 * @param    None
 *
 * @returns  None
 *
 * @details 
 *
 */
void com_gpio_init(void)
{
    /* enable COM GPIO clock */
    rcu_periph_clock_enable(RCU_GPIOA);

    /* connect port to USARTx_Tx */
    gpio_af_set(GPIOA, GPIO_AF_1, GPIO_PIN_9);

    /* connect port to USARTx_Rx */
    gpio_af_set(GPIOA, GPIO_AF_1, GPIO_PIN_10);

    /* configure USART Tx as alternate function push-pull */
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_9);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_10MHZ, GPIO_PIN_9);

    /* configure USART Rx as alternate function push-pull */
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_10);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_10MHZ, GPIO_PIN_10);
}
/**
 * @brief    串口初始化
 *
 * @param    None
 *
 * @returns  None
 *
 * @details 
 *
 */
void Usart_Init(void)
{
	/* 使能USART时钟*/
 
    rcu_periph_clock_enable(RCU_USART0);
	
	/* USART 配置*/
 
    usart_deinit(USART0);
 
	usart_word_length_set(USART0, USART_WL_8BIT);
    usart_stop_bit_set(USART0, USART_STB_1BIT);
    usart_parity_config(USART0, USART_PM_NONE);
	
    usart_baudrate_set(USART0,115200U);
 
    usart_receive_config(USART0, USART_RECEIVE_ENABLE);
 
    usart_transmit_config(USART0, USART_TRANSMIT_ENABLE);
	
	usart_interrupt_enable(USART0, USART_INT_RBNE);
	
	nvic_irq_enable(USART0_IRQn, 0);

	usart_enable(USART0);
	
	
}

/**
 * @brief    字符串比较函数
 *
 * @param    None
 *
 * @returns  None
 *
 * @details 
 *
 */
int8_t me_strcmp(uint8_t *str1, char *str2, uint8_t mode)
{
	uint8_t i,len,result=1;
	
	
	if(mode == 0)
		len = strlen((char*)str1);
	else
		len = strlen((char*)str2);
	
	for(i=0;i<len;i++)
	{
		if(str1[i]!=str2[i])
		{
			result = 0;
		}
	}
	return result;
}

/**
 * @brief    Uart command 
 *
 * @param    None
 *
 * @returns  None
 *
 * @details  串口命令处理函数
 *
 */
void UART_TEST_HANDLE(void)
{
	uint8_t i;
    if(Rx_flag)
	{
		for(i=0;i<cmd_num;i++)
		{
			if(me_strcmp(receiver_buffer,CMD_T[i].cmd ,CMD_T[i].priority)==1)
			{
				CMD_T[i].f(receiver_buffer);
				
				Rx_flag = 0;
				
				memset(receiver_buffer,0,sizeof(receiver_buffer));
				return;
			}
		}
		Rx_flag = 0;
				
		memset(receiver_buffer,0,sizeof(receiver_buffer));
		
	}
	
	
}

/*---------------------------------------------------------------------------------------------------------*/
/* ISR to handle UART Channel 0 interrupt event                                                            */
/*---------------------------------------------------------------------------------------------------------*/
void USART0_IRQHandler(void)
{
	if(RESET != usart_interrupt_flag_get(USART0, USART_INT_FLAG_RBNE)){
        /* receive data */
        receiver_buffer[rxcount] = usart_data_receive(USART0);
//        if(Rxcount == receivesize){
//            usart_interrupt_disable(USART0, USART_INT_RBNE);
//        }
		if(receiver_buffer[rxcount]==0x0a)
		{
			Rx_flag=1;
			rxcount=0;
		}
		else
		{
			rxcount++;
		}
    }

    if(RESET != usart_interrupt_flag_get(USART0, USART_INT_FLAG_TBE)){
        /* transmit data */
        usart_data_transmit(USART0, transmitter_buffer[txcount++]);
        if(txcount == transfersize){
            usart_interrupt_disable(USART0, USART_INT_TBE);
        }
    }
}
/**
 * @brief    从字符串中提取数字 
 *
 * @param    None
 *
 * @returns  None 
 *
 * @details  
 *
 */
void Get_Num(uint8_t *str,char ch)
{
	
	char *s;
	str_to_num=0;
	
	if(strchr((char*)str,ch) != NULL)
	{
		s = strchr((char*)str,ch)+1;
//		printf("str = %s",s);
		str_to_num=atoi(s);
	}

}

/**
 * @brief    UART Callback function 
 *
 * @param    None
 *
 * @returns  None 
 *
 * @details  指令处理回调函数
 *
 */
int8_t _helpf(uint8_t* s)
{
	printf("hello world!\r\n");
	return 1;
}
int8_t _set_timef(uint8_t* s)
{
	CD.start=1;
	CD.hour=2;
	CD.min=0;
	CD.sec=0;
	return 1;
}
int8_t _fanonf(uint8_t* s)
{
	gpio_bit_write(GPIOA,GPIO_PIN_13,1);
	
	printf("Fan ON\r\n");
	return 1;
}
int8_t _fanofff(uint8_t* s)
{
	gpio_bit_write(GPIOA,GPIO_PIN_13,0);
	
	printf("Fan OFF\r\n");
	return 1;
}

int8_t _tempf(uint8_t* s)
{
	printf("tempe ADC=%d\r\n",Read_temperature());//ADC_TO_TEMP
	printf("temperature=%d \r\n",ADC_TO_TEMP(Read_temperature()));
	return 1;
}
int8_t _ctf(uint8_t* s)
{
	printf("OK\r\n");
	CD.start=1;
	CD.hour=2;
	CD.min=0;
	CD.sec=0;
	return 1;
}
int8_t _hmdf(uint8_t* s)
{
	printf("hmd ADC=%d\r\n",Read_humidity());
	printf("humidity=%d%%\r\n",Hmd_calculate());//Hmd_calculate
	
	return 1;
}
int8_t _heatonf(uint8_t* s)
{
//	gpio_bit_write(GPIOA,GPIO_PIN_4,1);
	PWM_OUT(1);
	printf("Heat ON\r\n");
	return 1;
}
int8_t _heatofff(uint8_t* s)
{
//	gpio_bit_write(GPIOA,GPIO_PIN_4,0);
	PWM_OUT(0);
	printf("Heat OFF\r\n");
	return 1;
}
int8_t _sftf(uint8_t* s)
{
	Get_Num(s,'=');
//	printf("num = %d\r\n",str_to_num);
//	pid.TimerFlag =1;
//	pid.set_tmp =str_to_num;
//	printf("false temp = %d\r\n",(int)pid.set_tmp );
	return 1;
}
int8_t _sfatf(uint8_t* s)
{
	Get_Num(s,'=');
//	printf("num = %d\r\n",str_to_num);
//	pid.actual_tmp =str_to_num;
//	printf("false atemp = %d\r\n",(int)pid.actual_tmp);
	return 1;
}
