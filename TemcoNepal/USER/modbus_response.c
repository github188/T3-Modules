#include <modbus.h>
#include <string.h>
#include <tcp_modbus.h>
#include <crc.h>
#include <registerlist.h>
#include <ud_str.h>

#include "stdlib.h"
#include "usart.h"
#include "24cxx.h" 
#include "delay.h"
#include "define.h"
#include "inputs.h"
#include "../filter/filter.h"
#include "led.h"
#include "fifo.h"
#include "bacnettimer.h"
#include "../KEY/key.h"
#include "read_pt.h"
#include "tapdev.h"
#include <udp_scan.h>

u8 temp1 = 0, temp2 = 0;
extern u8 i;
extern vu8 rece_size;
u16 address;
extern u16 address_temp ;
u16 div_temp ;
extern u16 address_buf ;	
u16 send_cout;




void response_write_variables(u8 HeadLen,u8 type,u8 *sendbuf,u8 *pData){
		send_cout = HeadLen ;

		if(type == 0)
		{
			for(i = 0; i < rece_size; i++)
			{
				sendbuf[send_cout++] = pData[i] ;
				

			}
			memcpy(uart_send, sendbuf, send_cout);
			USART_SendDataString(send_cout);		
		}
		else // TCP   dont have CRC 
		{
		//	SetTransactionId(6 + UIP_HEAD);
			sendbuf[0] = pData[0];//0;			//	TransID
			sendbuf[1] = pData[1];//TransID++;	
			sendbuf[2] = 0;			//	ProtoID
			sendbuf[3] = 0;
			sendbuf[4] = 0;	//	Len
			sendbuf[5] = 6 ;

			for (i = 0;i < 6;i++)
			{
				sendbuf[HeadLen + i] = pData[HeadLen + i];	
			}
			
			memcpy(tcp_server_sendbuf,sendbuf,6+ HeadLen);
			tcp_server_sendlen = 6 + HeadLen;
//			if(cSemaphoreTake( xSemaphore_tcp_send, ( portTickType ) 10 ) == pdTRUE)
//			{				
//				TCPIP_TcpSend(pHttpConn->TcpSocket, sendbuf, 6 + UIP_HEAD, TCPIP_SEND_NOT_FINAL); 
//				cSemaphoreGive( xSemaphore_tcp_send );
//			}
		}

		
	}

	void response_multiple_write(u8 HeadLen,u8 type,u8 *sendbuf,u8 *pData){
		
		if(type == 0)
		{		
			for(i = 0; i < 6; i++)
			{
			sendbuf[HeadLen+i] = pData[HeadLen+i] ;
			crc16_byte(sendbuf[HeadLen+i]);
			}
			sendbuf[HeadLen+i] = CRChi ;
			sendbuf[HeadLen+i+1] = CRClo ;
			memcpy(uart_send, sendbuf, 8);
			USART_SendDataString(8);
		}
		else
		{
				sendbuf[0] = pData[0] ;
				sendbuf[1] = pData[1] ;
				sendbuf[2] = 0 ;
				sendbuf[3] = 0 ;
				sendbuf[4] = 0; 
				sendbuf[5] =6;					
				for (i = 0;i < 6;i++)
				{
					sendbuf[HeadLen + i] = pData[HeadLen + i];	
				}
				memcpy(tcp_server_sendbuf,sendbuf,	6 + HeadLen);
				tcp_server_sendlen = 6 + HeadLen;
		}
	}

	void response_read_variables(u8 HeadLen,u8 type,u8 RegNum,u16 StartAdd,u8 *sendbuf,u8 *pData){
		extern u16 send_cout;
		send_cout = HeadLen ;
		sendbuf[HeadLen] = pData[HeadLen] ;
		sendbuf[HeadLen + 1] = pData[HeadLen + 1];
		sendbuf[HeadLen + 2] = RegNum * 2;
		crc16_byte(sendbuf[HeadLen]);
		crc16_byte(sendbuf[HeadLen + 1]);
		crc16_byte(sendbuf[HeadLen + 2]);
		send_cout = HeadLen + 3 ;
		for(i = 0; i < RegNum; i++)
		{
			address = StartAdd + i;
			if(address <= MODBUS_SERIALNUMBER_HIWORD + 1)
			{
				temp1 = 0 ;
				temp2 = modbus.serial_Num[address] ;
				sendbuf[send_cout++] = temp1 ;
				sendbuf[send_cout++] = temp2 ;
				crc16_byte(temp1);
				crc16_byte(temp2);
			}
			else if(address == MODBUS_VERSION_NUMBER_LO)
			{
				temp1 = 0 ;
				temp2 =  (u8)(SOFTREV>>8) ;
				sendbuf[send_cout++] = temp1 ;
				sendbuf[send_cout++] = temp2 ;
				crc16_byte(temp1);
				crc16_byte(temp2);		
			}
			else if(address == MODBUS_VERSION_NUMBER_HI)
			{
				temp1 = 0 ;
				temp2 =  (u8)(SOFTREV) ;
				sendbuf[send_cout++] = temp1 ;
				sendbuf[send_cout++] = temp2 ;
				crc16_byte(temp1);
				crc16_byte(temp2);	
			}
			else if(address == MODBUS_ADDRESS)
			{
				temp1 = 0 ;
				temp2 =  modbus.address;
				sendbuf[send_cout++] = temp1 ;
				sendbuf[send_cout++] = temp2 ;
				crc16_byte(temp1);
				crc16_byte(temp2);		
			}
			else if(address == MODBUS_PRODUCT_MODEL)
			{
				temp1 = 0 ;
				temp2 =  modbus.product;
				sendbuf[send_cout++] = temp1 ;
				sendbuf[send_cout++] = temp2 ;
				crc16_byte(temp1);
				crc16_byte(temp2);
			}
			else if(address == MODBUS_HARDWARE_REV)
			{
				temp1 = 0 ;
				temp2 =  modbus.hardware_Rev;
				sendbuf[send_cout++] = temp1 ;
				sendbuf[send_cout++] = temp2 ;
				crc16_byte(temp1);
				crc16_byte(temp2);				
			}
			else if(address == MODBUS_BAUDRATE)
			{
				temp1 = 0 ;
				temp2 =  modbus.baud;
				sendbuf[send_cout++] = temp1 ;
				sendbuf[send_cout++] = temp2 ;
				crc16_byte(temp1);
				crc16_byte(temp2);				
			}
			
//			else if(address == MODBUS_BASE_ADDRESS)
//			{
//				temp1 = 0 ;
//				temp2 =  0;
//				uart_send[send_cout++] = temp1 ;
//				uart_send[send_cout++] = temp2 ;
//				crc16_byte(temp1);
//				crc16_byte(temp2);
//			}
			else if(address == MODBUS_UPDATE_STATUS)
			{
				temp1 = 0 ;
				temp2 =   modbus.update;
				sendbuf[send_cout++] = temp1 ;
				sendbuf[send_cout++] = temp2 ;
				crc16_byte(temp1);
				crc16_byte(temp2);
			}
			else if(address == MODBUS_SERINALNUMBER_WRITE_FLAG)
			{
				temp1 = 0 ;
				temp2 =  modbus.SNWriteflag ;
				sendbuf[send_cout++] = temp1 ;
				sendbuf[send_cout++] = temp2 ;
				crc16_byte(temp1);
				crc16_byte(temp2);
			}
			else if(address == MODBUS_PROTOCOL_TYPE)
			{
				temp1 = 0 ;
				temp2 =  modbus.protocal ;
				sendbuf[send_cout++] = temp1 ;
				sendbuf[send_cout++] = temp2 ;
				crc16_byte(temp1);
				crc16_byte(temp2);
			}
			else if((address >= MODBUS_MAC_ADDRESS_1)&&(address<= MODBUS_MAC_ADDRESS_6))
			{
				address_temp = address - MODBUS_MAC_ADDRESS_1 ;
				temp1 = 0 ;
				temp2 =  modbus.mac_addr[address_temp] ;
				sendbuf[send_cout++] = temp1 ;
				sendbuf[send_cout++] = temp2 ;
				crc16_byte(temp1);
				crc16_byte(temp2);
			}
			else if(address == MODBUS_IP_MODE)
			{
				temp1 = 0 ;
				temp2 =  modbus.ip_mode ;
				sendbuf[send_cout++] = temp1 ;
				sendbuf[send_cout++] = temp2 ;
				crc16_byte(temp1);
				crc16_byte(temp2);
			}
			else if((address >= MODBUS_IP_ADDRESS_1)&&(address<= MODBUS_IP_ADDRESS_4))
			{
				address_temp = address - MODBUS_IP_ADDRESS_1 ;
				temp1 = 0 ;
				temp2 =  modbus.ip_addr[address_temp] ;
				sendbuf[send_cout++] = temp1 ;
				sendbuf[send_cout++] = temp2 ;
				crc16_byte(temp1);
				crc16_byte(temp2);
			}
			else if((address >= MODBUS_SUB_MASK_ADDRESS_1)&&(address<= MODBUS_SUB_MASK_ADDRESS_4))
			{
				address_temp = address - MODBUS_SUB_MASK_ADDRESS_1 ;
				temp1 = 0 ;
				temp2 =  modbus.mask_addr[address_temp] ;
				sendbuf[send_cout++] = temp1 ;
				sendbuf[send_cout++] = temp2 ;
				crc16_byte(temp1);
				crc16_byte(temp2);
			}
			else if((address >= MODBUS_GATEWAY_ADDRESS_1)&&(address<= MODBUS_GATEWAY_ADDRESS_4))
			{
				address_temp = address - MODBUS_GATEWAY_ADDRESS_1 ;
				temp1 = 0 ;
				temp2 =  modbus.gate_addr[address_temp] ;
				sendbuf[send_cout++] = temp1 ;
				sendbuf[send_cout++] = temp2 ;
				crc16_byte(temp1);
				crc16_byte(temp2);
			}
			else if(address == MODBUS_TCP_SERVER)
			{
				temp1 = 0 ;
				temp2 =  modbus.tcp_server ;
				sendbuf[send_cout++] = temp1 ;
				sendbuf[send_cout++] = temp2 ;
				crc16_byte(temp1);
				crc16_byte(temp2);
			}
			else if(address == MODBUS_LISTEN_PORT)
			{
				temp1 = (modbus.listen_port>>8)&0xff ;
				temp2 =  modbus.listen_port &0xff ;
				sendbuf[send_cout++] = temp1 ;
				sendbuf[send_cout++] = temp2 ;
				crc16_byte(temp1);
				crc16_byte(temp2);
			}
			else if(address == MODBUS_GHOST_IP_MODE)
			{
				temp1 = 0 ;
				temp2 =  modbus.ghost_ip_mode;
				sendbuf[send_cout++] = temp1 ;
				sendbuf[send_cout++] = temp2 ;
				crc16_byte(temp1);
				crc16_byte(temp2);
			}
			else if((address >= MODBUS_GHOST_IP_ADDRESS_1)&&(address<= MODBUS_GHOST_IP_ADDRESS_4))
			{
				address_temp = address - MODBUS_GHOST_IP_ADDRESS_1 ;
				temp1 = 0 ;
				temp2 =  modbus.ghost_ip_addr[address_temp] ;
				sendbuf[send_cout++] = temp1 ;
				sendbuf[send_cout++] = temp2 ;
				crc16_byte(temp1);
				crc16_byte(temp2);
			}
			
			else if((address >= MODBUS_GHOST_SUB_MASK_ADDRESS_1)&&(address<= MODBUS_GHOST_SUB_MASK_ADDRESS_4))
			{
				address_temp = address - MODBUS_GHOST_SUB_MASK_ADDRESS_1 ;
				temp1 = 0 ;
				temp2 =  modbus.ghost_mask_addr[address_temp] ;
				sendbuf[send_cout++] = temp1 ;
				sendbuf[send_cout++] = temp2 ;
				crc16_byte(temp1);
				crc16_byte(temp2);
			}
			else if((address >= MODBUS_GHOST_GATEWAY_ADDRESS_1)&&(address<= MODBUS_GHOST_GATEWAY_ADDRESS_4))
			{
				address_temp = address - MODBUS_GHOST_GATEWAY_ADDRESS_1 ;
				temp1 = 0 ;
				temp2 =  modbus.ghost_gate_addr[address_temp] ;
				sendbuf[send_cout++] = temp1 ;
				sendbuf[send_cout++] = temp2 ;
				crc16_byte(temp1);
				crc16_byte(temp2);
			}
			else if(address == MODBUS_GHOST_TCP_SERVER)
			{
				temp1 = 0 ;
				temp2 =  modbus.ghost_tcp_server ;
				sendbuf[send_cout++] = temp1 ;
				sendbuf[send_cout++] = temp2 ;
				crc16_byte(temp1);
				crc16_byte(temp2);
			}
			else if(address == MODBUS_GHOST_LISTEN_PORT)
			{
				temp1 = (modbus.ghost_listen_port>>8)&0xff ;
				temp2 =  modbus.ghost_listen_port &0xff ;
				sendbuf[send_cout++] = temp1 ;
				sendbuf[send_cout++] = temp2 ;
				crc16_byte(temp1);
				crc16_byte(temp2);
			}
			else if(address == MODBUS_WRITE_GHOST_SYSTEM)
				{
				temp1 = 0 ;
				temp2 = modbus.write_ghost_system;
				sendbuf[send_cout++] = temp1 ;
				sendbuf[send_cout++] = temp2 ;
				crc16_byte(temp1);
				crc16_byte(temp2);
			}
			else if(address == MODBUS_MAC_ENABLE)
			{				
				temp1 = 0 ;
				temp2 = modbus.mac_enable;
				sendbuf[send_cout++] = temp1 ;
				sendbuf[send_cout++] = temp2 ;
				crc16_byte(temp1);
				crc16_byte(temp2);	
			}
			#ifdef T38AI8AO6DO
			else if((address >= MODBUS_AO_CHANNLE0)&&(address<= MODBUS_AO_CHANNLE7))
			{
				address_temp = address - MODBUS_AO_CHANNLE0 ; 
				temp1 = (modbus.analog_output[address_temp]>>8)&0xff ;
				temp2 =  modbus.analog_output[address_temp]&0xff; 
				sendbuf[send_cout++] = temp1 ;
				sendbuf[send_cout++] = temp2 ;
				crc16_byte(temp1);
				crc16_byte(temp2);
			}
			else if((address >= MODBUS_DO_CHANNLE0)&&(address<= MODBUS_DO_CHANNLE5))
			{
				address_temp = address - MODBUS_DO_CHANNLE0 ; 
				temp1 = 0;
				temp2 =  modbus.digit_output[address_temp]&0xff; 
				sendbuf[send_cout++] = temp1 ;
				sendbuf[send_cout++] = temp2 ;
				crc16_byte(temp1);
				crc16_byte(temp2);
			}
			else if(address == MODBUS_SWITCH_BANK0)
			{
					temp1 = (modbus.switch_gourp[1]>>8) & 0xff ;
					temp2 = modbus.switch_gourp[1]&0xff ;
					sendbuf[send_cout++] = temp1 ;
					sendbuf[send_cout++] = temp2 ;
					crc16_byte(temp1);
					crc16_byte(temp2);
			}
			else if(address == MODBUS_SWITCH_BANK1)
			{
					temp1 = (modbus.switch_gourp[0]>>8) & 0xff ;
					temp2 = modbus.switch_gourp[0]&0xff ;
					sendbuf[send_cout++] = temp1 ;
					sendbuf[send_cout++] = temp2 ;
					crc16_byte(temp1);
					crc16_byte(temp2);
			}
//			else if((address >= MODBUS_AI_CHANNLE0)&&(address<= MODBUS_AI_CHANNLE7))
//			{
//				address_temp = address - MODBUS_AI_CHANNLE0 ; 
//				temp1 = (modbus.input[address_temp]>>8)&0xff ;
//				temp2 =  modbus.input[address_temp]&0xff; 			
//				sendbuf[send_cout++] = temp1 ;
//				sendbuf[send_cout++] = temp2 ;
//				crc16_byte(temp1);
//				crc16_byte(temp2);
//			}
			
			else if((address >= MODBUS_AI_CHANNLE0_HI)&&(address<= MODBUS_AI_CHANNLE7_LO))
			{
				address_temp = address - MODBUS_AI_CHANNLE0_HI ;			
				if(modbus.range[address_temp/2] == PULSE)
				{	
						temp1 = modbus.pulse[address_temp/2].quarter[(3-((address_temp%2)<<1))] ;
						temp2 = modbus.pulse[address_temp/2].quarter[2-((address_temp%2)<<1)] ;
						sendbuf[send_cout++] = temp1 ;
						sendbuf[send_cout++] = temp2 ;
						crc16_byte(temp1);
						crc16_byte(temp2);
				}
				else
				{
					if(address_temp%2 == 1)
					{						
						temp1 = (modbus.input[address_temp/2]>>8)&0xff ;
						temp2 =  modbus.input[address_temp/2]&0xff; 
						sendbuf[send_cout++] = temp1 ;
						sendbuf[send_cout++] = temp2 ;
						crc16_byte(temp1);
						crc16_byte(temp2);
					}
					else
					{
						temp1 = 0 ;
						temp2 = 0 ;
						sendbuf[send_cout++] = temp1 ;
						sendbuf[send_cout++] = temp2 ;
						crc16_byte(temp1);
						crc16_byte(temp2);
					
					}
				}
			}
			else if((address >= MODBUS_AI_FILTER0)&&(address<= MODBUS_AI_FILTER7))
			{
				address_temp = address - MODBUS_AI_FILTER0 ; 
				temp1 = (modbus.filter_value[address_temp]>>8)&0xff ;
				temp2 =  modbus.filter_value[address_temp]&0xff; 
				sendbuf[send_cout++] = temp1 ;
				sendbuf[send_cout++] = temp2 ;
				crc16_byte(temp1);
				crc16_byte(temp2);
			}
			else if((address >= MODBUS_AI_RANGE0)&&(address<= MODBUS_AI_RANGE7))
			{
				address_temp = address - MODBUS_AI_RANGE0 ; 
				temp1 = (modbus.range[address_temp]>>8)&0xff ;
				temp2 =  modbus.range[address_temp]&0xff; 
				sendbuf[send_cout++] = temp1 ;
				sendbuf[send_cout++] = temp2 ;
				crc16_byte(temp1);
				crc16_byte(temp2);
			}
			else if((address >= MODBUS_AI_OFFSET0)&&(address<= MODBUS_AI_OFFSET7))
			{
				address_temp = address - MODBUS_AI_OFFSET0 ; 				
				temp1 = (modbus.offset[address_temp]>>8)&0xff ;
				temp2 =  modbus.offset[address_temp]&0xff; 
				sendbuf[send_cout++] = temp1 ;
				sendbuf[send_cout++] = temp2 ;
				crc16_byte(temp1);
				crc16_byte(temp2);
			}
			/*else if((address >= MODBUS_AI0_CUSTOMER_HI)&&(address<= MODBUS_AI7_CUSTOMER_LO))
			{
				address_temp = address - MODBUS_AI0_CUSTOMER_HI ; 	
				div_temp = address_temp %2 ;
				address_buf = address_temp /2 ;
				if(div_temp == 0)
				{
					temp1 = (modbus.customer_range_hi[address_buf]>>8)&0xff ;
					temp2 =  modbus.customer_range_hi[address_buf]&0xff; 
									
				}
				else
				{
					temp1 = (modbus.customer_range_lo[address_buf]>>8)&0xff ;
					temp2 =  modbus.customer_range_lo[address_buf]&0xff; 
				}
				sendbuf[send_cout++] = temp1 ;
				sendbuf[send_cout++] = temp2 ;
				crc16_byte(temp1);
				crc16_byte(temp2);
			}
			else if((address >= MODBUS_CUSTOMER_CHANNEL0_ENABLE)&&(address<= MODBUS_CUSTOMER_CHANNEL7_ENABLE))
			{
				address_temp = address - MODBUS_CUSTOMER_CHANNEL0_ENABLE ; 				
				temp1 = 0 ;
				temp2 =  modbus.customer_enable[address_temp]; 
				sendbuf[send_cout++] = temp1 ;
				sendbuf[send_cout++] = temp2 ;
				crc16_byte(temp1);
				crc16_byte(temp2);
			}*/
			#endif
			
			#ifdef T322AI
			else if((address >= MODBUS_AI_CHANNLE0_HI)&&(address<= MODBUS_AI_CHANNLE21_LO))
			{
				address_temp = address - MODBUS_AI_CHANNLE0_HI ;			
				if(modbus.range[address_temp/2] == HI_spd_count)
				{	
						temp1 = modbus.pulse[address_temp/2].quarter[(3-((address_temp%2)<<1))] ;
						temp2 = modbus.pulse[address_temp/2].quarter[2-((address_temp%2)<<1)] ;
						sendbuf[send_cout++] = temp1 ;
						sendbuf[send_cout++] = temp2 ;
						crc16_byte(temp1);
						crc16_byte(temp2);
				}
				else
				{
					if(address_temp%2 == 1)
					{						
						temp1 = (modbus.input[address_temp/2]>>8)&0xff ;
						temp2 =  modbus.input[address_temp/2]&0xff; 
						sendbuf[send_cout++] = temp1 ;
						sendbuf[send_cout++] = temp2 ;
						crc16_byte(temp1);
						crc16_byte(temp2);
					}
					else
					{
						temp1 = 0 ;
						temp2 = 0 ;
						sendbuf[send_cout++] = temp1 ;
						sendbuf[send_cout++] = temp2 ;
						crc16_byte(temp1);
						crc16_byte(temp2);
					
					}
				}
			}
//			else if((address >= MODBUS_AI_CHANNLE11)&&(address<= MODBUS_AI_CHANNLE21))
//			{
//				address_temp = address - MODBUS_AI_CHANNLE11 ; 
//				temp1 = (modbus.input[address_temp+11]>>8)&0xff ;
//				temp2 =  modbus.input[address_temp+11]&0xff; 
//				sendbuf[send_cout++] = temp1 ;
//				sendbuf[send_cout++] = temp2 ;
//				crc16_byte(temp1);
//				crc16_byte(temp2);
//			}
			else if((address >= MODBUS_AI_FILTER0)&&(address<= MODBUS_AI_FILTER21))
			{
				address_temp = address - MODBUS_AI_FILTER0 ; 
				temp1 = (modbus.filter_value[address_temp]>>8)&0xff ;
				temp2 =  modbus.filter_value[address_temp]&0xff; 
				sendbuf[send_cout++] = temp1 ;
				sendbuf[send_cout++] = temp2 ;
				crc16_byte(temp1);
				crc16_byte(temp2);
			}
			else if((address >= MODBUS_AI_RANGE0)&&(address<= MODBUS_AI_RANGE21))
			{
				address_temp = address - MODBUS_AI_RANGE0 ; 
				temp1 = (modbus.range[address_temp]>>8)&0xff ;
				temp2 =  modbus.range[address_temp]&0xff; 
				sendbuf[send_cout++] = temp1 ;
				sendbuf[send_cout++] = temp2 ;
				crc16_byte(temp1);
				crc16_byte(temp2);
			}
			else if((address >= MODBUS_AI_OFFSET0)&&(address<= MODBUS_AI_OFFSET21))
			{
				address_temp = address - MODBUS_AI_OFFSET0 ; 				
				temp1 = (modbus.offset[address_temp]>>8)&0xff ;
				temp2 =  modbus.offset[address_temp]&0xff; 
				sendbuf[send_cout++] = temp1 ;
				sendbuf[send_cout++] = temp2 ;
				crc16_byte(temp1);
				crc16_byte(temp2);
			}
//			else if((address >= MODBUS_AI0_CUSTOMER_HI)&&(address<= MODBUS_AI21_CUSTOMER_LO))
//			{
//				address_temp = address - MODBUS_AI0_CUSTOMER_HI ; 	
//				div_temp = address_temp %2 ;
//				address_buf = address_temp /2 ;
//				if(div_temp == 0)
//				{
//					temp1 = (modbus.customer_range_hi[address_buf]>>8)&0xff ;
//					temp2 =  modbus.customer_range_hi[address_buf]&0xff; 
//				}
//				else
//				{
//					temp1 = (modbus.customer_range_lo[address_buf]>>8)&0xff ;
//					temp2 =  modbus.customer_range_lo[address_buf]&0xff; 
//				}
//				sendbuf[send_cout++] = temp1 ;
//				sendbuf[send_cout++] = temp2 ;
//				crc16_byte(temp1);
//				crc16_byte(temp2);
//			}
//			else if((address >= MODBUS_CUSTOMER_CHANNEL0_ENABLE)&&(address<= MODBUS_CUSTOMER_CHANNEL21_ENABLE))
//			{
//				address_temp = address - MODBUS_CUSTOMER_CHANNEL0_ENABLE ; 				
//				temp1 = 0 ;
//				temp2 =  modbus.customer_enable[address_temp]; 
//				sendbuf[send_cout++] = temp1 ;
//				sendbuf[send_cout++] = temp2 ;
//				crc16_byte(temp1);
//				crc16_byte(temp2);
//			}
			#endif
			
			#ifdef T3PT12
			else if((address >= MODBUS_TEMP0_HI)&&(address<= MODBUS_TEMP11_LO))
			{
				if(int_float == 1)				
				 {
					 if(( address-MODBUS_TEMP0_HI)%2 == 0)
					 {
						temp1 = 0 ;
						temp2 = 0; 	
					 }
					 else
					 {
						temp1 = (temperature[(address-MODBUS_TEMP0_HI)/2]>>8)&0xff ;
						temp2 =  (temperature[(address-MODBUS_TEMP0_HI)/2])&0xff ; 	
					 }
				 }
				 else
				 {
					temp1 = _temp_value[(address - MODBUS_TEMP0_HI) / 2].qauter_temp_C[3 - (address - MODBUS_TEMP0_HI) % 2 * 2 - 1] ;
					temp2 = _temp_value[(address - MODBUS_TEMP0_HI) / 2].qauter_temp_C[3 - (address - MODBUS_TEMP0_HI) % 2 * 2] ;	
				 }
				sendbuf[send_cout++] = temp1 ;
				sendbuf[send_cout++] = temp2 ;
				crc16_byte(temp1);
				crc16_byte(temp2);
			}
			else if(address >= MODBUS_TEMPERATURE_OFFSET0_HI &&  address <= MODBUS_TEMPERATURE_OFFSET11_LO)
			{				
//				Send_Byte(_offset_rc[0].qauter_temp_C[2], CRC_NO);
//				Send_Byte(_offset_rc[0].qauter_temp_C[3], CRC_NO);
				temp1 = _offset_rc[(address - MODBUS_TEMPERATURE_OFFSET0_HI) / 2].qauter_temp_C[3 - (address - MODBUS_TEMPERATURE_OFFSET0_HI) % 2 * 2 - 1] ;
				temp2 = _offset_rc[(address - MODBUS_TEMPERATURE_OFFSET0_HI) / 2].qauter_temp_C[3 - (address - MODBUS_TEMPERATURE_OFFSET0_HI) % 2 * 2]; 
				sendbuf[send_cout++] = temp1 ;
				sendbuf[send_cout++] = temp2 ;
				crc16_byte(temp1);
				crc16_byte(temp2);				
			}
			else if((address >= MODBUS_CHANNEL0_RANGE)&&(address<= MODBUS_CHANNEL11_RANGE))
			{
				address_temp = address - MODBUS_CHANNEL0_RANGE ; 
				temp1 = 0 ;
				temp2 =  modbus.range[address_temp]&0xff; 
				sendbuf[send_cout++] = temp1 ;
				sendbuf[send_cout++] = temp2 ;
				crc16_byte(temp1);
				crc16_byte(temp2);
			}
			else if(address == MODBUS_RESOLE_BIT)
			{
				temp1 = 0;
				temp2 =  modbus.resole_bit; 
				sendbuf[send_cout++] = temp1 ;
				sendbuf[send_cout++] = temp2 ;
				crc16_byte(temp1);
				crc16_byte(temp2);	
			}
			else if((address >= MODBUS_CHANNEL0_FILTER)&&(address<= MODBUS_CHANNEL11_FILTER))
			{
				address_temp = address - MODBUS_CHANNEL0_FILTER ; 
				temp1 = (modbus.filter_value[address_temp]>>8)&0xff ;
				temp2 =  modbus.filter_value[address_temp]&0xff; 
				sendbuf[send_cout++] = temp1 ;
				sendbuf[send_cout++] = temp2 ;
				crc16_byte(temp1);
				crc16_byte(temp2);
			}
			else if(address == MODBUS_RESOLE_BIT)
			{
				temp1 = 0 ;
				temp2 =  modbus.resole_bit; 
				sendbuf[send_cout++] = temp1 ;
				sendbuf[send_cout++] = temp2 ;
				crc16_byte(temp1);
				crc16_byte(temp2);	
			}
			else if((address >= MODBUS_POINT_AD0_HI)&&(address<= MODBUS_CHANNEL_AD11_LO))
			{
				address_temp = address - MODBUS_POINT_AD0_HI ; 	
				div_temp = address_temp %2 ;
				address_buf = address_temp /2 ;
				if(div_temp == 0)
				{
					temp1 = (rs_data[address_buf]>>24)&0xff ;
					temp2 =  (rs_data[address_buf]>>16)&0xff; 
				}
				else
				{
					temp1 = (rs_data[address_buf]>>8)&0xff ;
					temp2 =  (rs_data[address_buf])&0xff; 
				}
				sendbuf[send_cout++] = temp1 ;
				sendbuf[send_cout++] = temp2 ;
				crc16_byte(temp1);
				crc16_byte(temp2);
			
			}
			#endif
			#ifndef T3PT12
			else if((address >= MODBUS_CUSTOMER_TABLE1_VOL_0)&&(address<= MODBUS_CUSTOMER_TABLE1_VAL_10))
			{
				address_temp = address - MODBUS_CUSTOMER_TABLE1_VOL_0 ; 	
				div_temp = address_temp %2 ;
				address_buf = address_temp /2 ;
				if(div_temp == 0)
				{
					temp1 = (modbus.customer_table1_vol[address_buf]>>16)&0xff ;
					temp2 =  modbus.customer_table1_vol[address_buf]&0xff; 
				}
				else
				{
					temp1 = (modbus.customer_table1_val[address_buf]>>16)&0xff ;
					temp2 =  modbus.customer_table1_val[address_buf]&0xff; 
				}					
			}
			else if((address >= MODBUS_CUSTOMER_TABLE2_VOL_0)&&(address<= MODBUS_CUSTOMER_TABLE2_VAL_10))
			{
				address_temp = address - MODBUS_CUSTOMER_TABLE2_VOL_0 ; 	
				div_temp = address_temp %2 ;
				address_buf = address_temp /2 ;
				if(div_temp == 0)
				{
					temp1 = (modbus.customer_table2_vol[address_buf]>>16)&0xff ;
					temp2 =  modbus.customer_table2_vol[address_buf]&0xff; 
				}
				else
				{
					temp1 = (modbus.customer_table2_val[address_buf]>>16)&0xff ;
					temp2 =  modbus.customer_table2_val[address_buf]&0xff; 
				}					
			}
			else if((address >= MODBUS_CUSTOMER_TABLE3_VOL_0)&&(address<= MODBUS_CUSTOMER_TABLE3_VAL_10))
			{
				address_temp = address - MODBUS_CUSTOMER_TABLE3_VOL_0 ; 	
				div_temp = address_temp %2 ;
				address_buf = address_temp /2 ;
				if(div_temp == 0)
				{
					temp1 = (modbus.customer_table3_vol[address_buf]>>16)&0xff ;
					temp2 =  modbus.customer_table3_vol[address_buf]&0xff; 
				}
				else
				{
					temp1 = (modbus.customer_table3_val[address_buf]>>16)&0xff ;
					temp2 =  modbus.customer_table3_val[address_buf]&0xff; 
				}					
			}
			else if((address >= MODBUS_CUSTOMER_TABLE4_VOL_0)&&(address<= MODBUS_CUSTOMER_TABLE4_VAL_10))
			{
				address_temp = address - MODBUS_CUSTOMER_TABLE4_VOL_0 ; 	
				div_temp = address_temp %2 ;
				address_buf = address_temp /2 ;
				if(div_temp == 0)
				{
					temp1 = (modbus.customer_table4_vol[address_buf]>>16)&0xff ;
					temp2 =  modbus.customer_table4_vol[address_buf]&0xff; 
				}
				else
				{
					temp1 = (modbus.customer_table4_val[address_buf]>>16)&0xff ;
					temp2 =  modbus.customer_table4_val[address_buf]&0xff; 
				}					
			}
			else if((address >= MODBUS_CUSTOMER_TABLE5_VOL_0)&&(address<= MODBUS_CUSTOMER_TABLE5_VAL_10))
			{
				address_temp = address - MODBUS_CUSTOMER_TABLE5_VOL_0 ; 	
				div_temp = address_temp %2 ;
				address_buf = address_temp /2 ;
				if(div_temp == 0)
				{
					temp1 = (modbus.customer_table5_vol[address_buf]>>16)&0xff ;
					temp2 =  modbus.customer_table5_vol[address_buf]&0xff; 
				}
				else
				{
					temp1 = (modbus.customer_table5_val[address_buf]>>16)&0xff ;
					temp2 =  modbus.customer_table5_val[address_buf]&0xff; 
				}					
			}
			
			#endif
			
			else if(address == MODBUS_RESET)
			{ 				
				
				temp1 = 0 ;
				temp2 = modbus.reset ;
				sendbuf[send_cout++] = temp1 ;
				sendbuf[send_cout++] = temp2 ;
				crc16_byte(temp1);
				crc16_byte(temp2);
			}
//			else if(address == MODBUS_TEST2)
//			{ 				
//				sendbuf[send_cout++] = temp1 ;
//				sendbuf[send_cout++] = temp2 ;
//				crc16_byte(temp1);
//				crc16_byte(temp2);
//			}
//			else if(address == MODBUS_TEST3)
//			{ 				
//				sendbuf[send_cout++] = temp1 ;
//				sendbuf[send_cout++] = temp2 ;
//				crc16_byte(temp1);
//				crc16_byte(temp2);
//			}
//			else if(address == MODBUS_TEST4)
//			{ 				
//				sendbuf[send_cout++] = temp1 ;
//				sendbuf[send_cout++] = temp2 ;
//				crc16_byte(temp1);
//				crc16_byte(temp2);
//			}
//			else if(address == MODBUS_TEST5)
//			{ 				
//				sendbuf[send_cout++] = temp1 ;
//				sendbuf[send_cout++] = temp2 ;
//				crc16_byte(temp1);
//				crc16_byte(temp2);
//			}
//			else if(address == MODBUS_TEST6)
//			{ 				
//				sendbuf[send_cout++] = temp1 ;
//				sendbuf[send_cout++] = temp2 ;
//				crc16_byte(temp1);
//				crc16_byte(temp2);
//			}
//			else if(address == MODBUS_TEST7)
//			{ 				 
//				sendbuf[send_cout++] = temp1 ;
//				sendbuf[send_cout++] = temp2 ;
//				crc16_byte(temp1);
//				crc16_byte(temp2);
//			}
//			else if(address == MODBUS_TEST8)
//			{ 				
//				sendbuf[send_cout++] = temp1 ;
//				sendbuf[send_cout++] = temp2 ;
//				crc16_byte(temp1);
//				crc16_byte(temp2);
//			}
//			else if(address == MODBUS_TEST9)
//			{ 				
//				sendbuf[send_cout++] = temp1 ;
//				sendbuf[send_cout++] = temp2 ;
//				crc16_byte(temp1);
//				crc16_byte(temp2);
//			}
//			else if(address == MODBUS_TEST10)
//			{ 				 
//				sendbuf[send_cout++] = temp1 ;
//				sendbuf[send_cout++] = temp2 ;
//				crc16_byte(temp1);
//				crc16_byte(temp2);
//			}
			else
			{
				temp1 = 0 ;
				temp2 =  0; 
				sendbuf[send_cout++] = temp1 ;
				sendbuf[send_cout++] = temp2 ;
				crc16_byte(temp1);
				crc16_byte(temp2);
			}

		}//end of number
		temp1 = CRChi ;
		temp2 =  CRClo; 
		sendbuf[send_cout++] = temp1 ;
		sendbuf[send_cout++] = temp2 ;
		if(type == 0)
		{
			memcpy(uart_send, sendbuf, send_cout);
			USART_SendDataString(send_cout);
		}
		else
		{
				sendbuf[0] = pData[0] ;
				sendbuf[1] = pData[1] ;
				sendbuf[2] = 0 ;
				sendbuf[3] = 0 ;
				sendbuf[4] = (3 + RegNum * 2) >> 8; 
				sendbuf[5] =(u8)(3 + RegNum * 2) ;
				memcpy(tcp_server_sendbuf,sendbuf,RegNum * 2 + 3 + HeadLen);
				tcp_server_sendlen = RegNum * 2 + 3 + HeadLen;
		}
		
	}
	
	void response_checkonline(u8 HeadLen,u8 *pData){
			send_cout = HeadLen ;
		
			temp2 =  pData[HeadLen+0]; 
			uart_send[send_cout++] = temp2 ;
			crc16_byte(temp2);
		

			temp2 = pData[HeadLen+1]; 
			uart_send[send_cout++] = temp2 ;
			crc16_byte(temp2);
		

			temp2 =  modbus.address; 
			uart_send[send_cout++] = temp2 ;
			crc16_byte(temp2);
			

			temp2 =  modbus.serial_Num[0]; 
			uart_send[send_cout++] = temp2 ;
			crc16_byte(temp2);
			
			temp2 =  modbus.serial_Num[1]; 
			uart_send[send_cout++] = temp2 ;
			crc16_byte(temp2);
			

			temp2 =  modbus.serial_Num[2]; 
			uart_send[send_cout++] = temp2 ;
			crc16_byte(temp2);
			

			temp2 =  modbus.serial_Num[3]; 
			uart_send[send_cout++] = temp2 ;
			crc16_byte(temp2);
			
			temp2 =  CRChi; 
			uart_send[send_cout++] = temp2 ;
			temp2 =  CRClo; 
			uart_send[send_cout++] = temp2 ;
			USART_SendDataString(send_cout);
	}
	