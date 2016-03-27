#include <string.h>
#include "stm32f10x.h"
#include "usart.h"
#include "delay.h"
#include "led.h"
//#include "key.h"
#include "24cxx.h"
#include "spi.h"
#include "lcd.h"
#include "touch.h"
#include "flash.h"
#include "stmflash.h"
//#include "sdcard.h"
#include "mmc_sd.h"
#include "dma.h"
#include "vmalloc.h"
#include "ff.h"
#include "fattester.h"
#include "exfuns.h"
#include "enc28j60.h"
#include "timerx.h"
#include "uip.h"
#include "uip_arp.h"
#include "tapdev.h"
#include "usb_app.h"
//#include "ai.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "modbus.h"
#include "define.h"
#include "inputs.h"
#include "../output/output.h"
#include "dlmstp.h"
#include "rs485.h"
#include "datalink.h"
#include "config.h"
#include "handlers.h"
#include "device.h"	
#include "registerlist.h"
#include "../filter/filter.h"
#include "../KEY/key.h"

#include "t3-pt12.h" 
#include "read_pt.h"
#include "pt12_i2c.h"
#include <udp_scan.h>
#if (defined T38AI8AO6DO)||(defined T322AI)
void vSTORE_EEPTask(void *pvParameters ) ;
static void vINPUTSTask( void *pvParameters );
#endif
static void vLED0Task( void *pvParameters );
static void vCOMMTask( void *pvParameters );

//static void vUSBTask( void *pvParameters );

static void vNETTask( void *pvParameters );
#ifdef T38AI8AO6DO
void vKEYTask( void *pvParameters );
void vOUTPUTSTask( void *pvParameters );
#endif
#ifdef T3PT12
void vI2C_READ(void *pvParameters) ;
#endif
static void vMSTP_TASK(void *pvParameters ) ;
void uip_polling(void);

#define	BUF	((struct uip_eth_hdr *)&uip_buf[0])	
	

u8 ram_test1  ;
u8 ram_test2  ;
u8 read_cal = 0 ;

uint8_t  PDUBuffer[MAX_APDU];

u8 global_key = KEY_NON;

static void debug_config(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOA, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
}

int main(void)
{
	NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x8008000);
	debug_config();
	//ram_test = 0 ;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD , ENABLE);
 	delay_init(72);	
//	uart1_init(38400);
//	modbus.baudrate = 38400 ;
	//KEY_Init();
	EEP_Dat_Init();
	beeper_gpio_init();
	beeper_on();
	delay_ms(1000);
	beeper_off();
	//Lcd_Initial();
	SPI1_Init();
	SPI2_Init();
	mem_init(SRAMIN);
//	TIM3_Int_Init(5000, 7199);
//	TIM6_Int_Init(100, 7199);
//	
printf("T3_series\n\r");
	#if (defined T38AI8AO6DO)||(defined T322AI)
		xTaskCreate( vINPUTSTask, ( signed portCHAR * ) "INPUTS", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 3, NULL );
		xTaskCreate( vSTORE_EEPTask, ( signed portCHAR * ) "STOREEEP", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 2, NULL );
	#endif
	xTaskCreate( vLED0Task, ( signed portCHAR * ) "LED0", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 2, NULL );
 	xTaskCreate( vCOMMTask, ( signed portCHAR * ) "COMM", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 3, NULL );
	xTaskCreate( vNETTask, ( signed portCHAR * ) "NET",  configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL );
//	xTaskCreate( vUSBTask, ( signed portCHAR * ) "USB", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 2, NULL );
	//xTaskCreate( vUSBTask, ( signed portCHAR * ) "USB", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 2, NULL );
	/* Start the scheduler. */
 	#ifdef T38AI8AO6DO
	xTaskCreate( vOUTPUTSTask, ( signed portCHAR * ) "OUTPUTS", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 2, NULL );
	xTaskCreate( vKEYTask, ( signed portCHAR * ) "KEY", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 2, NULL );
	#endif
	#ifdef T3PT12
	xTaskCreate( vI2C_READ, ( signed portCHAR * ) "READ_I2C", configMINIMAL_STACK_SIZE+128, NULL, tskIDLE_PRIORITY + 2, NULL );
	#endif
	xTaskCreate( vMSTP_TASK, ( signed portCHAR * ) "MSTP", configMINIMAL_STACK_SIZE + 1024  , NULL, tskIDLE_PRIORITY + 4, NULL );
	vTaskStartScheduler();
}

//u8 dim_timer[14];

void vLED0Task( void *pvParameters )
{
	static u8 table_led_count =0 ;
	
	LED_Init();
	for( ;; )
	{
		table_led_count++ ;
		if(table_led_count>= 3)
		{
			table_led_count= 0 ;
			tabulate_LED_STATE();
		}
		refresh_led();
		delay_ms(10);
	}
	
//	for(;;)
//	{
//		
//		table_led_count++ ;
//		if(table_led_count>= 10)
//		{
//			table_led_count= 0 ;
//			tabulate_LED_STATE();
//		}	
//		if(count == 0)
//		{
//			//bank2 disable
//			GPIO_SetBits(GPIOE, GPIO_Pin_14);
//			for(i = 0; i < 14; i++)
//			{
//				dim_timer[i] = dim_timer_setting[i];
//			}
//		}
//		else if(count == 8)
//		{
//			//bank1 disable
//			GPIO_SetBits(GPIOE, GPIO_Pin_15);
//			for(i = 0; i < 14; i++)
//			{
//				dim_timer[i] = dim_timer_setting[i+14];
//			}
//		}
//		
//		for(i = 0; i < 14; i++)
//		{
//			if(dim_timer[i])
//			{
//				dim_timer[i]--;
//				GPIO_ResetBits(GPIOE, (GPIO_Pin_0<<i));
//			}
//			else
//			{
//				GPIO_SetBits(GPIOE, (GPIO_Pin_0<<i));
//			}
//		}			
//		// wirte output to GPIO
//		if(count == 0)
//		{
//			//bank2 enable
//			GPIO_ResetBits(GPIOE, GPIO_Pin_15);
//		}
//		else if(count == 8)
//		{
//			//bank1 enable
//			GPIO_ResetBits(GPIOE, GPIO_Pin_14);
//		}
//		
//		count++;
//		count %= 16;		
//		delay_us(1500);
//	}
	
}
void vCOMMTask(void *pvParameters )
{
	modbus_init();
	for( ;; )
	{
		if (dealwithTag)
		{  
		 dealwithTag--;
		  if(dealwithTag == 1)//&& !Serial_Master )	
			dealwithData();
		}
		if(serial_receive_timeout_count>0)  
		{
				serial_receive_timeout_count -- ; 
				if(serial_receive_timeout_count == 0)
				{
					serial_restart();
				}
		}
		delay_ms(5) ;
	}
	
}

#ifdef T3PT12
void vI2C_READ(void *pvParameters)
{
	uint8_t  loop ;
	static u8 wait_ad_stable = 0 ;
	float Rc[RPS] = {1118284 ,966354,895032,676148};
	PT12_IO_Init();
	 
	
	for( ;; )
	{
		if(read_cal == 0)
		{
			if(init_celpoint()) 
			{
				read_cal = 1; 
				printf("rs_data= %u,%u,%u,%u\n\r",rs_data[0],rs_data[1],rs_data[2],rs_data[3] );
				Rc[0] = (float)rs_data[0] ;
				Rc[1] =  (float)rs_data[1] ;
				Rc[2] =  (float)rs_data[2] ;
				Rc[3] =  (float)rs_data[3] ;                                                                                                                                           rs_data[3] ;
				min2method(&linear_K.temp_C, &linear_B.temp_C, RPS, Rc, Rs);
				//printf("%f, %f\n\r", linear_K.temp_C, linear_B.temp_C);
			}
		
		}
		else
		{
			read_rtd_data();
			update_temperature();
		}
		delay_ms(3000) ;				//if pulse changes ,store the data
	}	

}

#endif



 #if (defined T322AI)||(defined T38AI8AO6DO)
void vSTORE_EEPTask(void *pvParameters )
{
	uint8_t  loop ;
	for( ;; )
	{
		for(loop=0; loop<MAX_AI_CHANNEL; loop++)
		{
			//if(data_change & (1<<loop))
			if(data_change[loop] == 1)
			{
				AT24CXX_WriteOneByte(EEP_PLUSE0_HI_HI+4*loop, modbus.pulse[loop].quarter[0]);
				AT24CXX_WriteOneByte(EEP_PLUSE0_HI_LO+4*loop, modbus.pulse[loop].quarter[1]);
				AT24CXX_WriteOneByte(EEP_PLUSE0_LO_HI+4*loop, modbus.pulse[loop].quarter[2]);
				AT24CXX_WriteOneByte(EEP_PLUSE0_LO_LO+4*loop, modbus.pulse[loop].quarter[3]);
				data_change[loop] = 0 ;
			}
		}
		delay_ms(3000) ;				//if pulse changes ,store the data
	}
	
}
#endif
#ifndef T3PT12
void vINPUTSTask( void *pvParameters )
{
	u8 i ;
	
	for(i=0; i<MAX_AI_CHANNEL; i++)
	{
		AD_Value[i] = 0 ;	
	}
	
	inputs_init();
	for( ;; )
	{
		
		inputs_scan();
		
		delay_ms(10);
	}	
}
#endif
#ifdef T38AI8AO6DO
//void TIM3_PWM_Init(u16 arr, u16 psc)
//{
//	GPIO_InitTypeDef GPIO_InitStructure;
//	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
//	TIM_OCInitTypeDef TIM_OCInitStructure;
//	
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, ENABLE);
//	GPIO_PinRemapConfig(GPIO_FullRemap_TIM3, ENABLE); //TIM3_CH1->PC6, TIM3_CH2->PC7, TIM3_CH3->PC8, TIM3_CH4->PC9
//	
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9;  //TIM3_CH1-4
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_Init(GPIOC, &GPIO_InitStructure);
//	
//	TIM_TimeBaseStructure.TIM_Period = arr;
//	TIM_TimeBaseStructure.TIM_Prescaler = psc;
//	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
//	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
//	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
//	
//	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
//	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
//	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
//	TIM_OC1Init(TIM3, &TIM_OCInitStructure);
//	TIM_OC2Init(TIM3, &TIM_OCInitStructure);
//	TIM_OC3Init(TIM3, &TIM_OCInitStructure);
//	TIM_OC4Init(TIM3, &TIM_OCInitStructure);
//	
//	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);
//	TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);
//	TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);
//	TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable);
//	
//	TIM_Cmd(TIM3, ENABLE);									  
//} 


void vOUTPUTSTask( void *pvParameters )
{

	output_init();
	for( ;; )
	{
		output_refresh();
		update_digit_output();
		delay_ms(100);
	}	
}

void vKEYTask( void *pvParameters )
{
	KEY_IO_Init();
	for( ;; )
	{
		KEY_Status_Scan();
		delay_ms(100);
	}	
}

#endif


void Inital_Bacnet_Server(void)
{
	u32 Instance = 0x0c;
	Device_Init();
	Device_Set_Object_Instance_Number(Instance);  


#if  READ_WRITE_PROPERTY   

//	memset(AV_Present_Value,0,MAX_AVS );
//	memset(AI_Present_Value,0,MAX_AIS );
//	memset(AO_Present_Value,0,MAX_AOS * BACNET_MAX_PRIORITY);
//	memset(BI_Present_Value,0,MAX_BIS);
//	memset(BO_Present_Value,0,MAX_BOS * BACNET_MAX_PRIORITY );
//	Binary_Output_Init();

//	Analog_Output_Init();


#endif	
}
void vMSTP_TASK(void *pvParameters )
{
	uint16_t pdu_len = 0; 
	BACNET_ADDRESS  src;
	Inital_Bacnet_Server();
	dlmstp_init(NULL);
		Recievebuf_Initialize(0);
	for (;;)
    {
			
		if(modbus.protocal == BAC_MSTP)
		{
					pdu_len = datalink_receive(&src, &PDUBuffer[0], sizeof(PDUBuffer), 0,	modbus.protocal);
					if(pdu_len) 
						{
				        npdu_handler(&src, &PDUBuffer[0], pdu_len);	
						} 
						
			}
			delay_ms(5);
	}
	
}

extern u32 count_out;
extern u8 buffer_out[64];
void vUSBTask( void *pvParameters )
{
//	u8 i;
	for( ;; )
	{
//		if((count_out != 0) && (bDeviceState == CONFIGURED))
//		{
////			USB_To_USART_Send_Data(&buffer_out[0], count_out);
//			for(i = 0; i < count_out; i++)
//			{

//			}
//			count_out = 0;
//		}
		delay_ms(200);

	}
}

void vNETTask( void *pvParameters )
{
	//uip_ipaddr_t ipaddr;
	
	//u8 tcp_server_tsta = 0XFF;
	//u8 tcp_client_tsta = 0XFF;
	//printf("Temco ARM Demo\r\n");
//	delay_ms(500);
	u8 count = 0 ;
	while(tapdev_init())	//��ʼ��ENC28J60����
	{								   
	//	printf("ENC28J60 Init Error!\r\n");
		delay_ms(50);
	};		
	//printf("ENC28J60 Init OK!\r\n");
//	uip_init();							//uIP��ʼ��
//	
//	uip_ipaddr(ipaddr, 192, 168, 0, 163);	//���ñ�������IP��ַ
//	uip_sethostaddr(ipaddr);					    
//	uip_ipaddr(ipaddr, 192, 168, 0, 4); 	//��������IP��ַ(��ʵ������·������IP��ַ)
//	uip_setdraddr(ipaddr);						 
//	uip_ipaddr(ipaddr, 255, 255, 255, 0);	//������������
//	uip_setnetmask(ipaddr);

//	printf("IP:192.168.0.163\r\n"); 
//	printf("MASK:255.255.255.0\r\n"); 
//	printf("GATEWAY:192.168.0.4\r\n"); 	
//	
//	uip_listen(HTONS(1200));			//����1200�˿�,����TCP Server
//	uip_listen(HTONS(80));				//����80�˿�,����Web Server
//	tcp_client_reconnect();	   		    //�������ӵ�TCP Server��,����TCP Client
	
    for( ;; )
	{
		uip_polling();	//����uip�¼���������뵽�û������ѭ������ 
		
		if(IP_Change == 1)
		{
			count++ ;
			if(count == 10)
			{
				count = 0 ;
				IP_Change = 0 ;	
//				//if(!tapdev_init()) printf("Init fail\n\r");				
//				while(tapdev_init())	//��ʼ��ENC28J60����
//				{								   
//				//	printf("ENC28J60 Init Error!\r\n");
//				delay_ms(50);
//				};	
				SoftReset();
			}
			
		}
		
//		if(tcp_server_tsta != tcp_server_sta)		//TCP Server״̬�ı�
//		{															 
//			if(tcp_server_sta & (1 << 7))
//				printf("TCP Server Connected   \r\n");
//			else
//				printf("TCP Server Disconnected\r\n"); 
//			
// 			if(tcp_server_sta & (1 << 6))			//�յ�������
//			{
//   			printf("TCP Server RX:%s\r\n", tcp_server_databuf);//��ӡ����
//				tcp_server_sta &= ~(1 << 6);		//��������Ѿ�������	
//			}
//			tcp_server_tsta = tcp_server_sta;
//		}
//		
//		if(global_key == KEY_1)						//TCP Server ����������
//		{
//			global_key = KEY_NON;
//			if(tcp_server_sta & (1 << 7))			//���ӻ�����
//			{
//				sprintf((char*)tcp_server_databuf, "TCP Server OK\r\n");	 
//				tcp_server_sta |= 1 << 5;			//�����������Ҫ����
//			}
//		}
//		
//		if(tcp_client_tsta != tcp_client_sta)		//TCP Client״̬�ı�
//		{
//			if(tcp_client_sta & (1 << 7))
//				printf("TCP Client Connected   \r\n");
//			else
//				printf("TCP Client Disconnected\r\n");
//			
// 			if(tcp_client_sta & (1 << 6))			//�յ�������
//			{
//    			printf("TCP Client RX:%s\r\n", tcp_client_databuf);//��ӡ����
//				tcp_client_sta &= ~(1 << 6);		//��������Ѿ�������	
//			}
//			tcp_client_tsta = tcp_client_sta;
//		}
//		
//		if(global_key == KEY_2)						//TCP Client ����������
//		{
//			global_key = KEY_NON;
//			if(tcp_client_sta & (1 << 7))			//���ӻ�����
//			{
//				sprintf((char*)tcp_client_databuf, "TCP Client OK\r\n");	 
//				tcp_client_sta |= 1 << 5;			//�����������Ҫ����
//			}
//		}
		
//		if(global_key == KEY_3)
//		{
//			global_key = KEY_NON;
//			printf("global_key == KEY_3\r\n");
//			mf_mount(0);
//			mf_scan_files("0:");
//			mf_showfree("0:");
//		}
		
//		delay_ms(5);
    }
}



//uip�¼�������
//���뽫�ú��������û���ѭ��,ѭ������.
void uip_polling(void)
{
	u8 i;
	static struct timer periodic_timer, arp_timer;
	static u8 timer_ok = 0;	 
	if(timer_ok == 0)		//����ʼ��һ��
	{
		timer_ok = 1;
		timer_set(&periodic_timer, CLOCK_SECOND / 2); 	//����1��0.5��Ķ�ʱ�� 
		timer_set(&arp_timer, CLOCK_SECOND * 10);	   	//����1��10��Ķ�ʱ�� 
	}
	
	uip_len = tapdev_read();							//�������豸��ȡһ��IP��,�õ����ݳ���.uip_len��uip.c�ж���
	if(uip_len > 0)							 			//������
	{   
		//����IP���ݰ�(ֻ��У��ͨ����IP���Żᱻ����) 
		if(BUF->type == htons(UIP_ETHTYPE_IP))			//�Ƿ���IP��? 
		{
			uip_arp_ipin();								//ȥ����̫��ͷ�ṹ������ARP��
			uip_input();   								//IP������			
			//������ĺ���ִ�к������Ҫ�������ݣ���ȫ�ֱ��� uip_len > 0
			//��Ҫ���͵�������uip_buf, ������uip_len  (����2��ȫ�ֱ���)		    
			if(uip_len > 0)								//��Ҫ��Ӧ����
			{
				uip_arp_out();							//����̫��ͷ�ṹ������������ʱ����Ҫ����ARP����
				tapdev_send();							//�������ݵ���̫��
			}
		}
		else if (BUF->type == htons(UIP_ETHTYPE_ARP))	//����arp����,�Ƿ���ARP�����?
		{
			uip_arp_arpin();
			
 			//������ĺ���ִ�к������Ҫ�������ݣ���ȫ�ֱ���uip_len>0
			//��Ҫ���͵�������uip_buf, ������uip_len(����2��ȫ�ֱ���)
 			if(uip_len > 0)
				tapdev_send();							//��Ҫ��������,��ͨ��tapdev_send����	 
		}
	}
	else if(timer_expired(&periodic_timer))				//0.5�붨ʱ����ʱ
	{
		timer_reset(&periodic_timer);					//��λ0.5�붨ʱ�� 
		
		//��������ÿ��TCP����, UIP_CONNSȱʡ��40��  
		for(i = 0; i < UIP_CONNS; i++)
		{
			//uip_periodic(i);							//����TCPͨ���¼�
			
	 		//������ĺ���ִ�к������Ҫ�������ݣ���ȫ�ֱ���uip_len>0
			//��Ҫ���͵�������uip_buf, ������uip_len (����2��ȫ�ֱ���)
	 		if(uip_len > 0)
			{
				uip_arp_out();							//����̫��ͷ�ṹ������������ʱ����Ҫ����ARP����
				tapdev_send();							//�������ݵ���̫��
			}
		}
		
#if UIP_UDP	//UIP_UDP 
		//��������ÿ��UDP����, UIP_UDP_CONNSȱʡ��10��
		for(i = 0; i < UIP_UDP_CONNS; i++)
		{
			uip_udp_periodic(i);						//����UDPͨ���¼�
			
	 		//������ĺ���ִ�к������Ҫ�������ݣ���ȫ�ֱ���uip_len>0
			//��Ҫ���͵�������uip_buf, ������uip_len (����2��ȫ�ֱ���)
			if(uip_len > 0)
			{
				uip_arp_out();							//����̫��ͷ�ṹ������������ʱ����Ҫ����ARP����
				tapdev_send();							//�������ݵ���̫��
			}
		}
#endif 
		//ÿ��10�����1��ARP��ʱ������ ���ڶ���ARP����,ARP��10�����һ�Σ��ɵ���Ŀ�ᱻ����
		if(timer_expired(&arp_timer))
		{
			timer_reset(&arp_timer);
			uip_arp_timer();
		}
	}
}


void EEP_Dat_Init(void)
{
		u8 loop ;
		u8 temp[6]; 
		AT24CXX_Init();
		modbus.serial_Num[0] = AT24CXX_ReadOneByte(EEP_SERIALNUMBER_LOWORD);
		modbus.serial_Num[1] = AT24CXX_ReadOneByte(EEP_SERIALNUMBER_LOWORD+1);
		modbus.serial_Num[2] = AT24CXX_ReadOneByte(EEP_SERIALNUMBER_HIWORD);
		modbus.serial_Num[3] = AT24CXX_ReadOneByte(EEP_SERIALNUMBER_HIWORD+1);
		if((modbus.serial_Num[0]==0xff)&&(modbus.serial_Num[1]== 0xff)&&(modbus.serial_Num[2] == 0xff)&&(modbus.serial_Num[3] == 0xff))
		{
					modbus.serial_Num[0] = 0 ;
					modbus.serial_Num[1] = 0 ;
					modbus.serial_Num[2] = 0 ;
					modbus.serial_Num[3] = 0 ;
					AT24CXX_WriteOneByte(EEP_SERIALNUMBER_LOWORD, modbus.serial_Num[0]);
					AT24CXX_WriteOneByte(EEP_SERIALNUMBER_LOWORD+1, modbus.serial_Num[1]);
					AT24CXX_WriteOneByte(EEP_SERIALNUMBER_LOWORD+2, modbus.serial_Num[2]);
					AT24CXX_WriteOneByte(EEP_SERIALNUMBER_LOWORD+3, modbus.serial_Num[3]);
		}
		AT24CXX_WriteOneByte(EEP_VERSION_NUMBER_LO, SOFTREV&0XFF);
		AT24CXX_WriteOneByte(EEP_VERSION_NUMBER_HI, (SOFTREV>>8)&0XFF);
		modbus.address = AT24CXX_ReadOneByte(EEP_ADDRESS);
		if((modbus.address == 255)||(modbus.address == 0))
		{
					modbus.address = 254 ;
					AT24CXX_WriteOneByte(EEP_ADDRESS, modbus.address);
		}
		modbus.product = AT24CXX_ReadOneByte(EEP_PRODUCT_MODEL);
		if((modbus.product == 255)||(modbus.product == 0))
		{
			modbus.product = PRODUCT_ID ;
			AT24CXX_WriteOneByte(EEP_PRODUCT_MODEL, modbus.product);
		}
		modbus.hardware_Rev = AT24CXX_ReadOneByte(EEP_HARDWARE_REV);
		if((modbus.hardware_Rev == 255)||(modbus.hardware_Rev == 0))
		{
					modbus.hardware_Rev = HW_VER ;
					AT24CXX_WriteOneByte(EEP_HARDWARE_REV, modbus.hardware_Rev);
		}
		modbus.update = AT24CXX_ReadOneByte(EEP_UPDATE_STATUS);
		modbus.SNWriteflag = AT24CXX_ReadOneByte(EEP_SERIALNUMBER_WRITE_FLAG);
		
		modbus.baud = AT24CXX_ReadOneByte(EEP_BAUDRATE);
		if(modbus.baud > 4) 
		{	
			modbus.baud = 1 ;
			AT24CXX_WriteOneByte(EEP_BAUDRATE, modbus.baud);
		}
		switch(modbus.baud)
				{
					case 0:
						modbus.baudrate = BAUDRATE_9600 ;
						uart1_init(BAUDRATE_9600);
				
						SERIAL_RECEIVE_TIMEOUT = 6;
					break ;
					case 1:
						modbus.baudrate = BAUDRATE_19200 ;
						uart1_init(BAUDRATE_19200);	
						SERIAL_RECEIVE_TIMEOUT = 3;
					break;
					case 2:
						modbus.baudrate = BAUDRATE_38400 ;
						uart1_init(BAUDRATE_38400);
						SERIAL_RECEIVE_TIMEOUT = 2;
					break;
					case 3:
						modbus.baudrate = BAUDRATE_57600 ;
						uart1_init(BAUDRATE_57600);	
						SERIAL_RECEIVE_TIMEOUT = 1;
					break;
					case 4:
						modbus.baudrate = BAUDRATE_115200 ;
						uart1_init(BAUDRATE_115200);	
						SERIAL_RECEIVE_TIMEOUT = 1;
					break;
					default:
					break ;				
				}
//				modbus.protocal= AT24CXX_ReadOneByte(EEP_MODBUS_COM_CONFIG);
//				if((modbus.protocal == 255)||(modbus.protocal!=MODBUS)||(modbus.protocal!=BAC_MSTP ))
//				{
//					modbus.protocal = MODBUS ;
//					AT24CXX_WriteOneByte(EEP_MODBUS_COM_CONFIG, modbus.protocal);
//				}
				modbus.protocal= AT24CXX_ReadOneByte(EEP_MODBUS_COM_CONFIG);
				if(modbus.protocal >2)
				{
					modbus.protocal = 0 ;
					AT24CXX_WriteOneByte(EEP_MODBUS_COM_CONFIG, modbus.protocal);
				}
				for(loop = 0 ; loop<6; loop++)
				{
					temp[loop] = AT24CXX_ReadOneByte(EEP_MAC_ADDRESS_1+loop); 
				}
				if((temp[0]== 0xff)&&(temp[1]== 0xff)&&(temp[2]== 0xff)&&(temp[3]== 0xff)&&(temp[4]== 0xff)&&(temp[5]== 0xff) )
				{
					temp[0] = 0x04 ;
					temp[1] = 0x02 ;
					temp[2] = 0x35 ;
					temp[3] = 0xaF ;
					temp[4] = 0x00 ;
					temp[5] = 0x01 ;
					AT24CXX_WriteOneByte(EEP_MAC_ADDRESS_1, temp[0]);
					AT24CXX_WriteOneByte(EEP_MAC_ADDRESS_2, temp[1]);
					AT24CXX_WriteOneByte(EEP_MAC_ADDRESS_3, temp[2]);
					AT24CXX_WriteOneByte(EEP_MAC_ADDRESS_4, temp[3]);
					AT24CXX_WriteOneByte(EEP_MAC_ADDRESS_5, temp[4]);
					AT24CXX_WriteOneByte(EEP_MAC_ADDRESS_6, temp[5]);		
				}
				for(loop =0; loop<6; loop++)
				{
					modbus.mac_addr[loop] =  temp[loop]	;
				}
				
				for(loop = 0 ; loop<4; loop++)
				{
					temp[loop] = AT24CXX_ReadOneByte(EEP_IP_ADDRESS_1+loop); 
				}
				if((temp[0]== 0xff)&&(temp[1]== 0xff)&&(temp[2]== 0xff)&&(temp[3]== 0xff) )
				{
					temp[0] = 192 ;
					temp[1] = 168 ;
					temp[2] = 0 ;
					temp[3] = 183 ;
					AT24CXX_WriteOneByte(EEP_IP_ADDRESS_1, temp[0]);
					AT24CXX_WriteOneByte(EEP_IP_ADDRESS_2, temp[1]);
					AT24CXX_WriteOneByte(EEP_IP_ADDRESS_3, temp[2]);
					AT24CXX_WriteOneByte(EEP_IP_ADDRESS_4, temp[3]);
				}
				for(loop = 0 ; loop<4; loop++)
				{
					modbus.ip_addr[loop] = 	temp[loop] ;
					modbus.ghost_ip_addr[loop] = modbus.ip_addr[loop] ;
				}
				
				temp[0] = AT24CXX_ReadOneByte(EEP_IP_MODE);
				if(temp[0] >1)
				{
					temp[0] = 0 ;
					AT24CXX_WriteOneByte(EEP_IP_MODE, temp[0]);	
				}
				modbus.ip_mode = temp[0] ;
				modbus.ghost_ip_mode = modbus.ip_mode ;
				
				
				for(loop = 0 ; loop<4; loop++)
				{
					temp[loop] = AT24CXX_ReadOneByte(EEP_SUB_MASK_ADDRESS_1+loop); 
				}
				if((temp[0]== 0xff)&&(temp[1]== 0xff)&&(temp[2]== 0xff)&&(temp[3]== 0xff) )
				{
					temp[0] = 0xff ;
					temp[1] = 0xff ;
					temp[2] = 0xff ;
					temp[3] = 0 ;
					AT24CXX_WriteOneByte(EEP_SUB_MASK_ADDRESS_1, temp[0]);
					AT24CXX_WriteOneByte(EEP_SUB_MASK_ADDRESS_2, temp[1]);
					AT24CXX_WriteOneByte(EEP_SUB_MASK_ADDRESS_3, temp[2]);
					AT24CXX_WriteOneByte(EEP_SUB_MASK_ADDRESS_4, temp[3]);
				
				}				
				for(loop = 0 ; loop<4; loop++)
				{
					modbus.mask_addr[loop] = 	temp[loop] ;
					modbus.ghost_mask_addr[loop] = modbus.mask_addr[loop] ;
				}
				
				for(loop = 0 ; loop<4; loop++)
				{
					temp[loop] = AT24CXX_ReadOneByte(EEP_GATEWAY_ADDRESS_1+loop); 
				}
				if((temp[0]== 0xff)&&(temp[1]== 0xff)&&(temp[2]== 0xff)&&(temp[3]== 0xff) )
				{
					temp[0] = 192 ;
					temp[1] = 168 ;
					temp[2] = 0 ;
					temp[3] = 4 ;
					AT24CXX_WriteOneByte(EEP_GATEWAY_ADDRESS_1, temp[0]);
					AT24CXX_WriteOneByte(EEP_GATEWAY_ADDRESS_2, temp[1]);
					AT24CXX_WriteOneByte(EEP_GATEWAY_ADDRESS_3, temp[2]);
					AT24CXX_WriteOneByte(EEP_GATEWAY_ADDRESS_4, temp[3]);
				
				}				
				for(loop = 0 ; loop<4; loop++)
				{
					modbus.gate_addr[loop] = 	temp[loop] ;
					modbus.ghost_gate_addr[loop] = modbus.gate_addr[loop] ;
				}
				
				temp[0] = AT24CXX_ReadOneByte(EEP_TCP_SERVER);
				if(temp[0] == 0xff)
				{
					temp[0] = 0 ;
					AT24CXX_WriteOneByte(EEP_TCP_SERVER, temp[0]);
				}
				modbus.tcp_server = temp[0];
				modbus.ghost_tcp_server = modbus.tcp_server  ;
				
				temp[0] =AT24CXX_ReadOneByte(EEP_LISTEN_PORT_HI);
				temp[1] =AT24CXX_ReadOneByte(EEP_LISTEN_PORT_LO);
				if(temp[0] == 0xff && temp[1] == 0xff )
				{
					modbus.listen_port = 502 ;
					temp[0] = (modbus.listen_port>>8)&0xff ;
					temp[1] = modbus.listen_port&0xff ;				
				}
				modbus.listen_port = (temp[0]<<8)|temp[1] ;
				modbus.ghost_listen_port = modbus.listen_port ;
				
				modbus.write_ghost_system = 0 ;
				modbus.reset = 0 ;
		#ifndef T3PT12
		for(loop = 0; loop < 11; loop++)
		{
			temp[0] =AT24CXX_ReadOneByte(EEP_CUSTOMER_TABLE1_VOL_HI_0+4*loop);
			temp[1] =AT24CXX_ReadOneByte(EEP_CUSTOMER_TABLE1_VOL_HI_0+1+4*loop);
			temp[2] =AT24CXX_ReadOneByte(EEP_CUSTOMER_TABLE1_VOL_HI_0+2+4*loop);
			temp[3] =AT24CXX_ReadOneByte(EEP_CUSTOMER_TABLE1_VOL_HI_0+3+4*loop);
			if((temp[0] == 0xff)&&(temp[1]==0xff)&&(temp[2]==0xff)&&(temp[3]==0xff))
			{
				temp[0] = 0 ;
				temp[1]= 0 ;
				temp[2] = 0 ;
				temp[3] = 0 ;
				AT24CXX_WriteOneByte(EEP_CUSTOMER_TABLE1_VOL_HI_0+4*loop, temp[0]);
				AT24CXX_WriteOneByte(EEP_CUSTOMER_TABLE1_VOL_HI_0+4*loop+1, temp[1]);
				AT24CXX_WriteOneByte(EEP_CUSTOMER_TABLE1_VOL_HI_0+4*loop+2, temp[2]);
				AT24CXX_WriteOneByte(EEP_CUSTOMER_TABLE1_VOL_HI_0+4*loop+3, temp[3]);
				modbus.customer_table1_val[loop] = 0 ;
				modbus.customer_table1_vol[loop] = 0 ;
			}
			else
			{
				modbus.customer_table1_vol[loop] = (temp[0]<<8)+temp[1] ;
				modbus.customer_table1_val[loop] = (temp[2]<<8)+temp[3] ;
			}
			
			temp[0] =AT24CXX_ReadOneByte(EEP_CUSTOMER_TABLE2_VOL_HI_0+4*loop);
			temp[1] =AT24CXX_ReadOneByte(EEP_CUSTOMER_TABLE2_VOL_HI_0+1+4*loop);
			temp[2] =AT24CXX_ReadOneByte(EEP_CUSTOMER_TABLE2_VOL_HI_0+2+4*loop);
			temp[3] =AT24CXX_ReadOneByte(EEP_CUSTOMER_TABLE2_VOL_HI_0+3+4*loop);
			if((temp[0] == 0xff)&&(temp[1]==0xff)&&(temp[2]==0xff)&&(temp[3]==0xff))
			{
				temp[0] = 0 ;
				temp[1]= 0 ;
				temp[2] = 0 ;
				temp[3] = 0 ;
				AT24CXX_WriteOneByte(EEP_CUSTOMER_TABLE2_VOL_HI_0+4*loop, temp[0]);
				AT24CXX_WriteOneByte(EEP_CUSTOMER_TABLE2_VOL_HI_0+4*loop+1, temp[1]);
				AT24CXX_WriteOneByte(EEP_CUSTOMER_TABLE2_VOL_HI_0+4*loop+2, temp[2]);
				AT24CXX_WriteOneByte(EEP_CUSTOMER_TABLE2_VOL_HI_0+4*loop+3, temp[3]);
				modbus.customer_table2_val[loop] = 0 ;
				modbus.customer_table2_vol[loop] = 0 ;
			}
			else
			{
				modbus.customer_table2_vol[loop] = (temp[0]<<8)+temp[1] ;
				modbus.customer_table2_val[loop] = (temp[2]<<8)+temp[3] ;
			}
			
			temp[0] =AT24CXX_ReadOneByte(EEP_CUSTOMER_TABLE3_VOL_HI_0+4*loop);
			temp[1] =AT24CXX_ReadOneByte(EEP_CUSTOMER_TABLE3_VOL_HI_0+1+4*loop);
			temp[2] =AT24CXX_ReadOneByte(EEP_CUSTOMER_TABLE3_VOL_HI_0+2+4*loop);
			temp[3] =AT24CXX_ReadOneByte(EEP_CUSTOMER_TABLE3_VOL_HI_0+3+4*loop);
			if((temp[0] == 0xff)&&(temp[1]==0xff)&&(temp[2]==0xff)&&(temp[3]==0xff))
			{
				temp[0] = 0 ;
				temp[1]= 0 ;
				temp[2] = 0 ;
				temp[3] = 0 ;
				AT24CXX_WriteOneByte(EEP_CUSTOMER_TABLE3_VOL_HI_0+4*loop, temp[0]);
				AT24CXX_WriteOneByte(EEP_CUSTOMER_TABLE3_VOL_HI_0+4*loop+1, temp[1]);
				AT24CXX_WriteOneByte(EEP_CUSTOMER_TABLE3_VOL_HI_0+4*loop+2, temp[2]);
				AT24CXX_WriteOneByte(EEP_CUSTOMER_TABLE3_VOL_HI_0+4*loop+3, temp[3]);
				modbus.customer_table3_val[loop] = 0 ;
				modbus.customer_table3_vol[loop] = 0 ;
			}
			else
			{
				modbus.customer_table3_vol[loop] = (temp[0]<<8)+temp[1] ;
				modbus.customer_table3_val[loop] = (temp[2]<<8)+temp[3] ;
			}
			
			temp[0] =AT24CXX_ReadOneByte(EEP_CUSTOMER_TABLE4_VOL_HI_0+4*loop);
			temp[1] =AT24CXX_ReadOneByte(EEP_CUSTOMER_TABLE4_VOL_HI_0+1+4*loop);
			temp[2] =AT24CXX_ReadOneByte(EEP_CUSTOMER_TABLE4_VOL_HI_0+2+4*loop);
			temp[3] =AT24CXX_ReadOneByte(EEP_CUSTOMER_TABLE4_VOL_HI_0+3+4*loop);
			if((temp[0] == 0xff)&&(temp[1]==0xff)&&(temp[2]==0xff)&&(temp[3]==0xff))
			{
				temp[0] = 0 ;
				temp[1]= 0 ;
				temp[2] = 0 ;
				temp[3] = 0 ;
				AT24CXX_WriteOneByte(EEP_CUSTOMER_TABLE4_VOL_HI_0+4*loop, temp[0]);
				AT24CXX_WriteOneByte(EEP_CUSTOMER_TABLE4_VOL_HI_0+4*loop+1, temp[1]);
				AT24CXX_WriteOneByte(EEP_CUSTOMER_TABLE4_VOL_HI_0+4*loop+2, temp[2]);
				AT24CXX_WriteOneByte(EEP_CUSTOMER_TABLE4_VOL_HI_0+4*loop+3, temp[3]);
				modbus.customer_table4_val[loop] = 0 ;
				modbus.customer_table4_vol[loop] = 0 ;
			}
			else
			{
				modbus.customer_table4_vol[loop] = (temp[0]<<8)+temp[1] ;
				modbus.customer_table4_val[loop] = (temp[2]<<8)+temp[3] ;
			}
			

		}
#endif		
		#ifdef T322AI
		for(loop = 0; loop < 22; loop++)
		{
				modbus.filter_value[loop] = AT24CXX_ReadOneByte(EEP_AI_FILTER0+loop) ;
				if((modbus.filter_value[loop] == 0xff)||(modbus.filter_value[loop] > 20))
				{
						modbus.filter_value[loop] = DEFAULT_FILTER;  //header at beginning of project, default filer setting
						AT24CXX_WriteOneByte(EEP_AI_FILTER0+loop, modbus.filter_value[loop]);
				}
				modbus.range[loop] = AT24CXX_ReadOneByte(EEP_AI_RANGE0+loop) ;
				if(modbus.range[loop]> 20)
				{
						modbus.range[loop] = 0 ;
						AT24CXX_WriteOneByte(EEP_AI_RANGE0+loop, modbus.range[loop]);
				}
				if(loop < 11)
				{
						 pulse_set(loop);
				}
				
				temp[0] = AT24CXX_ReadOneByte(EEP_AI_OFFSET0+2*loop) ;
				temp[1] = AT24CXX_ReadOneByte(EEP_AI_OFFSET0+2*loop +1) ;
				if((temp[0]== 0xff)&&(temp[1] == 0xff))
				{
						modbus.offset[loop] = 500 ;
						temp[0] = (modbus.offset[loop]>>8)&0xff ;
						temp[1] = (modbus.offset[loop])&0xff ;
						AT24CXX_WriteOneByte(EEP_AI_OFFSET0+2*loop, temp[0]);
						AT24CXX_WriteOneByte(EEP_AI_OFFSET0+2*loop+1, temp[1]);
				}
				else
				{
						modbus.offset[loop] = (temp[0]<<8)|temp[1];
				}
				
				modbus.pulse[loop].quarter[0] = AT24CXX_ReadOneByte(EEP_PLUSE0_HI_HI+4*loop) ;
				modbus.pulse[loop].quarter[1] = AT24CXX_ReadOneByte(EEP_PLUSE0_HI_LO+4*loop);
				modbus.pulse[loop].quarter[2] = AT24CXX_ReadOneByte(EEP_PLUSE0_LO_HI+4*loop) ;
				modbus.pulse[loop].quarter[3] = AT24CXX_ReadOneByte(EEP_PLUSE0_LO_LO+4*loop);
				if((modbus.pulse[loop].quarter[0] == 0xff)&&(modbus.pulse[loop].quarter[1]== 0xff)&&(modbus.pulse[loop].quarter[2]== 0xff)&& (modbus.pulse[loop].quarter[3] == 0xff))
				{
					modbus.pulse[loop].word = 0 ;
					AT24CXX_WriteOneByte(EEP_PLUSE0_HI_HI+4*loop, modbus.pulse[loop].quarter[0]);
					AT24CXX_WriteOneByte(EEP_PLUSE0_HI_LO+4*loop, modbus.pulse[loop].quarter[1]);
					AT24CXX_WriteOneByte(EEP_PLUSE0_LO_HI+4*loop, modbus.pulse[loop].quarter[2]);
					AT24CXX_WriteOneByte(EEP_PLUSE0_LO_LO+4*loop, modbus.pulse[loop].quarter[3]);
				}
				
//				temp[1] = AT24CXX_ReadOneByte(EEP_CUSTOMER_RANGE0_ENABLE+loop) ;
//				if(temp[1] == 0xff)
//				{
//					modbus.customer_enable[loop] = 0 ; 
//					AT24CXX_WriteOneByte(EEP_CUSTOMER_RANGE0_ENABLE+loop, modbus.customer_enable[loop]);
//				}
//				else
//				{
//					modbus.customer_enable[loop] = temp[1];
//				}
		
		}
		#endif
		#ifdef T38AI8AO6DO
		for(loop=0; loop<MAX_AO; loop++)
		{				
				temp[1]	= AT24CXX_ReadOneByte(EEP_AO_CHANNLE0+2*loop);
				temp[2]	= AT24CXX_ReadOneByte(EEP_AO_CHANNLE0+1+2*loop);
				if((temp[1]== 0xff)&&(temp[2] == 0xff))
				{
					modbus.analog_output[loop] = 0 ;
					AT24CXX_WriteOneByte(EEP_AO_CHANNLE0+2*loop+1, (modbus.analog_output[loop]>>8)&0xff);
					AT24CXX_WriteOneByte(EEP_AO_CHANNLE0+2*loop, modbus.analog_output[loop]);
				}
				else
				{
						modbus.analog_output[loop] = (temp[2]<<8)| temp[1] ;		
				}
		}
		for(loop=0; loop<MAX_DO; loop++)
		{
			modbus.digit_output[loop] = AT24CXX_ReadOneByte(EEP_DO_CHANNLE0+loop);
			if(modbus.digit_output[loop]> 1) 
			{
					modbus.digit_output[loop] = 1 ;
					AT24CXX_WriteOneByte(EEP_DO_CHANNLE0+loop, modbus.analog_output[loop]);
			}
		}
		for(loop=0; loop<MAX_AI_CHANNEL; loop++)
		{
				modbus.filter_value[loop] = AT24CXX_ReadOneByte(EEP_AI_FILTER0+loop);
				if(modbus.filter_value[loop] == 0xff)
				{
						modbus.filter_value[loop] = DEFAULT_FILTER ;
						AT24CXX_WriteOneByte(EEP_AI_FILTER0+loop, modbus.filter_value[loop]);
				}
				modbus.range[loop] = AT24CXX_ReadOneByte(EEP_AI_RANGE0+loop) ;
				if(modbus.range[loop] == 0xff)
				{
						modbus.range[loop] = 0 ;
						AT24CXX_WriteOneByte(EEP_AI_RANGE0+loop, modbus.range[loop]);
				}
				
				temp[1] = AT24CXX_ReadOneByte(EEP_AI_OFFSET0+2*loop) ;
				temp[2] = AT24CXX_ReadOneByte(EEP_AI_OFFSET0+2*loop +1) ;
				if((temp[1]== 0xff)&&(temp[2] == 0xff))
				{
						modbus.offset[loop] = 500 ;
						temp[1] = (modbus.offset[loop]>>8)&0xff ;
						temp[2] = (modbus.offset[loop])&0xff ;
						AT24CXX_WriteOneByte(EEP_AI_OFFSET0+2*loop, temp[1]);
						AT24CXX_WriteOneByte(EEP_AI_OFFSET0+2*loop+1, temp[2]);
				}
				else
				{
						modbus.offset[loop] = (temp[1]<<8)|temp[2];
				}
				modbus.pulse[loop].quarter[0] = AT24CXX_ReadOneByte(EEP_PLUSE0_HI_HI+4*loop) ;
				modbus.pulse[loop].quarter[1] = AT24CXX_ReadOneByte(EEP_PLUSE0_HI_LO+4*loop);
				modbus.pulse[loop].quarter[2] = AT24CXX_ReadOneByte(EEP_PLUSE0_LO_HI+4*loop) ;
				modbus.pulse[loop].quarter[3] = AT24CXX_ReadOneByte(EEP_PLUSE0_LO_LO+4*loop);
				if((modbus.pulse[loop].quarter[0] == 0xff)&&(modbus.pulse[loop].quarter[1]== 0xff)&&(modbus.pulse[loop].quarter[2]== 0xff)&& (modbus.pulse[loop].quarter[3] == 0xff))
				{
					modbus.pulse[loop].word = 0 ;
					AT24CXX_WriteOneByte(EEP_PLUSE0_HI_HI+4*loop, modbus.pulse[loop].quarter[0]);
					AT24CXX_WriteOneByte(EEP_PLUSE0_HI_LO+4*loop, modbus.pulse[loop].quarter[1]);
					AT24CXX_WriteOneByte(EEP_PLUSE0_LO_HI+4*loop, modbus.pulse[loop].quarter[2]);
					AT24CXX_WriteOneByte(EEP_PLUSE0_LO_LO+4*loop, modbus.pulse[loop].quarter[3]);
				}
				
				
				
				
//				modbus.pulse[loop].quarter[0] = AT24CXX_ReadOneByte(EEP_PLUSE0_HI_HI+4*loop) ;
//				modbus.pulse[loop].quarter[1] = AT24CXX_ReadOneByte(EEP_PLUSE0_HI_LO+4*loop);
//				modbus.pulse[loop].quarter[2] = AT24CXX_ReadOneByte(EEP_PLUSE0_LO_HI+4*loop) ;
//				modbus.pulse[loop].quarter[3] = AT24CXX_ReadOneByte(EEP_PLUSE0_LO_LO+4*loop);
//				if((modbus.pulse[loop].quarter[0] == 0xff)&&(modbus.pulse[loop].quarter[1]== 0xff)&&(modbus.pulse[loop].quarter[2]== 0xff)&& (modbus.pulse[loop].quarter[3] == 0xff))
//				{
//							modbus.pulse[loop].word = 0 ;
//						  AT24CXX_WriteOneByte(EEP_PLUSE0_HI_HI+4*loop, modbus.pulse[loop].quarter[0]);
//							AT24CXX_WriteOneByte(EEP_PLUSE0_HI_LO+4*loop, modbus.pulse[loop].quarter[1]);
//							AT24CXX_WriteOneByte(EEP_PLUSE0_LO_HI+4*loop, modbus.pulse[loop].quarter[2]);
//							AT24CXX_WriteOneByte(EEP_PLUSE0_LO_LO+4*loop, modbus.pulse[loop].quarter[3]);
//				}
				
		}
		#endif
		#ifdef T3PT12
		for(loop=0; loop<12; loop++)
		{
			temp[0] = AT24CXX_ReadOneByte(EEP_DEFAULT_TEMP_OFFSET0_HI+4*loop) ;
			temp[1] = AT24CXX_ReadOneByte(EEP_DEFAULT_TEMP_OFFSET0_HI+4*loop +1) ;
			temp[2] = AT24CXX_ReadOneByte(EEP_DEFAULT_TEMP_OFFSET0_HI+4*loop+2) ;
			temp[3] = AT24CXX_ReadOneByte(EEP_DEFAULT_TEMP_OFFSET0_HI+4*loop +3) ;
			
			if((temp[0] == 0xff)&&(temp[1] == 0xff)&&(temp[2] == 0xff)&&(temp[3] == 0xff))
			{
			 _offset_rc[loop].temp_C =100.00;
			temp[0] = _offset_rc[loop].qauter_temp_C[0];
			temp[1] = _offset_rc[loop].qauter_temp_C[1];
			temp[2] = _offset_rc[loop].qauter_temp_C[2];
			temp[3] = _offset_rc[loop].qauter_temp_C[3];
				
			AT24CXX_WriteOneByte(EEP_DEFAULT_TEMP_OFFSET0_HI+4*loop, _offset_rc[loop].qauter_temp_C[0]);
			AT24CXX_WriteOneByte(EEP_DEFAULT_TEMP_OFFSET0_HI+4*loop+1, _offset_rc[loop].qauter_temp_C[1]);
			AT24CXX_WriteOneByte(EEP_DEFAULT_TEMP_OFFSET0_LO+4*loop, _offset_rc[loop].qauter_temp_C[2]);
			AT24CXX_WriteOneByte(EEP_DEFAULT_TEMP_OFFSET0_LO+4*loop+1, _offset_rc[loop].qauter_temp_C[3]);	
			
			}
			else
			{
				_offset_rc[loop].qauter_temp_C[0] = temp[0] ;
				_offset_rc[loop].qauter_temp_C[1] = temp[1] ;
				_offset_rc[loop].qauter_temp_C[2] = temp[2] ;
				_offset_rc[loop].qauter_temp_C[3] = temp[3] ;				
			}
			
			temp[0] = AT24CXX_ReadOneByte(EEP_CHANNEL0_FLTER+loop) ;
			if((temp[0] == 0xff)||(temp[0]>20))
			{
				modbus.filter_value[loop] = 5 ;
				AT24CXX_WriteOneByte(EEP_CHANNEL0_FLTER+loop, modbus.filter_value[loop]);	
			}
			else
			{
				modbus.filter_value[loop] = temp[0] ;
			}
			
			temp[0] = AT24CXX_ReadOneByte(EEP_CHANNEL0_RANGE+loop) ;
			if((temp[0] == 0xff)||(temp[0]>2))
			{
				modbus.range[loop] = 0 ;
				AT24CXX_WriteOneByte(EEP_CHANNEL0_RANGE+loop, modbus.range[loop]);	
			}
			else
			{
				modbus.range[loop] = temp[0] ;
			}						
			
		}
		
		temp[0] = AT24CXX_ReadOneByte(EEP_RESOLE_BIT) ;
		if((temp[0] == 0xff)||(temp[0]>4))
		{
				modbus.resole_bit = 2;
				AT24CXX_WriteOneByte(EEP_RESOLE_BIT, modbus.resole_bit);
		}
		else
		modbus.resole_bit = temp[0] ;
		

		#endif
		
}
