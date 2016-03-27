void multiple_write_modbus(u16 StartAdd){
	
	if(StartAdd == MODBUS_MAC_ADDRESS_1)
		{
			if((modbus.mac_enable == 1) && (pData[HeadLen + 6] == 12))
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
//				while(tapdev_init())	//³õÊ¼»¯ENC28J60´íÎó
//				{								   
//				//	printf("ENC28J60 Init Error!\r\n");
//				delay_ms(50);
//				};
				IP_Change = 1 ;						// ?? [L] what is the use of this flag ?? 
				modbus.mac_enable = 0 ;		// what is the use of this flag ?? [L]
			}
		}
		#ifdef T3PT12			// Is it a Slave Modbus Device [L] ??
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
}