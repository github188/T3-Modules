void internalDeal(u8 type,  u8 *pData)	//Store the data from the master used for the register read/write operations
{

	u8 address_temp ;
	u8 div_buf ;
	u8 address_buf ;
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
		multiple_write_modbus(StartAdd);
	
	}
	else if(pData[HeadLen + 1] == WRITE_VARIABLES)
	{
		write_register_modbus(StartAdd);
	
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