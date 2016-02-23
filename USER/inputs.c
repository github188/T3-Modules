
#include "define.h"
#include "inputs.h"

#include "delay.h"
#include "modbus.h"
#include "../filter/filter.h"
#include "ud_str.h"
#include "RANGE.H"
#if (defined T322AI) || (T38AI8AO6DO)	
uint16_t data_change[MAX_AI_CHANNEL] = {0} ;
#endif

#ifndef T3PT12
void range_set_func(u8 range) ;
 
#define ADC_DR_ADDRESS  0x4001244C  

vu16 AD_Value[MAX_AI_CHANNEL]; 
#endif
 

void inputs_io_init(void)
{
	//ADC_RegularChannelConfig(ADC1, ADC_Channel_0|ADC_Channel_1, 1, ADC_SampleTime_28Cycles5);
// IO Configure 
	
GPIO_InitTypeDef GPIO_InitStructure;
#ifdef T322AI	
/**************************PortA configure---ADC1*****************************************/
////RCC_APB2PeriphClockCmd( RCC_APB2Periph_ADC1|RCC_APB2Periph_GPIOA, ENABLE);
//GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;  
//GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN; 
//GPIO_Init(GPIOA, &GPIO_InitStructure);

GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4;  
GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;	
GPIO_Init(GPIOA, &GPIO_InitStructure);

GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_9;  
GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;	
GPIO_Init(GPIOC, &GPIO_InitStructure);	
///**************************PortB configure----ADC1*****************************************/
//RCC_APB2PeriphClockCmd( RCC_APB2Periph_ADC1|RCC_APB2Periph_GPIOB, ENABLE);
//GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;  
//GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN; 
//GPIO_Init(GPIOB, &GPIO_InitStructure);
#endif

#ifdef T38AI8AO6DO
GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_7;  
GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;	
GPIO_Init(GPIOA, &GPIO_InitStructure);

GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3;  
GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
GPIO_Init(GPIOC, &GPIO_InitStructure);


#endif
/**************************PortC configure----ADC1*****************************************/
RCC_APB2PeriphClockCmd( RCC_APB2Periph_ADC1|RCC_APB2Periph_GPIOC, ENABLE);
GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;  
GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN; 
GPIO_Init(GPIOC, &GPIO_InitStructure);




	
}
void inputs_adc_init(void)
{
	ADC_InitTypeDef ADC_InitStructure;
	
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	/* configuration ------------------------------------------------------*/  
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;//
//	ADC_InitStructure.ADC_ScanConvMode = ENABLE;//DMA CONTINUS MODE
//	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;//
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//
//	#ifdef T322AI
	ADC_InitStructure.ADC_NbrOfChannel = 1;//
	ADC_Init(ADC1, &ADC_InitStructure);
//	ADC_Init(ADC2, &ADC_InitStructure);
	 /* ADC1 regular channels configuration */ 
////	ADC_RegularChannelConfig(ADC1, ADC_Channel_4 , 1, ADC_SampleTime_239Cycles5);  
////	ADC_RegularChannelConfig(ADC1, ADC_Channel_5 , 2, ADC_SampleTime_239Cycles5); 
////	ADC_RegularChannelConfig(ADC1, ADC_Channel_6 , 3, ADC_SampleTime_239Cycles5);
////	ADC_RegularChannelConfig(ADC1, ADC_Channel_7 , 4, ADC_SampleTime_239Cycles5);
////	ADC_RegularChannelConfig(ADC1, ADC_Channel_8 , 5, ADC_SampleTime_239Cycles5);
////	ADC_RegularChannelConfig(ADC1, ADC_Channel_9 , 6, ADC_SampleTime_239Cycles5);
////	ADC_RegularChannelConfig(ADC1, ADC_Channel_10 , 7, ADC_SampleTime_239Cycles5);
////	ADC_RegularChannelConfig(ADC1, ADC_Channel_11 , 8, ADC_SampleTime_239Cycles5);
////	ADC_RegularChannelConfig(ADC1, ADC_Channel_12 , 9, ADC_SampleTime_239Cycles5);
////	ADC_RegularChannelConfig(ADC1, ADC_Channel_13 , 10, ADC_SampleTime_239Cycles5);
////	ADC_RegularChannelConfig(ADC1, ADC_Channel_14 , 11, ADC_SampleTime_239Cycles5);
//	ADC_DMACmd(ADC1, ENABLE); //enable dma
//	ADC_Cmd(ADC2, ENABLE); 
//	ADC_ResetCalibration(ADC2);
//	while(ADC_GetResetCalibrationStatus(ADC2) == SET);
//	ADC_StartCalibration(ADC2);
//	while(ADC_GetCalibrationStatus(ADC2) == SET);
//	#endif
	ADC_Cmd(ADC1, ENABLE); 
	/* Enable ADC1 reset calibaration register */   
	ADC_ResetCalibration(ADC1);
	while(ADC_GetResetCalibrationStatus(ADC1)== SET)
	{
		;
	}		
//	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
     ADC_StartCalibration(ADC1);
     while(ADC_GetCalibrationStatus(ADC1) == SET);
}

//void dma_adc_init(void)
//{
///************DMA configure*******************************/
//DMA_InitTypeDef DMA_InitStructure;
//DMA_DeInit(DMA1_Channel1);
//DMA_InitStructure.DMA_PeripheralBaseAddr = ADC_DR_ADDRESS;
//DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&AD_Value  ;
//DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
//DMA_InitStructure.DMA_BufferSize = 11; 
//DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; 
//DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  
//DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
//DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;

//DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
//DMA_InitStructure.DMA_Priority = DMA_Priority_High;
//DMA_InitStructure.DMA_M2M = DMA_M2M_Disable; 
//DMA_Init(DMA1_Channel1, &DMA_InitStructure); 
//DMA_Cmd(DMA1_Channel1, ENABLE); 
//}
void inputs_init(void) 
{
	#ifndef T3PT12
	inputs_io_init();
	inputs_adc_init();
	//dma_adc_init();
	#endif
}


#ifdef T38AI8AO6DO
void inputs_scan(void)
{
	static u16 new_ad[22] ;
	static u16 old_ad[22]= {4095,4095,4095,4095,4095,4095,4095,4095,4095,4095,4095};
	static u8 channel_count =0 ;
	uint16_t ad_buf = 0 ;
	AD_Value[channel_count]=ADC_getChannal(ADC1,ADC_Channel_14);
	if(modbus.range[channel_count] == 6)
	{
		new_ad[channel_count] = AD_Value[channel_count] ;
		if((old_ad[channel_count]> new_ad[channel_count])&&((old_ad[channel_count] - new_ad[channel_count])>2000))
		{
			modbus.pulse[channel_count].word++ ;
			data_change[channel_count] = 1 ;
		}
		old_ad[channel_count] = new_ad[channel_count];
	}
	else
	{
	ad_buf = filter_inputs(channel_count, AD_Value[channel_count]);
	modbus.input[channel_count] = RangeConverter(modbus.range[channel_count], (ad_buf>>2),500,channel_count);
	}
	channel_count++;
	channel_count %= MAX_AI_CHANNEL;
	GPIO_ResetBits(GPIOC, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 );
	GPIO_SetBits(GPIOC, channel_count);
	CHA_SEL4 = 1  ;
	range_set_func(modbus.range[channel_count]);

//	switch(switch_temp)
//		{
//			case 0: 				
//				CHA_SEL0 = 1 ;
//				CHA_SEL1 = 0 ;
//				CHA_SEL2 = 0 ;
//			break;
//			case 1:			
//				CHA_SEL0 = 0 ;
//				CHA_SEL1 = 1 ;
//				CHA_SEL2 = 0 ;
//			break;
//			case 2:			
//				CHA_SEL0 = 1 ;
//				CHA_SEL1 = 1 ;
//				CHA_SEL2 = 0 ;
//			break;
//			case 3: 
//				CHA_SEL0 = 0 ;
//				CHA_SEL1 = 0 ;
//				CHA_SEL2 = 1 ;
//			break;
//			case 4: 
//				CHA_SEL0 = 1 ;
//				CHA_SEL1 = 0 ;
//				CHA_SEL2 = 1 ;
//			break;
//			case 5: 
//				CHA_SEL0 = 0 ;
//				CHA_SEL1 = 1 ;
//				CHA_SEL2 = 1 ;
//			break;
//			case 6: 
//				CHA_SEL0 = 1 ;
//				CHA_SEL1 = 1 ;
//				CHA_SEL2 = 1 ;
//			break;
//			case 7: 
//				CHA_SEL0 = 0 ;
//				CHA_SEL1 = 0 ;
//				CHA_SEL2 = 0 ;
//			break;																																				
//		}
}
#endif
#ifdef T322AI
void inputs_scan(void)
{
	//u8   switch_temp = 0 ;
	static u16 new_ad[22] ;
	static u16 old_ad[22]= {4095,4095,4095,4095,4095,4095,4095,4095,4095,4095,4095,4095,4095,4095,4095,4095,4095,4095,4095,4095,4095,4095};
	static u8 channel_count =0 ;
	u16 port_temp ; 
	AD_Value[channel_count]=ADC_getChannal(ADC1,ADC_Channel_14);
	if(modbus.range[channel_count] == 6)
	{
		new_ad[channel_count] = AD_Value[channel_count] ;
		if(channel_count>=11)
		{
			if((old_ad[channel_count]> new_ad[channel_count])&&((old_ad[channel_count] - new_ad[channel_count])>2000))
			{
				modbus.pulse[channel_count].word++ ;
				data_change[channel_count] = 1 ;
			}
			old_ad[channel_count] = new_ad[channel_count];
		}
	}
	else
	{
		modbus.raw_data[channel_count] = filter_inputs(channel_count, AD_Value[channel_count])/4;
		modbus.input[channel_count] = RangeConverter(modbus.range[channel_count], modbus.raw_data[channel_count],500,channel_count);	
	}
	channel_count++;
//	channel_count %= 16;
	 channel_count %= MAX_AI_CHANNEL;
	   
   if(channel_count % 8 == 0)
   {
      range_set_func(I020ma);
      delay_ms(5);
   }
  
	while(1)
	{
		if(channel_count >= 11 )  break ;
		if(modbus.range[channel_count] == PULSE)
		{
			channel_count++;
		}
		else
		{
			break; 
		}
		
	}
	
//	if(!((modbus.range[channel_count] == PULSE)&&(channel_count<11)))
	{
	
//		GPIO_ResetBits(GPIOA, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4);
//		GPIO_SetBits(GPIOA, channel_count);
		port_temp = GPIO_ReadOutputData(GPIOA);
		port_temp = 0xffe0&port_temp ;

//		port_temp = port_temp|0x10 ;		// modset = 1 diasble the first mode set
//		GPIO_Write(GPIOA, port_temp|channel_count);
//	    GPIO_Write(GPIOA, (port_temp|channel_count)&0xffef);//enasble the first mode set
		
//		GPIO_Write(GPIOA, port_temp|0x0010);
		
//		if(((modbus.range[channel_count] == PULSE)&&(channel_count<11)))
//		GPIO_Write(GPIOA, port_temp|channel_count|0x0010);
//		else
//		GPIO_Write(GPIOA, (port_temp|channel_count)&0xffef);
		GPIO_Write(GPIOA, (port_temp|channel_count));
		
		
		
//		GPIO_Write(GPIOA, port_temp|channel_count|0x0008);
//		GPIO_Write(GPIOA, port_temp|channel_count);
		
	}
	
	range_set_func(modbus.range[channel_count]);

}
#endif
u16 ADC_getChannal(ADC_TypeDef* ADCx, u8 channal)
{
         uint16_t tem = 0;
		 ADC_ClearFlag(ADCx, ADC_FLAG_EOC);
         ADC_RegularChannelConfig(ADCx, channal, 1, ADC_SampleTime_55Cycles5);
         ADC_SoftwareStartConvCmd(ADCx, ENABLE);         
         while(ADC_GetFlagStatus(ADCx, ADC_FLAG_EOC) == RESET);
         tem = ADC_GetConversionValue(ADCx);
         return tem;        
 }

 
 void range_set_func(u8 range)
 {
	 #ifndef T3PT12
	 if((range == V05)||(range == PULSE))
	 {
				RANGE_SET0 = 1 ;
				RANGE_SET1 = 0 ;
	 }
	 else if (range == V010)
	 {
				RANGE_SET0 = 0 ;
				RANGE_SET1 = 1 ;
	 }
	else if (range == I020ma)
	 {
				RANGE_SET0 = 0 ;
				RANGE_SET1 = 0 ;
	 }
	 else
	 {
				RANGE_SET0 = 1 ;
				RANGE_SET1 = 1 ;
	 }
	 #endif
 }
 

 
 #ifdef T322AI
  const struct
 {
	GPIO_TypeDef* GPIOx;
	uint16_t GPIO_Pin;
	uint8_t PortSource;
	uint8_t PinSource;
	uint8_t NVIC_IRQChannel;
	uint32_t Exit_line ; 
	 
 } _STR_PLUSE_SETTING_[11] = 
 {
	{GPIOA, GPIO_Pin_7, GPIO_PortSourceGPIOA, GPIO_PinSource7, EXTI9_5_IRQn, EXTI_Line7},
	{GPIOA, GPIO_Pin_6, GPIO_PortSourceGPIOA, GPIO_PinSource6, EXTI9_5_IRQn, EXTI_Line6},
	{GPIOA, GPIO_Pin_5, GPIO_PortSourceGPIOA, GPIO_PinSource5, EXTI9_5_IRQn, EXTI_Line5},
	{GPIOB, GPIO_Pin_11, GPIO_PortSourceGPIOB, GPIO_PinSource11, EXTI15_10_IRQn, EXTI_Line11},
	{GPIOB, GPIO_Pin_10, GPIO_PortSourceGPIOB, GPIO_PinSource10, EXTI15_10_IRQn, EXTI_Line10},
	{GPIOC, GPIO_Pin_3, GPIO_PortSourceGPIOC, GPIO_PinSource3, EXTI3_IRQn, EXTI_Line3},
	{GPIOC, GPIO_Pin_1, GPIO_PortSourceGPIOC, GPIO_PinSource1, EXTI1_IRQn, EXTI_Line1},
	{GPIOC, GPIO_Pin_2, GPIO_PortSourceGPIOC, GPIO_PinSource2, EXTI2_IRQn, EXTI_Line2},
	{GPIOD, GPIO_Pin_12, GPIO_PortSourceGPIOD, GPIO_PinSource12, EXTI15_10_IRQn, EXTI_Line12},
	{GPIOC, GPIO_Pin_0, GPIO_PortSourceGPIOC, GPIO_PinSource0, EXTI0_IRQn, EXTI_Line0},
	{GPIOD, GPIO_Pin_14, GPIO_PortSourceGPIOD, GPIO_PinSource14, EXTI15_10_IRQn, EXTI_Line14},
 };
  void pulse_set(uint8_t channel)
 {
	//u8 port_source ;
	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	GPIO_InitStructure.GPIO_Pin = _STR_PLUSE_SETTING_[channel].GPIO_Pin ;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(_STR_PLUSE_SETTING_[channel].GPIOx, &GPIO_InitStructure);
	GPIO_SetBits(_STR_PLUSE_SETTING_[channel].GPIOx, _STR_PLUSE_SETTING_[channel].GPIO_Pin );
	
	 
	GPIO_EXTILineConfig(_STR_PLUSE_SETTING_[channel].PortSource, _STR_PLUSE_SETTING_[channel].PinSource ); 
	//EXTI_InitStructure.EXTI_Line = (uint16_t)1<<GPIO_Pin ;
	EXTI_InitStructure.EXTI_Line  = _STR_PLUSE_SETTING_[channel].Exit_line ; 
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);	
	
	NVIC_InitStructure.NVIC_IRQChannel = _STR_PLUSE_SETTING_[channel].NVIC_IRQChannel ;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure); 
 
 }
// void pulse_set(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
// {
//	u8 port_source ;
//	GPIO_InitTypeDef GPIO_InitStructure;
//	EXTI_InitTypeDef EXTI_InitStructure;
//	NVIC_InitTypeDef NVIC_InitStructure;
//	 
//	GPIO_InitStructure.GPIO_Pin = (uint16_t)1<<GPIO_Pin ;	
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_Init(GPIOx, &GPIO_InitStructure);
//	GPIO_SetBits(GPIOx, GPIO_Pin );
//   
//	port_source = (GPIOx - GPIOA)/0x400 ; 
//	 if(GPIOx == GPIOA)
//	 {
//				port_source
//	 }

//	GPIO_EXTILineConfig(port_source, GPIO_Pin ); 
//	EXTI_InitStructure.EXTI_Line = (uint16_t)1<<GPIO_Pin ;
//	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
//	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
//	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
//	EXTI_Init(&EXTI_InitStructure);	
//	

//	switch(GPIO_Pin)
//	{
//		case 0:
//			NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
//		break ;
//		case 1:
//			NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;
//		break ;
//		case 2:
//			NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;
//		break ;
//		case 3:
//			NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;
//		break ;
//		case 4:
//			NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;
//		break ;
//		case 5:
//		case 6:
//		case 7:
//		case 8:
//		case 9:
//				NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
//		break ;
//		case 10:
//		case 11:
//		case 12:
//		case 13:
//		case 14:
//		case 15:
//				NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
//		break ;
//	}
//	
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_Init(&NVIC_InitStructure); 
// 
// }
/*
 LINE0------------>	INPUT9
 LINE1------------>	INPUT6
 LINE2------------>	INPUT7
 LINE3------------>	INPUT5
 LINE5------------>	INPUT2
 LINE6------------>	INPUT1
 LINE7------------>	INPUT0
 LINE10------------>INPUT4
 LINE11------------>INPUT3
 LINE12------------>INPUT8
 LINE14------------>INPUT10
*/
void EXTI0_IRQHandler(void)				//input10
{
	if(EXTI->PR & (1 << 0))	//��0�ߵ��ж�
	{    
		EXTI->PR = (1 << 0);	//���LINE0�ϵ��жϱ�־λ
	}
	modbus.pulse[9].word++ ;

}
void EXTI1_IRQHandler(void)		//input6
{
	if(EXTI->PR & (1 << 1))	//��3�ߵ��ж�
	{    
		EXTI->PR = (1 << 1);	//���LINE3�ϵ��жϱ�־λ
	}
	modbus.pulse[6].word++ ;
}
void EXTI2_IRQHandler(void)
{
	if(EXTI->PR & (1 << 2))	//��3�ߵ��ж�						//input7
	{    
		EXTI->PR = (1 << 2);	//���LINE3�ϵ��жϱ�־λ
	}
	modbus.pulse[7].word++ ;
}
void EXTI3_IRQHandler(void)											//input5
{
	if(EXTI->PR & (1 << 3))	//��3�ߵ��ж�
	{    
		EXTI->PR = (1 << 3);	//���LINE3�ϵ��жϱ�־λ
	}
	modbus.pulse[5].word++ ;

}
//void EXTI4_IRQHandler(void)										//
//{
//	if(EXTI->PR & (1 << 4))	//��3�ߵ��ж�
//	{    
//		EXTI->PR = (1 << 4);	//���LINE3�ϵ��жϱ�־λ
//	}
//}


void EXTI9_5_IRQHandler(void)
{

	if(EXTI->PR & (1 << 7))	//��7�ߵ��ж�
	{      
		EXTI->PR  = (1 << 7);	//���LINE7�ϵ��жϱ�־λ
		modbus.pulse[0].word++ ;

	}
	if(EXTI->PR & (1 << 6))	//��6�ߵ��ж�
	{      
		EXTI->PR  = (1 << 6);	//���LINE6�ϵ��жϱ�־λ
		modbus.pulse[1].word++ ;

	}
	if(EXTI->PR & (1 << 5))	//��5�ߵ��ж�
	{      
		EXTI->PR  = (1 << 5);	//���LINE5�ϵ��жϱ�־λ
		modbus.pulse[2].word++ ;

	}
} 

void EXTI15_10_IRQHandler(void)
{
	if(EXTI->PR & (1 << 11))	//��11�ߵ��ж�
	{      
		EXTI->PR  = (1 << 11);	//���LINE11�ϵ��жϱ�־λ
		modbus.pulse[3].word++ ;

	}
	if(EXTI->PR & (1 << 12))	//��12�ߵ��ж�
	{      
		EXTI->PR  = (1 << 12);	//���LINE12�ϵ��жϱ�־λ
		modbus.pulse[8].word++ ;
	}
	if(EXTI->PR & (1 << 10))	//��13�ߵ��ж�
	{      
		EXTI->PR  = (1 << 10);	//���LINE13�ϵ��жϱ�־λ
		modbus.pulse[4].word++ ;
	}
	if(EXTI->PR & (1 << 14))	//��14�ߵ��ж�
	{      
		EXTI->PR  = (1 << 14);	//���LINE14�ϵ��жϱ�־λ
		modbus.pulse[10].word++ ;
	}
}
#endif





//#ifdef T3PT12
//void inputs_scan(void)
//{
//	

//}
//#endif

 
 