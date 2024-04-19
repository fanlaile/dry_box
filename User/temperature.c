/****************************************************************************
 * @file     temperature.c
 * @version  V1.00
 * @brief    control driver source file
 * @author	 fan
 * @note
 * Copyright (C) 2023 Creality 3D
*****************************************************************************/
#include "temperature.h"
#include "systick.h"
#include <stdio.h>

extern uint16_t temp_switch;

const uint16_t ADC_NTC[112] =
{
    2048 ,2003,  //25-25.9
    2003 ,1958,  //26-26.9
    1958 ,1913,  //27-27.9
    1913 ,1869,  //28-28.9
    1869 ,1825,  //29-29.9
    1825 ,1782,  //30-30.9
    1782 ,1739,  //31-31.9
    1739 ,1697,  //32-32.9
    1697 ,1655,  //33-33.9
    1655 ,1614,  //34-34.9
    1614 ,1574,  //35-35.9
    1574 ,1534,  //36-36.9
    1534 ,1495,  //37-37.9
    1495 ,1457,  //38-38.9
    1457 ,1419,  //39-39.9
    1419 ,1382,  //40-40.9
    1382 ,1346,  //41-41.9
    1346 ,1310,  //42-42.9
    1310 ,1275,  //43-43.9
    1275 ,1245,  //44-44.9
	1242 ,1209,  //45
	1209 ,1160,  //46
	1160 ,1144,  //47
	1144 ,1112,  //48
	1112 ,1081,  //49
	1081 ,1051,  //50
	1051 ,1022,  //51
	1022 ,993 ,  //52
	993  ,965 ,  //53
	965  ,938 ,  //54
	938  ,911 ,  //55
	911  ,885 ,  //56
	885  ,860 ,  //57
	860  ,835 ,  //58
	835  ,811 ,  //59
	811  ,788 ,  //60
	788  ,765 ,  //61
	765  ,743 ,  //62
	743  ,722 ,  //63
	722  ,701 ,  //64
	701  ,681 ,  //65
	681  ,661 ,  //66
	661  ,642 ,  //67
	642  ,623 ,  //68
	623  ,605 ,  //69
	605  ,587 ,  //70
	587  ,570 ,  //71
	570  ,554 ,  //72
	554  ,538 ,  //73
	538  ,522 ,  //74
	522  ,507 ,  //75 
	507  ,492 ,  //76
	492  ,478 ,  //77
	478  ,464 ,  //78
	464  ,451 ,  //79
	451  ,438 ,  //80
};
const uint16_t ADC_HMD[8][81] =
{
   //15% 16% 17% 18% 19% 20% 21% 22% 23% 24% 25% 26% 27% 28% 29% 30% 31% 32% 33%  34%  35%  36%	 37%  38%  39%	40%  41%  42%  43%  44%	 45%  46%  47%	48%	 49%  50%  51%	52%	 53%  54%  55%  56%	 57%  58%  59%  60%  61%  62%  63%  64%  65%  66%  67%  68%  69%  70%  71%  72%  73%  74%  75%  76%  77%  78%  79%  80%  81%  82%  83% 	84%	 85%  86%  87%  88%  89%  90%  91%  92%  93%  94%  95%
	{66 ,70 ,73 ,77 ,80 ,84 ,97 ,110,123,135,146,169,198,229,258,278,296,318,239 ,361 ,380 ,430 ,488 ,530 ,579 ,623 ,678 ,722 ,763 ,803 ,841 ,877 ,945 ,1013,1087,1164,1246,1329,1415,1505,1581,1660,1740,1820,1902,1981,2065,2144,2226,2305,2390,2477,2560,2636,2718,2792,2864,2939,3004,3072,3125,3175,3223,3269,3314,3357,3398,3437,3475,3509,3540,3570,3597,3623,3648,3672,3695,3716,3738,3757,3776},//25℃
	{75 ,80 ,85 ,90 ,95 ,100,111,129,146,159,174,193,238,281,306,333,356,378,403 ,428 ,453 ,491 ,547 ,599 ,643 ,689 ,744 ,813 ,884 ,938 ,979 ,1028,1102,1180,1261,1345,1432,1520,1612,1706,1788,1873,1959,2044,2129,2214,2301,2381,2467,2544,2625,2700,2773,2844,2911,2975,3041,3103,3162,3219,3263,3306,3348,3387,3425,3461,3495,3529,3560,3590,3616,3640,3664,3686,3709,3729,3748,3767,3784,3800,3817},//30℃
	{85 ,91 ,97 ,105,111,118,130,149,169,186,207,240,288,323,359,396,421,454,487 ,511 ,538 ,592 ,659 ,699 ,751 ,803 ,865 ,930 ,1032,1097,1161,1197,1278,1362,1449,1537,1629,1723,1817,1914,2001,2089,2178,2267,2358,2442,2528,2608,2694,2773,2837,2904,2968,3026,3088,3145,3200,3251,3302,3349,3386,3423,3458,3490,3522,3553,3582,3609,3636,3661,3683,3704,3723,3742,3761,3778,3794,3810,3825,3838,3853},//35℃
	{96 ,106,116,126,133,140,156,189,218,226,246,291,344,396,438,470,495,530,575 ,605 ,640 ,698 ,748 ,799 ,853 ,910 ,999 ,1127,1181,1280,1350,1382,1469,1557,1649,1742,1836,1932,2027,2122,2214,2305,2400,2487,2576,2659,2742,2824,2904,2982,3034,3092,3145,3197,3247,3294,3340,3383,3424,3464,3495,3525,3554,3582,3608,3633,3657,3680,3702,3723,3741,3759,3775,3791,3806,3821,3835,3848,3860,3872,3884},//40℃
	{107,117,127,137,153,165,190,221,252,271,291,346,408,453,501,556,596,646,686 ,726 ,753 ,799 ,857 ,906 ,985 ,1088,1146,1227,1338,1442,1527,1580,1671,1763,1857,1952,2047,2142,2236,2330,2424,2517,2608,2696,2782,2865,2945,3021,3095,3164,3212,3259,3303,3345,3386,3425,3462,3498,3532,3564,3590,3614,3638,3661,3682,3703,3723,3742,3760,3777,3792,3807,3820,3834,3846,3859,3870,3881,3892,3902,3911},//45℃
	{132,143,154,167,179,195,220,253,287,309,344,405,478,541,596,655,699,740,796 ,842 ,884 ,956 ,1021,1102,1158,1207,1347,1437,1579,1662,1743,1787,1880,1975,2069,2164,2258,2351,2442,2533,2626,1718,2806,2891,2973,3051,3126,3197,3264,3327,3366,3404,3440,3474,3507,3538,3568,3597,3624,3650,3671,3691,3710,3729,3746,3763,3779,3795,3809,3823,3836,3848,3860,3871,3881,3891,3901,3910,3919,3927,3935},//50℃
	{147,158,169,189,200,229,256,291,347,386,406,466,548,612,689,768,803,850,919 ,968 ,1026,1111,1189,1265,1350,1414,1545,1699,1760,1896,1967,2000,2094,2188,2281,2373,2464,2553,2641,2725,2817,2905,2989,3069,3146,3218,3287,3351,3411,3467,3498,3528,3557,3584,3610,3635,3659,3681,3703,3724,3741,3757,3772,3787,3801,3815,3828,3840,3852,3864,3874,3884,3894,3903,3911,3920,3928,3935,3943,3950,3956},//55℃
	{187,199,212,235,252,269,291,333,383,436,477,568,651,732,808,895,954,999,1072,1122,1186,1278,1351,1423,1501,1585,1704,1839,1959,2086,2158,2214,2306,2398,2488,2576,2662,2746,2827,2906,2993,3076,3155,3229,3299,3365,3426,3483,3536,3585,3610,3633,3655,3677,3697,3717,3735,3753,3770,3786,3800,3813,3825,3837,3848,3859,3869,3879,3889,3898,3907,3915,3923,3930,3938,3944,3951,3957,3963,3969,3975},//60℃
};


uint16_t ADC_Value_Inject[2];

void adc_pin_config(void)
{
	rcu_periph_clock_enable(RCU_GPIOA);
	rcu_periph_clock_enable(RCU_GPIOF);
/* config the GPIO as analog mode */
    gpio_mode_set(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO_PIN_0 | GPIO_PIN_1 );
	
	gpio_mode_set(GPIOF, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_1);
	gpio_output_options_set(GPIOF, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,GPIO_PIN_1);
}

/**
 * @brief    ADC configuration function
 *
 * @param    None
 *
 * @returns  None
 *
 * @details 
 *
 */
void adc_config(void)
{
	/* ADCCLK = PCLK2/6 */
    rcu_adc_clock_config(RCU_ADCCK_APB2_DIV6);
	/* enable ADC clock */
    rcu_periph_clock_enable(RCU_ADC);
	/* ADC continous function enable */
    adc_special_function_config(ADC_SCAN_MODE, ENABLE);
    
	/* ADC data alignment config */
    adc_data_alignment_config(ADC_DATAALIGN_RIGHT);
	/* ADC分辨率 12B */
	  adc_resolution_config(ADC_RESOLUTION_12B);
	/* ADC channel length config */
    adc_channel_length_config(ADC_REGULAR_CHANNEL, 2);
    /* ADC inserted channel config */
    adc_regular_channel_config(0U, ADC_CHANNEL_0, ADC_SAMPLETIME_55POINT5);
	adc_regular_channel_config(1U, ADC_CHANNEL_1, ADC_SAMPLETIME_55POINT5);

//    /* ADC external trigger enable */
//    adc_external_trigger_config(ADC_REGULAR_CHANNEL, ENABLE);
//    adc_interrupt_enable(ADC_INT_EOIC);
//    /* ADC external trigger source config */
    adc_external_trigger_source_config(ADC_REGULAR_CHANNEL, ADC_EXTTRIG_REGULAR_NONE);

    /* enable ADC interface */
    adc_enable();
    /* ADC calibration and reset calibration */
    adc_calibration_enable();
    /* ADC interrupt */
//    nvic_irq_enable(ADC_CMP_IRQn, 1);
	
	/* ADC DMA function enable */
    adc_dma_mode_enable();
	/* ADC software trigger enable */
	adc_software_trigger_enable(ADC_REGULAR_CHANNEL);
}
void ADC_DMAConfig(void)  
{
    dma_parameter_struct dma_data_parameter;

	rcu_periph_clock_enable(RCU_DMA);
	rcu_periph_clock_enable(RCU_ADC);
    dma_deinit(DMA_CH0);
    
    dma_data_parameter.periph_addr  = (uint32_t)(&ADC_RDATA);
    dma_data_parameter.periph_inc   = DMA_PERIPH_INCREASE_DISABLE;
    dma_data_parameter.memory_addr  = (uint32_t)(&ADC_Value_Inject);
    dma_data_parameter.memory_inc   = DMA_MEMORY_INCREASE_ENABLE;
    dma_data_parameter.periph_width = DMA_PERIPHERAL_WIDTH_16BIT;
    dma_data_parameter.memory_width = DMA_MEMORY_WIDTH_16BIT;  
    dma_data_parameter.direction    = DMA_PERIPHERAL_TO_MEMORY;
    dma_data_parameter.number       = 2U;
    dma_data_parameter.priority     = DMA_PRIORITY_HIGH;
    dma_init(DMA_CH0, &dma_data_parameter);

    dma_circulation_enable(DMA_CH0);
    dma_memory_to_memory_disable(DMA_CH0);
    dma_channel_enable(DMA_CH0);
}
/**
 * @brief    温度AD采集
 *
 * @param    None
 *
 * @returns  None
 *
 * @details Vout/Rntc=Vcc/(Rntc+R1)   ->Vcc/Vout=(Rntc+R1)/Rntc   Vout=4096*Rntc/(Rntc+R1)
 *
 */

uint16_t Read_temperature(void)
{
	adc_software_trigger_enable(ADC_REGULAR_CHANNEL);
	while( !dma_flag_get(DMA_CH0, DMA_FLAG_FTF));
	/* clear channel0 transfer complete flag */
//	dma_flag_clear(DMA_CH0, DMA_FLAG_FTF);
	return  ADC_Value_Inject[1];
}
/**
 * @brief    AD转温度
 *
 * @param    None
 *
 * @returns  None
 *
 * @details 
 *
 */

uint8_t ADC_TO_TEMP(uint16_t AD_value)
{
	uint16_t temp_value;
	uint8_t i;
	for(i=0;i<56;i++)
	{
		if(AD_value>ADC_NTC[0])
		{
			return 24;
		}
		else if(AD_value <= ADC_NTC[0+(i*2)] && AD_value > ADC_NTC[1+(i*2)])
		{
			temp_value=25+i;
			return  temp_value;
		}
		else if(AD_value<ADC_NTC[111])
		{
			return 81;
		}
	}
	
	return  0;
}



/**
 * @brief    湿度AD采集
 *
 * @param    None
 *
 * @returns  None
 *
 * @details  高电平时间(充电时间)，为 500uS，低电平时间（放电时间）要求大于 2mS，在信号的正周期内的中间段进行采样（250uS），
直接通过分压法采集，注意采样时间，通过电压采样后得到 V0，计算 RX 的值，公式如下：RX=R0*(V-V0)/V0
 *
 */

uint16_t Read_humidity(void)
{
	IO3V3(0);
	delay_1ms(3);
	IO3V3(1);
	delay_10us(25);
//	IO3V3(0);
	adc_software_trigger_enable(ADC_REGULAR_CHANNEL);
	while( !dma_flag_get(DMA_CH0, DMA_FLAG_FTF));
	return  ADC_Value_Inject[0];
}
/**
 * @brief    湿度计算
 *
 * @param    None
 *
 * @returns  None
 *
 * @details 
 *
 */

uint8_t Hmd_calculate(void)
{
	uint16_t adcVal = 0;
	int8_t i = 0;
	uint8_t tempVal = 0;
	uint8_t row = 0;
	adcVal = Read_humidity()*2;//当前湿度ADC值*灵敏系数
	if (!adcVal)					
	{
		return 0xff;				//AD转换未完成
	}
	tempVal = ADC_TO_TEMP(Read_temperature());	//获取温度值
	row = (tempVal/5)-5;
	if(row>7)row=7;
	
	for(i=0;i<81;i++)
	{
		if(adcVal<=ADC_HMD[row][i])
		{
			return (15 + i ) ;
		}
	}
	
	return  0xff;
}
/**
 * @brief     计算温湿度，并存储到lcd缓存器
 *
 * @param    None
 *
 * @returns  None
 *
 * @details 
 *
 */
void Temp_Hum_To_LCD(void)
{
	uint8_t t,h;
	t=ADC_TO_TEMP(Read_temperature());
	pid.actual_tmp=t;
	h=Hmd_calculate();
	if(t>75)
	{
		pid.TimerFlag=0;
		PWM_OUT(0);
	}
	if(ENCODE_t.encode_menu!=menu_temp)
	{
//		if(temp_switch<5 || pid.TimerFlag == 0)
		{
			Stemp[0]=Smg[t/10];
			Stemp[1]=Smg[t%10];
		}
//		else
//		{
//			if(pid.set_tmp!=0)
//			{
//				Stemp[0]=Smg[(int)pid.set_tmp/10];
//				Stemp[1]=Smg[(int)pid.set_tmp%10];
//			}
//		}
	}
	if(h!=0xff)
	{
		Shumi[0]=Smg[h/10];
		Shumi[1]=Smg[h%10];
	}
}

////ADC中断函数
//void ADC_CMP_IRQHandler(void)
//{
///* clear the ADC interrupt or status flag */
//  adc_interrupt_flag_clear(ADC_INT_EOIC);
//  ADC_Value_Inject[0] = adc_inserted_data_read(ADC_INSERTED_CHANNEL_0);
// 
//}






