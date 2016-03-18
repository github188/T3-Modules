#include "stdlib.h"
#include "crc.h"
#include "usart.h"
#include "24cxx.h" 
#include "delay.h"
#include "define.h"
#include "modbus.h"
#include "registerlist.h"
#include "inputs.h"
#include "../filter/filter.h"
#include "led.h"
#include "fifo.h"
#include "bacnettimer.h"
#include "../KEY/key.h"
#include "read_pt.h"
#include "tcp_modbus.h"
#include "tapdev.h"
#include "ud_str.h"

static u8 randval = 0 ;
u8 i2c_test[10] ;
u8 USART_RX_BUF[USART_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
u8 uart_send[USART_SEND_LEN] ;
vu8 transmit_finished = 0 ; 
vu8 revce_count = 0 ;
vu8 rece_size = 0 ;
vu8 serial_receive_timeout_count ;
u8 SERIAL_RECEIVE_TIMEOUT ;
u8 dealwithTag ;
STR_MODBUS modbus ;
u8 DealwithTag ;
u16 sendbyte_num = 0 ;
u16 uart_num = 0 ;
 u8  	Station_NUM= 12;
//extern uint8_t Receive_Buffer_Data0[512];
extern FIFO_BUFFER Receive_Buffer0;
 
 
void USART1_IRQHandler(void)                	//This is the interrupt request handler for USART1
{		
	u8 receive_buf ;
	static u16 send_count = 0 ;
	if(USART_GetITStatus(USART1, USART_IT_RXNE) == SET)	//接收中断
	{
			if(modbus.protocal == MODBUS )	
			{
					if(revce_count < 250)
						USART_RX_BUF[revce_count++] = USART_ReceiveData(USART1);	//(USART1->DR);		//读取接收到的数据
						else
							 serial_restart();
						if(revce_count == 1)
						{
							// This starts a timer that will reset communication.  If you do not
							// receive the full packet, it insures that the next receive will be fresh.
							// The timeout is roughly 7.5ms.  (3 ticks of the hearbeat)
							rece_size = 250;
							serial_receive_timeout_count = SERIAL_RECEIVE_TIMEOUT;
						}
						else if(revce_count == 3 )
						{
							if(USART_RX_BUF[1] == CHECKONLINE)
							rece_size = 6;
						}
						else if(revce_count == 4)
						{
							//check if it is a scan command
							if((((vu16)(USART_RX_BUF[2] << 8) + USART_RX_BUF[3]) == 0x0a) && (USART_RX_BUF[1] == WRITE_VARIABLES))
							{
								rece_size = DATABUFLEN_SCAN;
								serial_receive_timeout_count = SERIAL_RECEIVE_TIMEOUT;	
							}
						}
						else if(revce_count == 7)
						{
							if((USART_RX_BUF[1] == READ_VARIABLES) || (USART_RX_BUF[1] == WRITE_VARIABLES))
							{
								rece_size = 8;
								//dealwithTag = 1;
							}
							else if(USART_RX_BUF[1] == MULTIPLE_WRITE)
							{
								rece_size = USART_RX_BUF[6] + 9;
								serial_receive_timeout_count = USART_RX_BUF[6] + 8;
							}
							else
							{
								rece_size = 250;
							}
						}
						else if(revce_count == rece_size)		
						{
							// full packet received - turn off serial timeout
							serial_receive_timeout_count = 0;
							dealwithTag = 2;		// making this number big to increase delay
							rx_count = 2 ;
						}
		
			}
			else if(modbus.protocal == BAC_MSTP )
			{
					if(USART_GetITStatus(USART1, USART_IT_RXNE) == SET)
					{
							receive_buf =  USART_ReceiveData(USART1); 
							FIFO_Put(&Receive_Buffer0, receive_buf);
					}
			}
	}
//	else if( USART_GetITStatus(USART1, USART_IT_TC) == SET  )
//     {
//         if( uart_num >=sendbyte_num)
//		 {
//             USART_ClearFlag(USART1, USART_FLAG_TC);
//			 uart_num = 0 ;
//		 }
//         else
//             USART_SendData(USART1, pDataByte[uart_num++]);
//	}
	else  if( USART_GetITStatus(USART1, USART_IT_TXE) == SET  )
     {
        if((modbus.protocal == MODBUS )||(modbus.protocal == BAC_MSTP))
				{
					 if( send_count > sendbyte_num)
						{
								 USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
								 send_count = 0 ;
								 Timer_Silence_Reset();
								 serial_restart();
						}
					 else
					 {
							USART_SendData(USART1, uart_send[send_count++] );
							Timer_Silence_Reset();
					 }
		   }
	
	
					
     }
}

void serial_restart(void)											// Clears the Transmit flag. Used for serial restart.
{
	TXEN = RECEIVE;
	revce_count = 0;
	dealwithTag = 0;
} 

//it is ready to send data by serial port . 
static void initSend_COM(void)								//Initialize the sending operation by setting the Transmit flag.
{
	TXEN = SEND;
}

void send_byte(u8 ch, u8 crc)
{	
	USART_ClearFlag(USART1, USART_FLAG_TC); 
	USART_SendData(USART1,  ch);
	tx_count = 2 ;
	if(crc)
	{
		crc16_byte(ch);
	}
}
//void USART_SendDataString(u8 *pbuf, u8 num)
// {
//     sendbyte_num = num ;
//	 uart_num = 0 ;
//	 pDataByte = pbuf ;
//	 USART_ClearFlag(USART1, USART_FLAG_TC);   
//     USART_SendData(USART1, pDataByte[uart_num++] ); 
//	 tx_count = 20 ;
// }
 void USART_SendDataString( u16 num )		//This function has been used to enable the Transmit interrupt 
 {
	 tx_count = 2 ;
	 sendbyte_num = num;
	 uart_num = 0 ;
   USART_ITConfig(USART1, USART_IT_TXE, ENABLE);//
 }
void modbus_init(void)									//This just sets few parameters for the modbus initialization and restarts serial
{
	//uart1_init(19200);
	serial_restart();
	SERIAL_RECEIVE_TIMEOUT = 3;
	serial_receive_timeout_count = SERIAL_RECEIVE_TIMEOUT;

//	laddress = read_eeprom(EEP_ADDRESS);
//	if((laddress == 255) || (laddress == 0))
//	{
//		laddress = 254;
//	}
//	laddress = 254;
//	update_flash = 0;
}

																				//Write to the EEPROM programmer (24cxx)
void internalDeal(u8 type,  u8 *pData)	//Store the data from the master used for the register read/write operations
{
	u8 i ;
	u8 HeadLen ;
	u16 StartAdd ;
	// HeadLen is used for indexing the Modbus commands that are stored in "pData" array [L]
	if(type == 0)
	{
		HeadLen = 0 ;	
	}
	else
	{
		HeadLen = 6 ;

	}
	StartAdd = (u16)(pData[HeadLen + 2] <<8 ) + pData[HeadLen + 3];	// Formed a Start Address [L]
	 if (pData[HeadLen + 1] == MULTIPLE_WRITE) //multi_write
	{
		multiplewrite_register_modbus(StartAdd, pData, HeadLen);
	}
	else if(pData[HeadLen + 1] == WRITE_VARIABLES)
	{
		write_register_modbus(StartAdd, pData, HeadLen);
	}
//	else if(pData[HeadLen + 1] == MULTIPLE_WRITE)
//	{
//		uint8_t packet_counter ;
//		packet_counter = pData[HeadLen+6];			// for now buffer[4] will remain zero...
//		if()
//	
//	}
	
	if (modbus.update == 0x7F)
	{
		SoftReset();		
	}
	else if(modbus.update == 0x8e)
	{
		
		//address_temp = AT24CXX_ReadOneByte(EEP_UPDATE_STATUS);
		for(i=0; i<255; i++)
		AT24CXX_WriteOneByte(i, 0xff);
		
		
		EEP_Dat_Init();
//		SoftReset();
		AT24CXX_WriteOneByte(EEP_UPDATE_STATUS, 0);
		modbus.SNWriteflag = 0x00;
		AT24CXX_WriteOneByte(EEP_SERIALNUMBER_WRITE_FLAG, 0);
		SoftReset();
	}
//	// --------------- reset board -------------------------------------------
//	else if (update_flash == 0xFF)
//	{	
////		// disable the global interrupts
////		EA = 0;
////		WDTC = 0x80; 	//reset the CPU
////		while(1){};

//	}
}


//static void responseData(u16 start_address)
void responseCmd(u8 type, u8* pData)		//Respond to the request from the master
{
	u8  i, temp1 =0, temp2 = 0;
	u16 send_cout = 0 ;
	u8  sendbuf[300];
	u8 HeadLen = 0 ;
	u16  RegNum;
	u8 cmd  ;
	u16 StartAdd ;
	if(type == 0)
	{
		HeadLen = 0 ;	
	}
	else
	{
		HeadLen = 6 ;
		for(i=0; i<6; i++)
		{
			sendbuf[i] = 0 ;	
		}
		
	}
	cmd = pData[HeadLen + 1]; 
	StartAdd = (u16)(pData[HeadLen + 2] <<8 ) + pData[HeadLen + 3];
	RegNum = (u8)pData[HeadLen + 5];
	
	
	if(cmd == WRITE_VARIABLES)
	{
		
		
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
	else if(cmd == MULTIPLE_WRITE)
	{
		//send_cout = HeadLen ;

		
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
	else if(cmd == READ_VARIABLES)
	{
		u16 address;
		u16 address_temp ;
		u16 div_temp ;
		u16 address_buf ;		
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

	
	else if (USART_RX_BUF[1] == CHECKONLINE)
	{
		// send first byte of information
			
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
}
u8 checkData(u16 address)								//This function is used by the slave to check whether the commands received are valid
{
	//static unsigned char xdata rand_read_ten_count = 0 ;
	u16 crc_val;
	u8 minaddr,maxaddr, variable_delay;
	u8 i;
	static u8 srand_count =0 ;
	srand_count ++ ;
	// check if packet completely received
	if(revce_count != rece_size)
		return 0;

	// check if talking to correct device ID
	if(USART_RX_BUF[0] != 255 && USART_RX_BUF[0] != modbus.address && USART_RX_BUF[0] != 0)
		return 0;	

	//  --- code to verify what is on the network ---------------------------------------------------
	if( USART_RX_BUF[1] == CHECKONLINE)
	{

	crc_val = crc16(USART_RX_BUF,4) ;
	if(crc_val != (USART_RX_BUF[4]<<8) + USART_RX_BUF[5] )
	{
		return 0;
	}
	minaddr = (USART_RX_BUF[2] >= USART_RX_BUF[3] ) ? USART_RX_BUF[3] : USART_RX_BUF[2] ;	
	maxaddr = (USART_RX_BUF[2] >= USART_RX_BUF[3] ) ? USART_RX_BUF[2] : USART_RX_BUF[3] ;
	if(modbus.address < minaddr || modbus.address > maxaddr)
		return 0;
	else
	{	// in the TRUE case, we add a random delay such that the Interface can pick up the packets
		srand(srand_count);
		variable_delay = rand() % 20;
		for ( i=0; i<variable_delay; i++)
			delay_us(100);
		return 1;
	}

	}
	// ------------------------------------------------------------------------------------------------------



	// check that message is one of the following
	if( (USART_RX_BUF[1]!=READ_VARIABLES) && (USART_RX_BUF[1]!=WRITE_VARIABLES) && (USART_RX_BUF[1]!=MULTIPLE_WRITE) )
		return 0;
	// ------------------------------------------------------------------------------------------------------
		// ------------------------------------------------------------------------------------------------------
		
	if(USART_RX_BUF[2]*256 + USART_RX_BUF[3] ==  MODBUS_ADDRESS_PLUG_N_PLAY)
	{
		if(USART_RX_BUF[1] == WRITE_VARIABLES)
		{
			if(USART_RX_BUF[6] != modbus.serial_Num[0]) 
			return FALSE;
			if(USART_RX_BUF[7] != modbus.serial_Num[1]) 
			return FALSE;
			if(USART_RX_BUF[8] != modbus.serial_Num[2])  
			return FALSE;
			if(USART_RX_BUF[9] != modbus.serial_Num[3]) 
			return FALSE;
		}
		if (USART_RX_BUF[1] == READ_VARIABLES)
		{
			randval = rand() % 5 ;
		}
		if(randval != RESPONSERANDVALUE)
		{

				return FALSE;
		}
		else
		{	// in the TRUE case, we add a random delay such that the Interface can pick up the packets
			variable_delay = rand() % 20;
			for ( i=0; i<variable_delay; i++)
				delay_us(100);
		}
		
	}

	// if trying to write the Serial number, first check to see if it has been already written
	// note this does not take count of multiple-write, thus if try to write into those reg with multiple-write, command will accept
	if( (USART_RX_BUF[1]==WRITE_VARIABLES)  && (address<= MODBUS_HARDWARE_REV) )
	{
		// Return false if trying to write SN Low word that has already been written
		if(USART_RX_BUF[3] < 2)
		{
			if(modbus.SNWriteflag & 0x01)                // low byte of SN writed
				return FALSE;
		}
		// Return false if trying to write SN High word that has already been written
		else if (USART_RX_BUF[3] < 4)
		{
			if(modbus.SNWriteflag  & 0x02)                 // high byte of SN writed
				return FALSE;
		}
		else if (USART_RX_BUF[3] ==  MODBUS_HARDWARE_REV)
		{
			if(modbus.SNWriteflag  & 0x04)                 // hardware byte writed
				return FALSE;
		}

	}


	crc_val = crc16(USART_RX_BUF, rece_size-2);

	if(crc_val == (USART_RX_BUF[rece_size-2]<<8) + USART_RX_BUF[rece_size-1] )
	{
		return 1;
	}
	else
	{
		return 0;
	}
	//return TRUE;

 }

 
 
																				//Calls the functions to initialize transmission and CRC and also to respond
 void dealwithData(void)								// to the requests and store the data
{	
	u16 address;
	// given this is used in multiple places, decided to put it as an argument
	address = (u16)(USART_RX_BUF[2]<<8) + USART_RX_BUF[3];
	if (checkData(address))
	{		
//		// Initialize tranmission
		initSend_COM();	
		// Initialize CRC
		init_crc16();		

//		// Respond with any data requested
		responseCmd(0,USART_RX_BUF);
//		// Store any data being written
		internalDeal(0, USART_RX_BUF);

	}
	else
	{
		serial_restart();
	}
}


void modbus_data_cope(u8 XDATA* pData, u16 length, u8 conn_id)
{
	
}

void SoftReset(void)										//Closes all interrupts and resets
{
	__set_FAULTMASK(1);      // 关闭所有中断
	NVIC_SystemReset();      // 复位
}
