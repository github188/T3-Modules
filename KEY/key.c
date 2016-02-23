#include "key.h"
#include "delay.h"
#include "stdio.h"

#include "modbus.h"


 void beeper_gpio_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
//	GPIO_SetBits(GPIOB, GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_9 | GPIO_Pin_7 | GPIO_Pin_8);
}

void beeper_on(void)
{
	GPIO_SetBits(GPIOB, GPIO_Pin_9);	
}

void beeper_off(void)
{
	GPIO_ResetBits(GPIOB, GPIO_Pin_9);	
}




#ifdef T38AI8AO6DO

u16 hand_status = 0 ;
u16 auto_status = 0 ;
u8  hand_key13_status = 0 ;
u8  auto_key13_status = 0 ;
u8  hand_key14_status = 0 ;
u8  auto_key14_status = 0 ;

u8 switch_state_buf[SWITCH_NUM] ;


static void key_analyse(u16 hand_status, u16 auto_status, u8 key_num, u8* switch_buf) ;
//������ʼ������
void KEY_IO_Init(void)
{
	
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);//ʹ��GPIODʱ��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6
	| GPIO_Pin_7| GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10| GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);//ʹ��GPIOCʱ�� 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //SET PC10 AS INPUT PULL UP
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);//ʹ��GPIOAʱ�� 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //SET PA11 AS INPUT PULL UP
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11|GPIO_Pin_12 ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //SET PC11 AND PC12 AS OUTPUT 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_SetBits(GPIOC, GPIO_Pin_11 | GPIO_Pin_12 );	
	
} 

void KEY_Status_Scan(void)
{	 
	u8 i ;
	GPIO_SetBits(GPIOC, GPIO_Pin_11);
	GPIO_ResetBits(GPIOC, GPIO_Pin_12 );
	auto_status = GPIO_ReadInputData(GPIOD);
	auto_key13_status =GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_10)  ;
	auto_key14_status =GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_11)  ;
	
	GPIO_SetBits(GPIOC, GPIO_Pin_12);
	GPIO_ResetBits(GPIOC, GPIO_Pin_11);
	hand_status = GPIO_ReadInputData(GPIOD);
	hand_key13_status =GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_10)  ;
	hand_key14_status =GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_11)  ;
//	hand_status = (hand_status<<2)| hand_key13_status ;
//	auto_status = (auto_status<<2)| auto_key13_status ;
	
	key_analyse(hand_status, auto_status, (SWITCH_NUM-1),  (u8*)switch_state_buf);
	key_analyse(hand_key13_status, auto_key13_status, 1,  (u8*)switch_state_buf+12);
	key_analyse(hand_key14_status, auto_key14_status, 1,  (u8*)switch_state_buf+13);
	
	modbus.switch_gourp[0] = 0 ;
	modbus.switch_gourp[1] = 0 ;
	
//	
//	modbus.switch_gourp[0]
	for(i=0; i<5; i++)
	{
		modbus.switch_gourp[0] = modbus.switch_gourp[0]<<2 ;
		modbus.switch_gourp[0] |= switch_state_buf[i] ;
	}
	
	
	modbus.switch_gourp[0] =  switch_state_buf[0] + (switch_state_buf[1]<<2)+(switch_state_buf[2]<<4)+(switch_state_buf[13]<<6)+(switch_state_buf[12]<<8)+(switch_state_buf[11]<<10);
	
	modbus.switch_gourp[1] =  switch_state_buf[10] + (switch_state_buf[9]<<2)+(switch_state_buf[8]<<4)+(switch_state_buf[7]<<6)+(switch_state_buf[6]<<8)+(switch_state_buf[5]<<10)+(switch_state_buf[4]<<12)
	+(switch_state_buf[3]<<14);
	
	//	for(i=8; i<14; i++)
//	{
//		modbus.switch_gourp[1] = modbus.switch_gourp[1]<<2 ;
//		modbus.switch_gourp[1] |= switch_state_buf[i-8] ;
//	}
}

void key_analyse(u16 hand_status, u16 auto_status, u8 key_num, u8* switch_buf)
{
	u16 result1 , result2;
	u8 i ;
	for(i=0; i<key_num; i++)
	{
		result1 = auto_status&(1<<i) ;
		result2 = hand_status&(1<<i) ;
	
		if((result1)&&(result2))
		{
			switch_buf[i] = SW_OFF;		
		}
		else if(result1)
		{
			switch_buf[i] = SW_AUTO;	
		}
		else if(result2)
		{
			switch_buf[i] = SW_HAND;	
		}
//		switch_test[i] = switch_buf[i] ;
	}
}
#endif
