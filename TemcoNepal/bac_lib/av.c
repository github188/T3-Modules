/**************************************************************************
*
* Copyright (C) 2006 Steve Karg <skarg@users.sourceforge.net>
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

/* Analog Value Objects - customize for your use */

#include <stdbool.h>
#include <stdint.h>
//#include "hardware.h"
#include "bacdef.h"
#include "bacdcode.h"
#include "bacenum.h"
#include "bacapp.h"
#include "config.h"     /* the custom stuff */
#include "wp.h"
#include "av.h"
#include "modbus.h"
#include "usart.h"
#include "registerlist.h"
#include "24cxx.h" 
//#include "ud_str.h"
//#include "user_data.h"
#ifdef T322AI
 uint8_t const  av_name[][16] = 
{
	"serial number hi",
	"serial number lo",
	"software rev",
	"baudrate",
	"modbus address",
	"product id",
	"hardware rev" ,
	"protocol",
	"filter0",
	"filter1",
	"filter2",
	"filter3",
	"filter4",
	"filter5",
	"filter6",
	"filter7",
	"filter8",
	"filter9",
	"filter10",
	"filter11",
	"filter12",
	"filter13",
	"filter14",
	"filter15",
	"filter16",
	"filter17",
	"filter18",
	"filter19",
	"filter20",
	"filter21",
	"range0",
	"range1",
	"range2",
	"range3",
	"range4",
	"range5",
	"range6",
	"range7",
	"range8",
	"range9",
	"range10",
	"range11",
	"range12",
	"range13",
	"range14",
	"range15",
	"range16",
	"range17",
	"range18",
	"range19",
	"range20",
	"range21",


};
#endif

#ifdef T38AI8AO6DO
 uint8_t const  av_name[][16] = 
{
	"serial number hi",
	"serial number lo",
	"software rev",
	"baudrate",
	"modbus address",
	"product id",
	"hardware rev" ,
	"protocol",
	"filter0",
	"filter1",
	"filter2",
	"filter3",
	"filter4",
	"filter5",
	"filter6",
	"filter7",	
	"range0",
	"range1",
	"range2",
	"range3",
	"range4",
	"range5",
	"range6",
	"range7",
};
#endif

#ifdef T3PT12
 uint8_t const  av_name[][16] = 
{
	"serial number hi",
	"serial number lo",
	"software rev",
	"baudrate",
	"modbus address",
	"product id",
	"hardware rev" ,
	"protocol",
	"filter0",
	"filter1",
	"filter2",
	"filter3",
	"filter4",
	"filter5",
	"filter6",
	"filter7",	
	"range0",
	"range1",
	"range2",
	"range3",
	"range4",
	"range5",
	"range6",
	"range7",
};
#endif
//#include "product.h"


#if READ_WRITE_PROPERTY


extern  char text_string[20];
//#if (MAX_AVS > 10)
//#error Modify the Analog_Value_Name to handle multiple digits
//#endif

uint32_t far AV_Present_Value[MAX_AVS];

/* we simply have 0-n object instances.  Yours might be */
/* more complex, and then you need validate that the */
/* given instance exists */
bool Analog_Value_Valid_Instance(
    uint32_t object_instance)
{
    if (object_instance < MAX_AVS)
        return true;

    return false;
}

/* we simply have 0-n object instances.  Yours might be */
/* more complex, and then count how many you have */
unsigned Analog_Value_Count(
    void)
{
    return MAX_AVS;
}

/* we simply have 0-n object instances.  Yours might be */
/* more complex, and then you need to return the instance */
/* that correlates to the correct index */
uint32_t Analog_Value_Index_To_Instance(
    unsigned index)
{
    return index;
}

/* we simply have 0-n object instances.  Yours might be */
/* more complex, and then you need to return the index */
/* that correlates to the correct instance number */
unsigned Analog_Value_Instance_To_Index(
    uint32_t object_instance)
{   
//	unsigned index = MAX_AVS;
//
//    if (object_instance < MAX_AVS)
//        index = object_instance;
//
//    return index;
	return object_instance;
}

/* note: the object name must be unique within this device */
char *Analog_Value_Name(
    uint32_t object_instance)
{

	uint16_t object_index ;
	
	object_index = Analog_Value_Instance_To_Index(object_instance);
    if (object_instance < MAX_AVS) {
		memcpy(text_string,av_name[object_instance],16);
        return text_string;
    }


    return text_string;
}

/* return apdu len, or -1 on error */
int Analog_Value_Encode_Property_APDU(
    uint8_t * apdu,
    uint32_t object_instance,
    BACNET_PROPERTY_ID property,
    uint32_t array_index,
    BACNET_ERROR_CLASS * error_class,
    BACNET_ERROR_CODE * error_code)
{
    int apdu_len = 0;   /* return value */
	uint8_t i ;
    BACNET_BIT_STRING xdata bit_string;
    BACNET_CHARACTER_STRING xdata char_string;
    unsigned object_index;
    switch (property) {
        case PROP_OBJECT_IDENTIFIER:
            apdu_len =
                encode_application_object_id(&apdu[0], OBJECT_ANALOG_VALUE,
                object_instance);
            break;
        case PROP_OBJECT_NAME:

            characterstring_init_ansi(&char_string,
			//	vars[object_instance].label
                Analog_Value_Name(object_instance)
				);
            apdu_len =
                encode_application_character_string(&apdu[0], &char_string);

            break;
        case PROP_OBJECT_TYPE:
            apdu_len =
                encode_application_enumerated(&apdu[0], OBJECT_ANALOG_VALUE);
            break;
        case PROP_PRESENT_VALUE:
//			#ifdef T322AI
			AV_Present_Value[0] = (modbus.serial_Num[0]<<8)|modbus.serial_Num[1];
			AV_Present_Value[1] = (modbus.serial_Num[1]<<8)|modbus.serial_Num[3]  ;
			AV_Present_Value[2] = SOFTREV;
			AV_Present_Value[3] = 	modbus.baudrate ;
			AV_Present_Value[4] = 	modbus.address ;
			AV_Present_Value[5] = 	modbus.product ;
			AV_Present_Value[6] =	modbus.hardware_Rev;
			AV_Present_Value[7] =	modbus.protocal;
			for(i=0; i<MAX_AIS; i++)
			{
				AV_Present_Value[8+i] = modbus.filter_value[i] ;
				AV_Present_Value[8+MAX_AIS+i] = modbus.range[i] ;
			}
//			#endif
            object_index = Analog_Value_Instance_To_Index(object_instance);
            apdu_len = encode_application_real(&apdu[0],
				AV_Present_Value[object_index]);
            break;
        case PROP_STATUS_FLAGS:
            bitstring_init(&bit_string);
            bitstring_set_bit(&bit_string, STATUS_FLAG_IN_ALARM, false);
            bitstring_set_bit(&bit_string, STATUS_FLAG_FAULT, false);
            bitstring_set_bit(&bit_string, STATUS_FLAG_OVERRIDDEN, false);
            bitstring_set_bit(&bit_string, STATUS_FLAG_OUT_OF_SERVICE, false);
            apdu_len = encode_application_bitstring(&apdu[0], &bit_string);
            break;
        case PROP_EVENT_STATE:
            apdu_len =
                encode_application_enumerated(&apdu[0], EVENT_STATE_NORMAL);
            break;
        case PROP_OUT_OF_SERVICE:
            apdu_len = encode_application_boolean(&apdu[0], false);
            break;
        case PROP_UNITS:
            apdu_len = encode_application_enumerated(&apdu[0], UNITS_NO_UNITS);
            break;
        default:
            *error_class = ERROR_CLASS_PROPERTY;
            *error_code = ERROR_CODE_UNKNOWN_PROPERTY;
            apdu_len = -1;
            break;
    }
    /*  only array properties can have array options */
    if ((apdu_len >= 0) && (array_index != BACNET_ARRAY_ALL)) {
        *error_class = ERROR_CLASS_PROPERTY;
        *error_code = ERROR_CODE_PROPERTY_IS_NOT_AN_ARRAY;
        apdu_len = -1;    
    }

    return apdu_len;
}

/* returns true if successful */
bool Analog_Value_Write_Property(
    BACNET_WRITE_PROPERTY_DATA xdata * wp_data,
    BACNET_ERROR_CLASS * error_class,
    BACNET_ERROR_CODE * error_code)
{
    bool status = false;        /* return value */
    unsigned int object_index = 0;
    int len = 0;
    BACNET_APPLICATION_DATA_VALUE value;

    if (!Analog_Value_Valid_Instance(wp_data->object_instance)) {
        *error_class = ERROR_CLASS_OBJECT;
        *error_code = ERROR_CODE_UNKNOWN_OBJECT;
        return false;
    }
    /* decode the some of the request */
    len =
        bacapp_decode_application_data(wp_data->application_data,
        wp_data->application_data_len, &value);
    /* FIXME: len < application_data_len: more data? */
    if (len < 0) {
        /* error while decoding - a value larger than we can handle */
        *error_class = ERROR_CLASS_PROPERTY;
        *error_code = ERROR_CODE_VALUE_OUT_OF_RANGE;
        return false;
    }
    if ((wp_data->object_property != PROP_PRIORITY_ARRAY) &&
        (wp_data->array_index != BACNET_ARRAY_ALL)) {
        /*  only array properties can have array options */
        *error_class = ERROR_CLASS_PROPERTY;
        *error_code = ERROR_CODE_PROPERTY_IS_NOT_AN_ARRAY;
        return false;
    }
    switch (wp_data->object_property) {
        case PROP_PRESENT_VALUE:
            if (value.tag == BACNET_APPLICATION_TAG_REAL) {
                object_index =
                    Analog_Value_Instance_To_Index(wp_data->object_instance);
                AV_Present_Value[object_index] = value.type.Real;
								switch(object_index)
				{
					case 0:
						modbus.serial_Num[0]= (AV_Present_Value[object_index]>>8)&0xff ;
						modbus.serial_Num[1]=  AV_Present_Value[object_index]&0xff ;
						AT24CXX_WriteOneByte((u16)EEP_SERIALNUMBER_LOWORD, modbus.serial_Num[0]);
						AT24CXX_WriteOneByte((u16)EEP_SERIALNUMBER_LOWORD+1, modbus.serial_Num[1]);
					break;
					case 1:
						modbus.serial_Num[2]= (AV_Present_Value[object_index]>>8)&0xff ;
						modbus.serial_Num[3]=  AV_Present_Value[object_index]&0xff ;
						AT24CXX_WriteOneByte(EEP_SERIALNUMBER_HIWORD, modbus.serial_Num[2]);
						AT24CXX_WriteOneByte(EEP_SERIALNUMBER_HIWORD+1, modbus.serial_Num[3]);
					break;
					case 2:
//						modbus.software = AV_Present_Value[object_index];	
//						AT24CXX_WriteOneByte(EEP_VERSION_NUMBER_LO,(modbus.software>>8)&0xff );
//						AT24CXX_WriteOneByte(EEP_VERSION_NUMBER_HI, modbus.software&0xff);
					break;
					case 3:
						modbus.baudrate = AV_Present_Value[object_index];
						uart1_init(modbus.baudrate);
						if(modbus.baudrate == 9600)  modbus.baud = 0; 
						else if(modbus.baudrate == 19200)  modbus.baud = 1; 
						else if(modbus.baudrate == 38400)  modbus.baud = 2;
						else if(modbus.baudrate == 57600)  modbus.baud = 3;
						else if(modbus.baudrate == 115200)  modbus.baud = 4;
						AT24CXX_WriteOneByte(EEP_BAUDRATE, modbus.baud);
					break;
					case 4:
						modbus.address = AV_Present_Value[object_index];
						AT24CXX_WriteOneByte(EEP_ADDRESS, modbus.address);
					break;
					case 5:
						modbus.product = AV_Present_Value[object_index];
						AT24CXX_WriteOneByte(EEP_PRODUCT_MODEL, modbus.product);
					break;
					case 6:
						modbus.hardware_Rev = AV_Present_Value[object_index];
						AT24CXX_WriteOneByte(EEP_HARDWARE_REV, modbus.hardware_Rev);
					break;
					case 7:
						modbus.protocal = AV_Present_Value[object_index];
						AT24CXX_WriteOneByte(EEP_MODBUS_COM_CONFIG, modbus.protocal);					
					break;
//					case 8:
//					case 9:
//					case 10:
//					case 11:
//					case 12:			
//					case 13:
//					case 14:
//					case 15:
//					case 16:
//					case 17:
//					case 18:
//					case 19:
//					case 20:			
//					case 21:
//					case 22:
//					case 23:
//					case 24:
//					case 25:
//					case 26:			
//					case 27:
//					case 28:
//					case 29:
//						modbus.filter_value[object_index -8] = AV_Present_Value[object_index];
//						AT24CXX_WriteOneByte(EEP_AI_FILTER0+object_index -8, modbus.filter_value[object_index -8]);
//					break;
//					case 30:
//					case 31:
//					case 32:
//					case 33:
//					case 34:			
//					case 35:
//					case 36:
//					case 37:
//					case 38:
//					case 39:
//					case 40:
//					case 41:
//					case 42:			
//					case 43:
//					case 44:
//					case 45:
//					case 46:
//					case 47:
//					case 48:			
//					case 49:
//					case 50:
//					case 51:
//						modbus.range[object_index -30] = AV_Present_Value[object_index];
//						AT24CXX_WriteOneByte(EEP_AI_RANGE0+object_index -30, modbus.range[object_index -30]);
//					break;
					
				}
//				wirte_bacnet_value_to_buf(AV,wp_data->priority,object_index);
                status = true;
            } else {
                *error_class = ERROR_CLASS_PROPERTY;
                *error_code = ERROR_CODE_INVALID_DATA_TYPE;
            }
            break;
        case PROP_OBJECT_IDENTIFIER:
        case PROP_OBJECT_NAME:
        case PROP_OBJECT_TYPE:
        case PROP_STATUS_FLAGS:
        case PROP_EVENT_STATE:
        case PROP_OUT_OF_SERVICE:
        case PROP_DESCRIPTION:
        case PROP_PRIORITY_ARRAY:
            *error_class = ERROR_CLASS_PROPERTY;
            *error_code = ERROR_CODE_WRITE_ACCESS_DENIED;
            break;
        default:
            *error_class = ERROR_CLASS_PROPERTY;
            *error_code = ERROR_CODE_UNKNOWN_PROPERTY;
            break;
    }

    return status;
}

#ifdef TEST
#include <assert.h>
#include <string.h>
#include "ctest.h"

void testAnalog_Value(
    Test * pTest)
{
    uint8_t far apdu[MAX_APDU] = { 0 };
    int len = 0;
    uint32_t len_value = 0;
    uint8_t tag_number = 0;
    BACNET_OBJECT_TYPE decoded_type = OBJECT_ANALOG_VALUE;
    uint32_t decoded_instance = 0;
    uint32_t instance = 123;
    BACNET_ERROR_CLASS error_class;
    BACNET_ERROR_CODE error_code;

    len =
        Analog_Value_Encode_Property_APDU(&apdu[0], instance,
        PROP_OBJECT_IDENTIFIER, BACNET_ARRAY_ALL, &error_class, &error_code);
    ct_test(pTest, len != 0);
    len = decode_tag_number_and_value(&apdu[0], &tag_number, &len_value);
    ct_test(pTest, tag_number == BACNET_APPLICATION_TAG_OBJECT_ID);
    len =
        decode_object_id(&apdu[len], (int *) &decoded_type, &decoded_instance);
    ct_test(pTest, decoded_type == OBJECT_ANALOG_VALUE);
    ct_test(pTest, decoded_instance == instance);

    return;
}

#ifdef TEST_ANALOG_VALUE
int main(
    void)
{
    Test *pTest;
    bool rc;

    pTest = ct_create("BACnet Analog Value", NULL);
    /* individual tests */
    rc = ct_addTestFunction(pTest, testAnalog_Value);
    assert(rc);

    ct_setStream(pTest, stdout);
    ct_run(pTest);
    (void) ct_report(pTest);
    ct_destroy(pTest);

    return 0;
}
#endif /* TEST_ANALOG_VALUE */
#endif /* TEST */

#endif
