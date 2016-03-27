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
#include <udp_scan.h>

u8 address_temp ;
u8 div_buf ;
u8 address_buf ;
u8 i ;

void multiple_write_modbus(u16 StartAdd,u8 *pData,u8 HeadLen){
		
	if(StartAdd == MODBUS_MAC_ADDRESS_1)
		{
			if((modbus.mac_enable == 1) && (pData[HeadLen + 6] == 12))						 	//Forming the MAC address
			{
				modbus.mac_addr[0] = pData[HeadLen + 8];
				modbus.mac_addr[1] = pData[HeadLen + 10];
				modbus.mac_addr[2] = pData[HeadLen + 12];
				modbus.mac_addr[3] = pData[HeadLen + 14];
				modbus.mac_addr[4] = pData[HeadLen + 16];
				modbus.mac_addr[5] = pData[HeadLen + 18];
				for(i=0; i<6; i++)
						{
							AT24CXX_WriteOneByte(EEP_MAC_ADDRESS_1+i, modbus.mac_addr[i]);	// Saving the MAC address on EEPROM [L]
						}
//				while(tapdev_init())	//初始化ENC28J60错误
//				{								   
//				//	printf("ENC28J60 Init Error!\r\n");
//				delay_ms(50);
//				};
				IP_Change = 1 ;						// ???? what is the use of this flag ????
				modbus.mac_enable = 0 ;		// what is the use of this flag ???? [L]
			}
		}
		#ifdef T3PT12			// It is one of the slave modbus devices @#@
		else if((StartAdd >= MODBUS_TEMP0_HI)&&(StartAdd <= MODBUS_TEMP11_LO))
		{
			_TEMP_VALUE_  _offset_ra[12] ;					
			address_temp = (StartAdd-MODBUS_TEMP0_HI)/2 ;
			_offset_ra[address_temp].qauter_temp_C[0] = pData[HeadLen+9] ;
			_offset_ra[address_temp].qauter_temp_C[1] = pData[HeadLen+10] ;
			_offset_ra[address_temp].qauter_temp_C[2] = pData[HeadLen+7] ;
			_offset_ra[address_temp].qauter_temp_C[3] = pData[HeadLen+8] ;
//			printf("%f\n\r", _offset_ra[address_temp].temp_C);
			_offset_rc[(StartAdd-MODBUS_TEMP0_HI)/2].temp_C = _offset_ra[(StartAdd-MODBUS_TEMP0_HI)/2].temp_C - _temp_offset_value[(StartAdd - MODBUS_TEMP0_HI) / 2].temp_C+100;
//			printf("%f\n\r", _offset_rc[address_temp].temp_C);
//			AT24CXX_WriteOneByte(EEP_DEFAULT_TEMP_OFFSET0_HI+4*address_temp, _offset_rc[address_temp].qauter_temp_C[0]);
//			AT24CXX_WriteOneByte(EEP_DEFAULT_TEMP_OFFSET0_HI+4*address_temp+1, _offset_rc[address_temp].qauter_temp_C[1]);
//			AT24CXX_WriteOneByte(EEP_DEFAULT_TEMP_OFFSET0_LO+4*address_temp, _offset_rc[address_temp].qauter_temp_C[2]);
//			AT24CXX_WriteOneByte(EEP_DEFAULT_TEMP_OFFSET0_LO+4*address_temp+1, _offset_rc[address_temp].qauter_temp_C[3]);	

		
		
		}
		#endif
	}


// Register write function
void write_register_modbus(u16 StartAdd,u8 *pData,u8 HeadLen){

		if(StartAdd  <= 99 )
				{								
					// If writing to Serial number Low word, set the Serial number Low flag
					if(StartAdd <= MODBUS_SERIALNUMBER_LOWORD+1)
					{
						AT24CXX_WriteOneByte((u16)EEP_SERIALNUMBER_LOWORD, pData[HeadLen+5]);
						AT24CXX_WriteOneByte((u16)EEP_SERIALNUMBER_LOWORD+1, pData[HeadLen+4]);
						modbus.serial_Num[0] = pData[HeadLen+5] ;
						modbus.serial_Num[1] = pData[HeadLen+4] ;
						modbus.SNWriteflag |= 0x01;
						AT24CXX_WriteOneByte((u16)EEP_SERIALNUMBER_WRITE_FLAG, modbus.SNWriteflag);
						
						if(modbus.SNWriteflag)
						{
							modbus.update = 0;
							AT24CXX_WriteOneByte((u16)EEP_UPDATE_STATUS, 0);
						}
					}
					// If writing to Serial number High word, set the Serial number High flag
					else if(StartAdd <= MODBUS_SERIALNUMBER_HIWORD+1)
					{
						
						AT24CXX_WriteOneByte((u16)EEP_SERIALNUMBER_HIWORD, pData[HeadLen+5]);
						AT24CXX_WriteOneByte((u16)EEP_SERIALNUMBER_HIWORD+1, pData[HeadLen+4]);
						modbus.serial_Num[2] = pData[HeadLen+5] ;
						modbus.serial_Num[3] = pData[HeadLen+4] ;
						modbus.SNWriteflag |= 0x02;
						AT24CXX_WriteOneByte((u16)EEP_SERIALNUMBER_WRITE_FLAG, modbus.SNWriteflag);
						
						if(modbus.SNWriteflag)
						{
							modbus.update = 0;
							AT24CXX_WriteOneByte((u16)EEP_UPDATE_STATUS, 0);
						}
					}
		//			else if(USART_RX_BUF[3] <= MODBUS_VERSION_NUMBER_LO+1)
		//			{	
		//				AT24CXX_WriteOneByte((u16)EEP_VERSION_NUMBER_LO, USART_RX_BUF[5]);
		//				AT24CXX_WriteOneByte((u16)EEP_VERSION_NUMBER_LO+1, USART_RX_BUF[4]);
		//				modbus.software = (USART_RX_BUF[5]<<8) ;
		//				modbus.software |= USART_RX_BUF[4] ;				
		//			}
					else if(StartAdd == MODBUS_ADDRESS )					//Write the modbus address in the memory @#@
					{
						AT24CXX_WriteOneByte((u16)EEP_ADDRESS, pData[HeadLen+5]);
						modbus.address	= pData[HeadLen+5] ;
					}
					else if(StartAdd == MODBUS_PRODUCT_MODEL )		//Used to write the product model @#@
					{
						AT24CXX_WriteOneByte((u16)EEP_PRODUCT_MODEL, pData[HeadLen+5]);
						modbus.product	= pData[HeadLen+5] ;
						modbus.SNWriteflag |= 0x08;
						AT24CXX_WriteOneByte((u16)EEP_SERIALNUMBER_WRITE_FLAG, modbus.SNWriteflag);
					}
					else if(StartAdd == MODBUS_HARDWARE_REV )			//Used for the hardware revision info @#@
					{
						AT24CXX_WriteOneByte((u16)EEP_HARDWARE_REV, pData[HeadLen+5]);
						modbus.hardware_Rev	= pData[HeadLen+5] ;
						modbus.SNWriteflag |= 0x04;
						AT24CXX_WriteOneByte((u16)EEP_SERIALNUMBER_WRITE_FLAG, modbus.SNWriteflag);
					}
					else if(StartAdd == MODBUS_BAUDRATE )			//  baud rates info
					{			
						modbus.baud = pData[HeadLen+5] ;
						switch(modbus.baud)
						{
							case 0:
								modbus.baudrate = BAUDRATE_9600 ;
								uart1_init(BAUDRATE_9600);
								AT24CXX_WriteOneByte(EEP_BAUDRATE, pData[HeadLen+5]);					
								SERIAL_RECEIVE_TIMEOUT = 6;
							break ;
							case 1:
								modbus.baudrate = BAUDRATE_19200 ;
								uart1_init(BAUDRATE_19200);
								AT24CXX_WriteOneByte(EEP_BAUDRATE, pData[HeadLen+5]);	
								SERIAL_RECEIVE_TIMEOUT = 3;
							break;
							case 2:
								modbus.baudrate = BAUDRATE_38400 ;
								uart1_init(BAUDRATE_38400);
								AT24CXX_WriteOneByte(EEP_BAUDRATE, pData[HeadLen+5]);	
								SERIAL_RECEIVE_TIMEOUT = 2;
							break;
							case 3:
								modbus.baudrate = BAUDRATE_57600 ;
								uart1_init(BAUDRATE_57600);
								AT24CXX_WriteOneByte(EEP_BAUDRATE, pData[HeadLen+5]);	
								SERIAL_RECEIVE_TIMEOUT = 1;
							break;
							case 4:
								modbus.baudrate = BAUDRATE_115200 ;
								uart1_init(BAUDRATE_115200);
								AT24CXX_WriteOneByte(EEP_BAUDRATE, pData[HeadLen+5]);	
								SERIAL_RECEIVE_TIMEOUT = 1;		
							default:
							break ;				
						}
						modbus_init();
					}
					else if(StartAdd == MODBUS_UPDATE_STATUS )			// july 21 Ron
					{
						//AT24CXX_WriteOneByte(EEP_UPDATE_STATUS, pData[HeadLen+5]);
						modbus.update = pData[HeadLen+5] ;
					}
					else if(StartAdd == MODBUS_PROTOCOL_TYPE )			// july 21 Ron
					{
						if((pData[HeadLen+5] == MODBUS)||(pData[HeadLen+5] == BAC_MSTP))
						{
							AT24CXX_WriteOneByte(EEP_MODBUS_COM_CONFIG, pData[HeadLen+5]);
								modbus.protocal = pData[HeadLen+5] ;
						}
					}
					else if(( StartAdd >= MODBUS_MAC_ADDRESS_1 )&&( StartAdd <= MODBUS_MAC_ADDRESS_6 ))
					{
							address_temp	= StartAdd - MODBUS_MAC_ADDRESS_1 ;
							modbus.mac_addr[address_temp] = pData[HeadLen+5] ;
							AT24CXX_WriteOneByte(EEP_MAC_ADDRESS_1+address_temp, pData[HeadLen+5]);
					}
					
// ----------------Used for ghost IP mode-------------------------//
					else if(StartAdd == MODBUS_GHOST_IP_MODE )
					{
						modbus.ghost_ip_mode = pData[HeadLen+5] ;
					}
					else if(( StartAdd >= MODBUS_GHOST_IP_ADDRESS_1 )&&( StartAdd <= MODBUS_GHOST_IP_ADDRESS_4 ))
					{
							address_temp	= StartAdd - MODBUS_GHOST_IP_ADDRESS_1 ;
							modbus.ghost_ip_addr[address_temp] = pData[HeadLen+5] ;
					}
					else if((StartAdd >= MODBUS_GHOST_SUB_MASK_ADDRESS_1 )&&( StartAdd <= MODBUS_GHOST_SUB_MASK_ADDRESS_4 ))
					{
							address_temp	= StartAdd - MODBUS_GHOST_SUB_MASK_ADDRESS_1 ;
							modbus.ghost_mask_addr[address_temp] = pData[HeadLen+5] ;
					}
					else if(( StartAdd >= MODBUS_GHOST_GATEWAY_ADDRESS_1 )&&( StartAdd <= MODBUS_GHOST_GATEWAY_ADDRESS_4 ))
					{
							address_temp	= StartAdd - MODBUS_GHOST_GATEWAY_ADDRESS_1 ;
							modbus.ghost_gate_addr[address_temp] = pData[HeadLen+5] ;
					}
					else if(StartAdd == MODBUS_GHOST_TCP_SERVER )
					{
						modbus.ghost_tcp_server = pData[HeadLen+5] ;
					}
					else if(StartAdd == MODBUS_GHOST_LISTEN_PORT )
					{
						modbus.ghost_listen_port =  (pData[HeadLen+4]<<8) +pData[HeadLen+5] ;
					}
					else if(StartAdd == MODBUS_WRITE_GHOST_SYSTEM )
					{
						modbus.write_ghost_system = pData[HeadLen+5] ;
						if(modbus.write_ghost_system == 1)
						{
								modbus.ip_mode = modbus.ghost_ip_mode ;
								modbus.tcp_server = modbus.ghost_tcp_server ;
								modbus.listen_port = modbus.ghost_listen_port ;
								AT24CXX_WriteOneByte(EEP_IP_MODE, modbus.ip_mode);
								AT24CXX_WriteOneByte(EEP_TCP_SERVER, modbus.tcp_server);				
								AT24CXX_WriteOneByte(EEP_LISTEN_PORT_HI, modbus.listen_port>>8);
								AT24CXX_WriteOneByte(EEP_LISTEN_PORT_LO, modbus.listen_port &0xff);
								for(i=0; i<4; i++)
								{
									modbus.ip_addr[i] = modbus.ghost_ip_addr[i] ;
									modbus.mask_addr[i] = modbus.ghost_mask_addr[i] ;
									modbus.gate_addr[i] = modbus.ghost_gate_addr[i] ;
									
									AT24CXX_WriteOneByte(EEP_IP_ADDRESS_1+i, modbus.ip_addr[i]);
									AT24CXX_WriteOneByte(EEP_SUB_MASK_ADDRESS_1+i, modbus.mask_addr[i]);
									AT24CXX_WriteOneByte(EEP_GATEWAY_ADDRESS_1+i, modbus.gate_addr[i]);						
								}
		//						for(i=0; i<5; i++)
		//						{
		//							AT24CXX_WriteOneByte(EEP_MAC_ADDRESS_1+i, modbus.mac_addr[i]);
		//						}
		//						if(!tapdev_init()) 
		//						{
		//							printf("Init fail\n\r");
		//						}
		//							while(tapdev_init())	//初始化ENC28J60错误
		//							{								   
		//							//	printf("ENC28J60 Init Error!\r\n");
		//							delay_ms(50);
		//							};	
									IP_Change = 1; 
								modbus.write_ghost_system = 0 ;
						}
					}
					else if(StartAdd == MODBUS_MAC_ENABLE )
					{
						modbus.mac_enable = pData[HeadLen+5] ;	
					}

				}
					#ifdef T38AI8AO6DO	//Another one of the modbus slave devices @#@
					else if(( StartAdd >= MODBUS_AO_CHANNLE0 )&&( StartAdd <= MODBUS_AO_CHANNLE7 ))
					{
							address_temp	= StartAdd - MODBUS_AO_CHANNLE0 ;
							modbus.analog_output[address_temp] = (pData[HeadLen+4]<<8)|pData[HeadLen+5] ; 
							if(modbus.analog_output[address_temp] >1000) modbus.analog_output[address_temp] = 1000 ;
							AT24CXX_WriteOneByte(EEP_AO_CHANNLE0+address_temp*2, pData[HeadLen+5]);
							AT24CXX_WriteOneByte(EEP_AO_CHANNLE0+address_temp*2+1, pData[HeadLen+4]);
					}
					else if(( StartAdd >= MODBUS_DO_CHANNLE0 )&&( StartAdd <= MODBUS_DO_CHANNLE5))
					{
							address_temp	= StartAdd - MODBUS_DO_CHANNLE0 ;
							if(pData[HeadLen+5]>1) pData[HeadLen+5] = 1 ;
							modbus.digit_output[address_temp] = pData[HeadLen+5] ; 
							AT24CXX_WriteOneByte(EEP_DO_CHANNLE0+address_temp, modbus.digit_output[address_temp]);
					}
					else if(( StartAdd >= MODBUS_AI_FILTER0 )&&( StartAdd <= MODBUS_AI_FILTER7))
					{
							address_temp	= StartAdd - MODBUS_AI_FILTER0 ;
							modbus.filter_value[address_temp] = pData[HeadLen+5] ; 
							AT24CXX_WriteOneByte(EEP_AI_FILTER0+address_temp, modbus.filter_value[address_temp]);
					}
		//			else if(( StartAdd >= MODBUS_AI_CHANNLE0 )&&( StartAdd <= MODBUS_AI_CHANNLE7_LO ))
		//			{
		//				u16 modbus_temp ;
		//				modbus_temp = 	(pData[HeadLen+4]<<8)|pData[HeadLen+5] ;
		//				address_temp	= StartAdd - MODBUS_AI_CHANNLE0 ;
		//				modbus.offset[address_temp] += (s16)modbus.input[address_temp] - modbus_temp ;			
		//			}
					
					else if(( StartAdd >= MODBUS_AI_CHANNLE0_HI )&&( StartAdd <= MODBUS_AI_CHANNLE7_LO ))
					{
						u8 div_temp ;
						u16 modbus_temp ;
						address_temp	= StartAdd - MODBUS_AI_CHANNLE0_HI ;
						div_temp =  address_temp /2 ;
						
							if(modbus.range[div_temp] == PULSE)
							{
									modbus.pulse[div_temp].word = 0 ;
									AT24CXX_WriteOneByte(EEP_PLUSE0_HI_HI+4*div_temp, modbus.pulse[div_temp].quarter[0]);
									AT24CXX_WriteOneByte(EEP_PLUSE0_HI_LO+4*div_temp, modbus.pulse[div_temp].quarter[1]);
									AT24CXX_WriteOneByte(EEP_PLUSE0_LO_HI+4*div_temp, modbus.pulse[div_temp].quarter[2]);
									AT24CXX_WriteOneByte(EEP_PLUSE0_LO_LO+4*div_temp, modbus.pulse[div_temp].quarter[3]);
							}
						if(address_temp%2 == 1 )
						{
							if(modbus.range[div_temp] != PULSE)
							{
								modbus_temp = 	(pData[HeadLen+4]<<8)|pData[HeadLen+5] ;					
								modbus.offset[div_temp] += (s16)modbus.input[div_temp] - modbus_temp ;
							}
						}
					}
					else if(( StartAdd >= MODBUS_AI_RANGE0 )&&( StartAdd <= MODBUS_AI_RANGE7))
					{
							address_temp	= StartAdd - MODBUS_AI_RANGE0 ;
							modbus.range[address_temp] = pData[HeadLen+5] ; 
							AT24CXX_WriteOneByte(EEP_AI_RANGE0+ address_temp, modbus.range[address_temp]);
							modbus.offset[address_temp] = 500 ;
							AT24CXX_WriteOneByte(EEP_AI_OFFSET0+ address_temp*2, (modbus.offset[address_temp]>>8)&0xff );
							AT24CXX_WriteOneByte(EEP_AI_OFFSET0+ address_temp*2+1, modbus.offset[address_temp]&0xff);
					}
					else if(( StartAdd >= MODBUS_AI_OFFSET0 )&&( StartAdd <= MODBUS_AI_OFFSET7 ))
					{
							address_temp	= StartAdd - MODBUS_AI_OFFSET0 ;
							modbus.offset[address_temp] = (pData[HeadLen+4]<<8)|pData[HeadLen+5] ;
							AT24CXX_WriteOneByte(EEP_AI_OFFSET0+ address_temp*2, pData[4]);
							AT24CXX_WriteOneByte(EEP_AI_OFFSET0+ address_temp*2+1, pData[5]);
					}
					
					/*else if(( StartAdd >= MODBUS_AI0_CUSTOMER_HI )&&( StartAdd <= MODBUS_AI7_CUSTOMER_LO ))
					{
							address_temp	= StartAdd - MODBUS_AI0_CUSTOMER_HI ;
							div_buf =  address_temp%2  ;
							address_buf = address_temp/2 ;
							if(div_buf == 0)
							{
								modbus.customer_range_hi[address_buf] = (pData[HeadLen+4]<<8)|pData[HeadLen+5] ;
								AT24CXX_WriteOneByte(EEP_CUSTOMER_REANGE0_HI+ address_buf*4, pData[HeadLen+4]);
								AT24CXX_WriteOneByte(EEP_CUSTOMER_REANGE0_HI+ address_buf*4+1, pData[HeadLen+5]);
							}
							else
							{
								modbus.customer_range_lo[address_buf] = (pData[HeadLen+4]<<8)|pData[HeadLen+5] ;
								AT24CXX_WriteOneByte(EEP_CUSTOMER_REANGE0_LO+ address_buf*4, pData[HeadLen+4]);
								AT24CXX_WriteOneByte(EEP_CUSTOMER_REANGE0_LO+ address_buf*4+1, pData[HeadLen+5]);	
							}
					}
					else if(( StartAdd >= MODBUS_CUSTOMER_CHANNEL0_ENABLE )&&( StartAdd <= MODBUS_CUSTOMER_CHANNEL7_ENABLE ))
					{
							address_temp	= StartAdd - MODBUS_CUSTOMER_CHANNEL0_ENABLE ;
							modbus.customer_enable[address_temp] = pData[HeadLen+5] ;				
							AT24CXX_WriteOneByte(EEP_CUSTOMER_RANGE0_ENABLE+ address_temp, modbus.customer_enable[address_temp]);

					}*/
					#endif
					
					#ifdef T322AI
					else if(( StartAdd >= MODBUS_AI_CHANNLE0_HI )&&( StartAdd <= MODBUS_AI_CHANNLE21_LO ))
					{
						u8 div_temp ;
						u16 modbus_temp ;
						address_temp	= StartAdd - MODBUS_AI_CHANNLE0_HI ;
						div_temp =  address_temp /2 ;
						
							if(modbus.range[div_temp] == HI_spd_count)
							{
									modbus.pulse[div_temp].word = 0 ;
									AT24CXX_WriteOneByte(EEP_PLUSE0_HI_HI+4*div_temp, modbus.pulse[div_temp].quarter[0]);
									AT24CXX_WriteOneByte(EEP_PLUSE0_HI_LO+4*div_temp, modbus.pulse[div_temp].quarter[1]);
									AT24CXX_WriteOneByte(EEP_PLUSE0_LO_HI+4*div_temp, modbus.pulse[div_temp].quarter[2]);
									AT24CXX_WriteOneByte(EEP_PLUSE0_LO_LO+4*div_temp, modbus.pulse[div_temp].quarter[3]);
							}
						if(address_temp%2 == 1 )
						{
							if(modbus.range[div_temp] != HI_spd_count)
							{
								modbus_temp = 	(pData[HeadLen+4]<<8)|pData[HeadLen+5] ;					
								modbus.offset[div_temp] += (s16)modbus.input[div_temp] - modbus_temp ;
							}
						}
					}
		//			else if(( StartAdd >= MODBUS_AI_CHANNLE11 )&&( StartAdd <= MODBUS_AI_CHANNLE21 ))
		//			{
		//				u16 modbus_temp ;
		//				modbus_temp = 	(pData[HeadLen+4]<<8)|pData[HeadLen+5] ;
		//				address_temp	= StartAdd - MODBUS_AI_CHANNLE11+11 ;
		//				modbus.offset[address_temp] += (s16)modbus.input[address_temp] - modbus_temp ;
		////				div_temp =  address_temp /2 ;
		////					if(modbus.range[div_temp] == PULSE)
		////					{
		////							modbus.pulse[div_temp].word = 0 ;
		////							AT24CXX_WriteOneByte(EEP_PLUSE0_HI_HI+4*div_temp, modbus.pulse[div_temp].quarter[0]);
		////							AT24CXX_WriteOneByte(EEP_PLUSE0_HI_LO+4*div_temp, modbus.pulse[div_temp].quarter[1]);
		////							AT24CXX_WriteOneByte(EEP_PLUSE0_LO_HI+4*div_temp, modbus.pulse[div_temp].quarter[2]);
		////							AT24CXX_WriteOneByte(EEP_PLUSE0_LO_LO+4*div_temp, modbus.pulse[div_temp].quarter[3]);
		////					}			
		//			}
					else if(( StartAdd >= MODBUS_AI_FILTER0 )&&( StartAdd <= MODBUS_AI_FILTER21 ))
					{
							address_temp	= StartAdd - MODBUS_AI_FILTER0 ;
							modbus.filter_value[address_temp] = pData[HeadLen+5] ;
							AT24CXX_WriteOneByte(EEP_AI_FILTER0+address_temp, pData[HeadLen+5]);
					}
					else if(( StartAdd >= MODBUS_AI_RANGE0 )&&( StartAdd <= MODBUS_AI_RANGE21 ))
					{
							address_temp	= StartAdd - MODBUS_AI_RANGE0 ;
							modbus.range[address_temp] = pData[HeadLen+5] ;
							AT24CXX_WriteOneByte(EEP_AI_RANGE0+ address_temp, modbus.range[address_temp]);
							
							modbus.offset[address_temp] = 500 ;				
							AT24CXX_WriteOneByte(EEP_AI_OFFSET0+ address_temp*2, (modbus.offset[address_temp]>>8)&0xff);
							AT24CXX_WriteOneByte(EEP_AI_OFFSET0+ address_temp*2+1, modbus.offset[address_temp]&0xff);
						
						
							if((modbus.range[address_temp] == HI_spd_count)&&(address_temp<11))
							{
										pulse_set(address_temp);

							}
					}
					else if(( StartAdd >= MODBUS_AI_OFFSET0 )&&( StartAdd <= MODBUS_AI_OFFSET21 ))
					{
							address_temp	= StartAdd - MODBUS_AI_OFFSET0 ;
							modbus.offset[address_temp] = (pData[HeadLen+4]<<8)|pData[HeadLen+5] ;
						
							AT24CXX_WriteOneByte(EEP_AI_OFFSET0+ address_temp*2, pData[HeadLen+4]);
							AT24CXX_WriteOneByte(EEP_AI_OFFSET0+ address_temp*2+1, pData[HeadLen+5]);
					}
					
		//			else if(( StartAdd >= MODBUS_AI0_CUSTOMER_HI )&&( StartAdd <= MODBUS_AI21_CUSTOMER_LO ))
		//			{
		//					address_temp	= StartAdd - MODBUS_AI0_CUSTOMER_HI ;
		//					div_buf =  address_temp%2  ;
		//					address_buf = address_temp/2 ;
		//					if(div_buf == 0)
		//					{
		//						modbus.customer_range_hi[address_buf] = (pData[HeadLen+4]<<8)|pData[HeadLen+5] ;
		//						AT24CXX_WriteOneByte(EEP_CUSTOMER_REANGE0_HI+ address_buf*4, pData[HeadLen+4]);
		//						AT24CXX_WriteOneByte(EEP_CUSTOMER_REANGE0_HI+ address_buf*4+1, pData[HeadLen+5]);
		//					}
		//					else
		//					{
		//						modbus.customer_range_lo[address_buf] = (pData[HeadLen+4]<<8)|pData[HeadLen+5] ;
		//						AT24CXX_WriteOneByte(EEP_CUSTOMER_REANGE0_LO+ address_buf*4, pData[HeadLen+4]);
		//						AT24CXX_WriteOneByte(EEP_CUSTOMER_REANGE0_LO+ address_buf*4+1, pData[HeadLen+5]);	
		//					}
		//			}
		//			else if(( StartAdd >= MODBUS_CUSTOMER_CHANNEL0_ENABLE )&&( StartAdd <= MODBUS_CUSTOMER_CHANNEL21_ENABLE ))
		//			{
		//					address_temp	= StartAdd - MODBUS_CUSTOMER_CHANNEL0_ENABLE ;
		//					modbus.customer_enable[address_temp] = pData[HeadLen+5] ;				
		//					AT24CXX_WriteOneByte(EEP_CUSTOMER_RANGE0_ENABLE+ address_temp, modbus.customer_enable[address_temp]);

		//			}
					#endif	
					#ifndef T3PT12
					else if(( StartAdd >= MODBUS_CUSTOMER_TABLE1_VOL_0 )&&( StartAdd <= MODBUS_CUSTOMER_TABLE1_VAL_10 ))
					{
							address_temp	= StartAdd - MODBUS_CUSTOMER_TABLE1_VOL_0 ;
							div_buf =  address_temp%2  ;
							address_buf = address_temp/2 ;
							if(div_buf == 0)
							{
								modbus.customer_table1_vol[address_buf] = (pData[HeadLen+4]<<8)|pData[HeadLen+5] ;
								AT24CXX_WriteOneByte(EEP_CUSTOMER_TABLE1_VOL_HI_0+ address_buf*4, pData[HeadLen+4]);
								AT24CXX_WriteOneByte(EEP_CUSTOMER_TABLE1_VOL_HI_0+ address_buf*4+1, pData[HeadLen+5]);
							}
							else
							{
								modbus.customer_table1_val[address_buf] = (pData[HeadLen+4]<<8)|pData[HeadLen+5] ;
								AT24CXX_WriteOneByte(EEP_CUSTOMER_TABLE1_VOL_HI_0+ address_buf*4+2, pData[HeadLen+4]);
								AT24CXX_WriteOneByte(EEP_CUSTOMER_TABLE1_VOL_HI_0+ address_buf*4+3, pData[HeadLen+5]);	
							}			
					}
					else if(( StartAdd >= MODBUS_CUSTOMER_TABLE2_VOL_0 )&&( StartAdd <= MODBUS_CUSTOMER_TABLE2_VAL_10 ))
					{
							address_temp	= StartAdd - MODBUS_CUSTOMER_TABLE2_VOL_0 ;
							div_buf =  address_temp%2  ;
							address_buf = address_temp/2 ;
							if(div_buf == 0)
							{
								modbus.customer_table2_vol[address_buf] = (pData[HeadLen+4]<<8)|pData[HeadLen+5] ;
								AT24CXX_WriteOneByte(EEP_CUSTOMER_TABLE2_VOL_HI_0+ address_buf*4, pData[HeadLen+4]);
								AT24CXX_WriteOneByte(EEP_CUSTOMER_TABLE2_VOL_HI_0+ address_buf*4+1, pData[HeadLen+5]);
							}
							else
							{
								modbus.customer_table2_val[address_buf] = (pData[HeadLen+4]<<8)|pData[HeadLen+5] ;
								AT24CXX_WriteOneByte(EEP_CUSTOMER_TABLE2_VOL_HI_0+ address_buf*4+2, pData[HeadLen+4]);
								AT24CXX_WriteOneByte(EEP_CUSTOMER_TABLE2_VOL_HI_0+ address_buf*4+3, pData[HeadLen+5]);	
							}			
					}
					else if(( StartAdd >= MODBUS_CUSTOMER_TABLE3_VOL_0 )&&( StartAdd <= MODBUS_CUSTOMER_TABLE3_VAL_10 ))
					{
							address_temp	= StartAdd - MODBUS_CUSTOMER_TABLE3_VOL_0 ;
							div_buf =  address_temp%2  ;
							address_buf = address_temp/2 ;
							if(div_buf == 0)
							{
								modbus.customer_table3_vol[address_buf] = (pData[HeadLen+4]<<8)|pData[HeadLen+5] ;
								AT24CXX_WriteOneByte(EEP_CUSTOMER_TABLE3_VOL_HI_0+ address_buf*4, pData[HeadLen+4]);
								AT24CXX_WriteOneByte(EEP_CUSTOMER_TABLE3_VOL_HI_0+ address_buf*4+1, pData[HeadLen+5]);
							}
							else
							{
								modbus.customer_table3_val[address_buf] = (pData[HeadLen+4]<<8)|pData[HeadLen+5] ;
								AT24CXX_WriteOneByte(EEP_CUSTOMER_TABLE3_VOL_HI_0+ address_buf*4+2, pData[HeadLen+4]);
								AT24CXX_WriteOneByte(EEP_CUSTOMER_TABLE3_VOL_HI_0+ address_buf*4+3, pData[HeadLen+5]);	
							}			
					}
					else if(( StartAdd >= MODBUS_CUSTOMER_TABLE4_VOL_0 )&&( StartAdd <= MODBUS_CUSTOMER_TABLE4_VAL_10 ))
					{
							address_temp	= StartAdd - MODBUS_CUSTOMER_TABLE4_VOL_0 ;
							div_buf =  address_temp%2  ;
							address_buf = address_temp/2 ;
							if(div_buf == 0)
							{
								modbus.customer_table3_vol[address_buf] = (pData[HeadLen+4]<<8)|pData[HeadLen+5] ;
								AT24CXX_WriteOneByte(EEP_CUSTOMER_TABLE3_VOL_HI_0+ address_buf*4, pData[HeadLen+4]);
								AT24CXX_WriteOneByte(EEP_CUSTOMER_TABLE3_VOL_HI_0+ address_buf*4+1, pData[HeadLen+5]);
							}
							else
							{
								modbus.customer_table3_val[address_buf] = (pData[HeadLen+4]<<8)|pData[HeadLen+5] ;
								AT24CXX_WriteOneByte(EEP_CUSTOMER_TABLE3_VOL_HI_0+ address_buf*4+2, pData[HeadLen+4]);
								AT24CXX_WriteOneByte(EEP_CUSTOMER_TABLE3_VOL_HI_0+ address_buf*4+3, pData[HeadLen+5]);	
							}			
					}
					else if(( StartAdd >= MODBUS_CUSTOMER_TABLE3_VOL_0 )&&( StartAdd <= MODBUS_CUSTOMER_TABLE3_VAL_10 ))
					{
							address_temp	= StartAdd - MODBUS_CUSTOMER_TABLE3_VOL_0 ;
							div_buf =  address_temp%2  ;
							address_buf = address_temp/2 ;
							if(div_buf == 0)
							{
								modbus.customer_table3_vol[address_buf] = (pData[HeadLen+4]<<8)|pData[HeadLen+5] ;
								AT24CXX_WriteOneByte(EEP_CUSTOMER_TABLE3_VOL_HI_0+ address_buf*4, pData[HeadLen+4]);
								AT24CXX_WriteOneByte(EEP_CUSTOMER_TABLE3_VOL_HI_0+ address_buf*4+1, pData[HeadLen+5]);
							}
							else
							{
								modbus.customer_table3_val[address_buf] = (pData[HeadLen+4]<<8)|pData[HeadLen+5] ;
								AT24CXX_WriteOneByte(EEP_CUSTOMER_TABLE3_VOL_HI_0+ address_buf*4+2, pData[HeadLen+4]);
								AT24CXX_WriteOneByte(EEP_CUSTOMER_TABLE3_VOL_HI_0+ address_buf*4+3, pData[HeadLen+5]);	
							}			
					}
					#endif
					
					#ifdef T3PT12
					else if(( StartAdd >= MODBUS_CHANNEL0_FILTER )&&( StartAdd <= MODBUS_CHANNEL11_FILTER ))
					{
							address_temp	= StartAdd - MODBUS_CHANNEL0_FILTER ;
							modbus.filter_value[address_temp] = pData[HeadLen+5] ;
							AT24CXX_WriteOneByte(EEP_CHANNEL0_FLTER+address_temp, pData[HeadLen+5]);
					}
					else if(( StartAdd >= MODBUS_CHANNEL0_RANGE )&&( StartAdd <= MODBUS_CHANNEL11_RANGE ))
					{
							address_temp	= StartAdd - MODBUS_CHANNEL0_RANGE ;
							modbus.range[address_temp] = pData[HeadLen+5] ;
							AT24CXX_WriteOneByte(EEP_CHANNEL0_RANGE+address_temp, pData[HeadLen+5]);
					}
					else if( StartAdd == MODBUS_RESOLE_BIT )
					{
						if(pData[HeadLen+5]<=4)  modbus.resole_bit = pData[HeadLen+5] ;
						AT24CXX_WriteOneByte(EEP_RESOLE_BIT, modbus.resole_bit);
					}
					#endif
					else if(StartAdd == MODBUS_RESET)
					{ 				
						
					
						 modbus.reset = pData[HeadLen+5] ;
						if(modbus.reset == 1)
						{
							for(i=0; i<255; i++)
							AT24CXX_WriteOneByte(i, 0xff);
						}				
						EEP_Dat_Init();
		//				AT24CXX_WriteOneByte(16, 1);
						SoftReset();
					}
}
