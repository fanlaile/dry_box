/****************************************************************************
 * @file     show_lcd.c
 * @version  V1.00
 * @brief    tm1621 lcd driver source file
 * @author	 fan
 * @note
 * Copyright (C) 2023 Creality 3D
*****************************************************************************/
#include "show_lcd.h"
#include "systick.h"

#define nop __NOP();__NOP();//__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();//delay_10us(1)

//__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP(); //宏定义

#define uchar unsigned char				 //数据类型宏定义
#define uint unsigned int				 //数据类型宏定义

/******************TM1621模块命令定义*********************/
#define SYSDIS   0x00	 //关系统振荡器和LCD偏压发生器	
#define SYSEN    0x02    //打开系统振荡器	

#define LCDOFF   0x04     //关LCD偏压
#define LCDON    0x06     //开LCD偏压

#define TONEON   0x12     //打开声音输出
#define TONEOFF  0x10     //关闭声音输出

#define XTAL     0x28     //外部接晶振								 
#define RC       0x30     //内部RC振荡	

#define BIAS     0x52     //1/3偏压 4公共口	

#define WDTDIS   0x0a     //禁止看门狗
#define WDTEN    0x0e     //开启看门狗


/********************定义数据*************************/
//                       h   h    m     m    s    s   t     t    hum hum
unsigned char Smg[10]={0xfb,0x61,0xbd,0xf5,0x67,0xd7,0xdf,0x71,0xff,0xf7};  //0~F字型码
unsigned char Tab0[10]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}; //清屏

void lcd_pin_config(void)
{
	rcu_periph_clock_enable(RCU_GPIOA);
	rcu_periph_clock_enable(RCU_GPIOB);
    /* configure PA6(TIMER2 CH0) as alternate function */
    gpio_mode_set(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7);
	gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7);
    gpio_mode_set(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_1);
	gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,GPIO_PIN_1);
}


/********************从高位写入数据*************************/
void Write_Data_H(uchar Data, uchar Cnt)	   //Data的高cnt位写入TM1621，高位在前
{
	uchar i;
	for(i=0;i<Cnt;i++)
	{
		WRITE(0);
		nop;
		if(Data&0x80) 		                 //从最高位发送
			DATA(1);		
		else 
			DATA(0);
		nop;
		nop;
		WRITE(1);
		Data<<=1;
	}
	WRITE(0);
	DATA(0);
}

/********************从低位写入数据*************************/
void Write_Data_L(uchar Data,uchar Cnt)        //Data 的低cnt位写入TM1621，低位在前 
{ 
	unsigned char i; 
	for(i=0;i<Cnt;i++) 
	{ 
		WRITE(0);
		if(Data&0x01) 	                       //从低位发送
			DATA(1);  
		else 
			DATA(0); 
		nop;
		nop;
		WRITE(1);
		Data>>=1;
	} 
	WRITE(0);
	DATA(0);
} 

/********************写入控制命令*************************/
void WriteCmd(uchar Cmd)
{
	CS(0);
	nop;
	Write_Data_H(0x80,4);     //写入命令标志100
	Write_Data_H(Cmd,8);      //写入命令数据
	CS(1);
	nop;
}

/*********指定地址写入数据，实际写入后4位************/
void WriteOneData(uchar Addr, uchar Data)
{
	CS(0);
	Write_Data_H(0xa0,3);     //写入数据标志101
	Write_Data_H(Addr<<2,6);  //写入地址数据(A5-A4-A3-A2-A1-A0)从高位开始写数据
	Write_Data_L(Data,8);     //写入数据
	CS(1);
	nop;
}

/*********连续写入方式，每次数据为8位，写入数据************/
void WriteAllData(uchar Addr,uchar *p,uchar cnt)
{
	uchar i;
	CS(0);
	Write_Data_H(0xa0,3);      //写入数据标志101
	Write_Data_H(Addr<<2,6);   //写入地址数据
	for(i=0;i<cnt;i++)	    //写入数据
	{
		Write_Data_L(*p,8);	    
		p++;
	}
	CS(1);
	nop;
}

/*******************TM1621初始化**********************/
void TM1621_init()
{
	uchar i;
	CS(1);
	WRITE(1);
	DATA(1);
	nop;
	delay_1ms(2);			
	WriteCmd(BIAS);		 	 //1/3偏压 4公共口
	WriteCmd(RC);			 //内部RC振荡
	WriteCmd(SYSDIS);		 //关系统振荡器和LCD偏压发生器
	WriteCmd(WDTDIS);		 //禁止看门狗
	WriteCmd(SYSEN);		 //打开系统振荡器
	WriteCmd(LCDON);		 //开LCD偏压
	
	delay_1ms(1);
	WriteAllData(0,Tab0,10);
	delay_1ms(1);
	
	for(i=0;i<20;i+=2)
	{
//		Stime[i]=0xfb;
		WriteOneData(i,0xfb);
	}
	
}



