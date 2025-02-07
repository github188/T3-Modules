/**************************************************************************
*
* Copyright (C) 2009 Steve Karg <skarg@users.sourceforge.net>
*
* Permission is hereby granted, free of charge, to any person obtaining
* a copy of this software and associated documentation files (the
* "Software"), to deal in the Software without restriction, including
* without limitation the rights to use, copy, modify, merge, publish,
* distribute, sublicense, and/or sell copies of the Software, and to
* permit persons to whom the Software is furnished to do so, subject to
* the following conditions:
*
* The above copyright notice and this permission notice shall be included
* in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*
*********************************************************************/
#include <stddef.h>
#include <stdint.h>
#include <errno.h>
#include <string.h>
#include "config.h"
#include "txbuf.h"
#include "bacdef.h"
#include "bacdcode.h"
#include "address.h"
#include "tsm.h"
#include "npdu.h"
#include "apdu.h"
#include "device.h"
#include "datalink.h"
#include "dcc.h"
#include "ptransfer.h"
/* some demo stuff needed */
#include "handlers.h"
#include "txbuf.h"
#include "client.h"
#include "modbus.h"

//#include "main.h"

/** @file s_upt.c  Send an Unconfirmed Private Transfer request. */
extern uint8_t invoke_id;


void Send_UnconfirmedPrivateTransfer(
    BACNET_ADDRESS * dest,
    BACNET_PRIVATE_TRANSFER_DATA * private_data)
{
    int len = 0;
    int pdu_len = 0;
    
    BACNET_NPDU_DATA npdu_data;
    BACNET_ADDRESS my_address; 	

//    if (!dcc_communication_enabled())	   tbd:
//        return;
    datalink_get_my_address(&my_address,modbus.protocal);
    /* encode the NPDU portion of the packet */
    npdu_encode_npdu_data(&npdu_data, false, MESSAGE_PRIORITY_NORMAL);

    pdu_len =
        npdu_encode_pdu(&Handler_Transmit_Buffer[0], dest, &my_address,
        &npdu_data);
#if USB_HOST
	if(Modbus.protocal == BAC_GSM)
	{
		pdu_len = 2;	
	}
#endif
    /* encode the APDU portion of the packet */
    len =
        ptransfer_ack_encode_apdu(&Handler_Transmit_Buffer[pdu_len],invoke_id,
        private_data);
    pdu_len += len;
   	datalink_send_pdu(dest, &npdu_data, &Handler_Transmit_Buffer[0],pdu_len,modbus.protocal);	

}
