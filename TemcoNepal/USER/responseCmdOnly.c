void responseCmd(u8 type, u8* pData)		//Respond to the request from the master @#@
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
		response_write_variables(HeadLen, type, pData, sendbuf);
	}