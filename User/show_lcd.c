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

//__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP(); //�궨��

#define uchar unsigned char				 //�������ͺ궨��
#define uint unsigned int				 //�������ͺ궨��

/******************TM1621ģ�������*********************/
#define SYSDIS   0x00	 //��ϵͳ������LCDƫѹ������	
#define SYSEN    0x02    //��ϵͳ����	

#define LCDOFF   0x04     //��LCDƫѹ
#define LCDON    0x06     //��LCDƫѹ

#define TONEON   0x12     //���������
#define TONEOFF  0x10     //�ر��������

#define XTAL     0x28     //�ⲿ�Ӿ���								 
#define RC       0x30     //�ڲ�RC��	

#define BIAS     0x52     //1/3ƫѹ 4������	

#define WDTDIS   0x0a     //��ֹ���Ź�
#define WDTEN    0x0e     //�������Ź�


/********************��������*************************/
//                       h   h    m     m    s    s   t     t    hum hum
unsigned char Smg[10]={0xfb,0x61,0xbd,0xf5,0x67,0xd7,0xdf,0x71,0xff,0xf7};  //0~F������
unsigned char Tab0[10]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}; //����

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


/********************�Ӹ�λд������*************************/
void Write_Data_H(uchar Data, uchar Cnt)	   //Data�ĸ�cntλд��TM1621����λ��ǰ
{
	uchar i;
	for(i=0;i<Cnt;i++)
	{
		WRITE(0);
		nop;
		if(Data&0x80) 		                 //�����λ����
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

/********************�ӵ�λд������*************************/
void Write_Data_L(uchar Data,uchar Cnt)        //Data �ĵ�cntλд��TM1621����λ��ǰ 
{ 
	unsigned char i; 
	for(i=0;i<Cnt;i++) 
	{ 
		WRITE(0);
		if(Data&0x01) 	                       //�ӵ�λ����
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

/********************д���������*************************/
void WriteCmd(uchar Cmd)
{
	CS(0);
	nop;
	Write_Data_H(0x80,4);     //д�������־100
	Write_Data_H(Cmd,8);      //д����������
	CS(1);
	nop;
}

/*********ָ����ַд�����ݣ�ʵ��д���4λ************/
void WriteOneData(uchar Addr, uchar Data)
{
	CS(0);
	Write_Data_H(0xa0,3);     //д�����ݱ�־101
	Write_Data_H(Addr<<2,6);  //д���ַ����(A5-A4-A3-A2-A1-A0)�Ӹ�λ��ʼд����
	Write_Data_L(Data,8);     //д������
	CS(1);
	nop;
}

/*********����д�뷽ʽ��ÿ������Ϊ8λ��д������************/
void WriteAllData(uchar Addr,uchar *p,uchar cnt)
{
	uchar i;
	CS(0);
	Write_Data_H(0xa0,3);      //д�����ݱ�־101
	Write_Data_H(Addr<<2,6);   //д���ַ����
	for(i=0;i<cnt;i++)	    //д������
	{
		Write_Data_L(*p,8);	    
		p++;
	}
	CS(1);
	nop;
}

/*******************TM1621��ʼ��**********************/
void TM1621_init()
{
	uchar i;
	CS(1);
	WRITE(1);
	DATA(1);
	nop;
	delay_1ms(2);			
	WriteCmd(BIAS);		 	 //1/3ƫѹ 4������
	WriteCmd(RC);			 //�ڲ�RC��
	WriteCmd(SYSDIS);		 //��ϵͳ������LCDƫѹ������
	WriteCmd(WDTDIS);		 //��ֹ���Ź�
	WriteCmd(SYSEN);		 //��ϵͳ����
	WriteCmd(LCDON);		 //��LCDƫѹ
	
	delay_1ms(1);
	WriteAllData(0,Tab0,10);
	delay_1ms(1);
	
	for(i=0;i<20;i+=2)
	{
//		Stime[i]=0xfb;
		WriteOneData(i,0xfb);
	}
	
}



