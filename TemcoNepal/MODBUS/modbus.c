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
u8 USART_RX_BUF[USART_REC_LEN];     //USART_REC_LEN is the length of data to be received @#@
u8 uart_send[USART_SEND_LEN] ;			//USART_SEND_LEN is the length of data to be sent @#@
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
 
 
void USART1_IRQHandler(void)                	//This is the interrupt request handler for USART1 interrupts @#@
{		
	u8 receive_buf ;
	static u16 send_count = 0 ;
	
	//USART_GetITStatus (USART_TypeDef *USARTx, uint32_t USART_IT)
 	//Checks whether the specified USART interrupt has occurred or not. 
	//USART_IT_RXNE: specifies the interrupt source for Receive Data register not empty interrupt.
	if(USART_GetITStatus(USART1, USART_IT_RXNE) == SET)		//Check if the USART1 receive interrupt flag was set @#@
	{
			if(modbus.protocal == MODBUS )	
			{
					if(revce_count < 250)
						USART_RX_BUF[revce_count++] = USART_ReceiveData(USART1);	//(USART1->DR); the character from the USART1 data register is saved @#@
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
					if(USART_GetITStatus(USART1, USART_IT_RXNE) == SET)		//check if the USART_IT_RXNE flag (Receive Data register not empty interrupt) is set @#@
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
	else  if( USART_GetITStatus(USART1, USART_IT_TXE) == SET  )					//if the USART_IT_TXE flag is set (Tansmit Data Register empty interrupt) @#@
     {
        if((modbus.protocal == MODBUS )||(modbus.protocal == BAC_MSTP))
				{
					 if( send_count > sendbyte_num)
						{
								 USART_ITConfig(USART1, USART_IT_TXE, DISABLE);				//USART_ITConfig (USART_TypeDef *USARTx, uint32_t USART_IT, FunctionalState NewState) Enables or disables the specified USART interrupts. @#@
								 send_count = 0 ;
								 Timer_Silence_Reset();
								 serial_restart();
						}
					 else
					 {
							USART_SendData(USART1, uart_send[send_count++] );				//Transmits single data through the USARTx peripheral. @#@
							Timer_Silence_Reset();																	//Public reset of the Silence Timer @#@
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
static void initSend_COM(void)								//Initialize the sending operation by setting the Transmit flag. @#@
{
	TXEN = SEND;
}

void send_byte(u8 ch, u8 crc)
{	
	USART_ClearFlag(USART1, USART_FLAG_TC); 		//USART_FLAG_TC: to indicate the status of the transmit operation.
																							//USART_ClearFlag (USART_TypeDef *USARTx, uint32_t USART_FLAG) Clears the USARTx's pending flags.
	USART_SendData(USART1,  ch);								//USART_SendData (USART_TypeDef *USARTx, uint16_t Data) Transmits single data through the USARTx peripheral. @#@
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
 void USART_SendDataString( u16 num )		//This function has been used to enable the Transmit interrupt @#@
 {
	 tx_count = 2 ;
	 sendbyte_num = num;
	 uart_num = 0 ;
   USART_ITConfig(USART1, USART_IT_TXE, ENABLE);//
 }
void modbus_init(void)									//This just sets parameters for the modbus initialization and restarts serial @#@
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
void internalDeal(u8 type,  u8 *pData)	//Store the data from the master used for the register read/write operations @#@
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
		multiple_write_modbus(StartAdd, pData, HeadLen);			//This is the callback to the multiple registers write operation
																																	//The callback is defined in modbus_regOp.c file @#@
	}
	else if(pData[HeadLen + 1] == WRITE_VARIABLES)
	{
		write_register_modbus(StartAdd, pData, HeadLen);							//This is the callback to the single register write operation
																																	//The callback is defined in modbus_regOp.c @#@
	}
//	else if(pData[HeadLen + 1] == MULTIPLE_WRITE)
//	{
//		uint8_t packet_counter ;
//		packet_counter = pData[HeadLen+6];			// for now buffer[4] will remain zero...
//		if()
//	
//	}
	
	if (modbus.update == 0x7F)				// ???? What object does 0x7F object id represent ???? @#@
	{
		SoftReset();		
	}
	else if(modbus.update == 0x8e)		// ???? What object does 0x8e object id represent ???? @#@
	{
		
		//address_temp = AT24CXX_ReadOneByte(EEP_UPDATE_STATUS);
		for(i=0; i<255; i++)
		AT24CXX_WriteOneByte(i, 0xff);	// ???? What object does 0xff object id represent ???? @#@
		
		
		EEP_Dat_Init();									//Initialize Data op for EEPROM @#@
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
void responseCmd(u8 type, u8* pData)		//Respond to the request from the master @#@
{
	u8  i;
	//u16 send_cout=0;
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
		response_write_variables(HeadLen, type, sendbuf, pData);		
	}
	else if(cmd == MULTIPLE_WRITE)
	{
		//send_cout = HeadLen ;
			response_multiple_write(HeadLen, type, sendbuf, pData);
	}
	else if(cmd == READ_VARIABLES)
	{
		response_read_variables(HeadLen, type, RegNum,StartAdd, sendbuf, pData);
	}

	
	else if (USART_RX_BUF[1] == CHECKONLINE)
	{
		// send first byte of information
		response_checkonline(HeadLen, pData);
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

 
 
																				//Calls the functions to initialize transmission and CRC and also to respond	@#@
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
