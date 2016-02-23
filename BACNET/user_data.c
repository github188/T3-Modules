
#include "ud_str.h"
#include "string.h"
#include "types.h"
#include "lcd.h"
#include "define.h"
#include "modbus.h"
#include "rtc.h"
#include "ud_str.h"
#include "user_data.h"


void Updata_Clock(void);
void update_timers( void );

#if PTP

#define MAX_reachable_networks 		2
#define MAX_Routing_table      		2
#define MAX_NetSession         		2


//FRAME_ENTRY 			  	SendFrame[MAX_SEND_FRAMES];
//STR_PTP  					Ptp_para;
//UNITDATA_PARAMETERS  		NL_PARAMETERS;
//Routing_Table               Routing_table[MAX_Routing_table];

#endif


//#include "schedule.h"
//uint8_t far BACnet_Port;

//uint8_t				far				 no_outs;
//uint8_t				far				 no_ins;

Info_Table			far						 info[18];// _at_ 0x41000;

//U8_T far month_length[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

unsigned long 				timestart;	   /* seconds since the beginning of the year */
unsigned long 				ora_current_sec;  /* seconds since the beginning of the day */
unsigned long 				time_since_1970;   /* seconds since the beginning of 2010 */


Str_Panel_Info 		far 		Panel_Info;
Str_Setting_Info    far 		Setting_Info;
//In_aux				far			in_aux[MAX_IO_POINTS];//_at_ 0x17500; 

U16_T input_raw[MAX_AIS];
U16_T output_raw[MAX_OUTS];
U16_T chip_info[6];
uint16_t far Instance;

U8_T far flag_E2_changed;
U32_T far changed_index;

Str_in_point far inputs[MAX_AIS];// _at_ 0x20000
Str_out_point far	outputs[MAX_OUTS];//_at_ 0x22000;
//Con_aux				far			con_aux[MAX_CONS];//_at_ 0x26000; 
//Mon_aux           	far         mon_aux[MAX_MONITORS];// _at_ 0x27000; 
//U32_T 				far 		SD_lenght[MAX_MONITORS * 2];
//U32_T 				far 		SD_Start_pos[MAX_MONITORS * 2];
//Monitor_Block		far			*mon_block;
//char 				far         mon_data_buf[sizeof(Str_mon_element) * MAX_MON_ELEMENT * MAX_MONITORS ];// _at_ 0x30000;
//Mon_Data 			far 		*Graphi_data;
//char 				far			Garphi_data_buf[sizeof(Mon_Data)];//  _at_ 0x41100;
//Alarm_point 		far			alarms[MAX_ALARMS];
//U8_T 			    far			ind_alarms;
//Alarm_set_point 	far    		alarms_set[MAX_ALARMS_SET];
//U8_T 			    far							 ind_alarms_set;
//U16_T                alarm_id;
//char                         new_alarm_flag;
Units_element		    far			digi_units[MAX_DIG_UNIT];
U8_T 					far 		ind_passwords;
Password_point			far			passwords[ MAX_PASSW ];
//Str_program_point	    far			programs[MAX_PRGS] _at_ 0x24000;
Str_variable_point		far			vars[MAX_VARS];// _at_ 0x18000;
//Str_controller_point 	far			controllers[MAX_CONS];// _at_ 0x25000;
Str_totalizer_point     far     	totalizers[MAX_TOTALIZERS];// _at_ 0x12500;
//Str_monitor_point		far			monitors[MAX_MONITORS];// _at_ 0x12800;
//Str_monitor_point		far			backup_monitors[MAX_MONITORS];// _at_ 0x2e000;


//Aux_group_point         far 		aux_groups[MAX_GRPS];// _at_ 0x13500;
//S8_T                    far		Icon_names[MAX_ICONS][14];
//Control_group_point  	far 		control_groups[MAX_GRPS];// _at_ 0x14000;
//Str_grp_element		    far	    	group_data[MAX_ELEMENTS];// _at_ 0x14500;
//Str_grp_element			far 	    group_data[MAX_ELEMENTS];

S16_T 					far							 total_elements;
S16_T 					far							 group_data_length;


//Str_weekly_routine_point 	far		weekly_routines[MAX_WR];//_at_ 0x28000; // _at_ 0x15200;
//Wr_one_day					far		wr_times[MAX_WR][MAX_SCHEDULES_PER_WEEK];//_at_ 0x29000 ;//_at_ 0x16000;
Str_annual_routine_point	far	 	annual_routines[MAX_AR];// _at_ 0x2a000;//_at_ 0x16500;
//U8_T                   		far     ar_dates[MAX_AR][AR_DATES_SIZE];//_at_ 0x2b000 ;//_at_ 0x17000;
	
//Date_block	ora_current;
 /* Assume bit0 from octet0 = Jan 1st */
//S8_T 			    	far			*program_address[MAX_PRGS]; /*pointer to code*/
//U8_T    	    	far				prg_code[MAX_PRGS][CODE_ELEMENT];// _at_ 0x8000; 
//U16_T				far	 			Code_len[MAX_PRGS];
U16_T 			    far				Code_total_length;
//Str_array_point 	    far			 arrays[MAX_ARRAYS];
//S32_T  			    				*arrays_address[MAX_ARRAYS];
//Str_table_point			far				 custom_tab[MAX_TBLS];
U16_T          far         		PRG_crc;
U8_T           far              free_mon_blocks;
//S16_T                          	MAX_MONITOR_BLOCKS = FLASH_BUFFER_LENGTH / sizeof( Monitor_Block );
U16_T  total_send_length;
//Byte			               	Station_NUM;
char far panelname[20];

//Str_pro_timer far	progrom_timer[MAX_PRO_TIMER];
Byte far	num_of_time;
Byte far program_index;
Byte far timer_type;
Byte far timer_index;

NETWORK_POINTS        far  		 network_points_list[MAXNETWORKPOINTS];	 /* points wanted by others */
Byte                 far			 number_of_network_points; 
REMOTE_POINTS		  far  		 remote_points_list[MAXREMOTEPOINTS];  /* points from other panels used localy */
Byte                far			 number_of_remote_points;

//U8_T 	client_ip[4];
//U8_T newsocket = 0;

U8_T far *prog;
S32_T far stack[20];
S32_T far *index_stack;
S8_T far *time_buf;
U32_T far cond;
S32_T far v, value;
S32_T far op1,op2;
S32_T far n,*pn;
//char far message[10];
//unsigned char alarm_flag;
//char alarm_at_all;
//char ind_alarm_panel;
//char alarm_panel[5];

//U8_T far flag_Moniter_changed;
//U8_T count_monitor_changed;
//extern STR_flag_flash 	far bac_flash;

#if 1
U8_T far table_bank[TABLE_BANK_LENGTH] =
{
	 MAX_OUTS,     	/*OUT*/
	 MAX_AIS,     	/*IN*/
	 MAX_VARS,      /*VAR*/
	// MAX_CONS,     	/*CON*/
	 MAX_WR,        /*WR*/
	 MAX_AR,        /*AR*/
	 MAX_PRGS,     	/*PRG*/
	 MAX_TBLS,      /*TBL*/
	 MAX_TOTALIZERS, /*TOTAL*/
//	 MAX_MONITORS,	/*AMON*/
	 MAX_GRPS,      /*GRP*/
	 MAX_ARRAYS,    /*AY*/
	 MAX_ALARMS,
	 MAX_DIG_UNIT,
	 MAX_PASSW
};
#endif

//char * TEST_IN[10] = {"INPUT1","INPUT2","INPUT3","INPUT4","INPUT5","INPUT6","INPUT7","INPUT8","INPUT9","INPUT10"};
//char * TEST_OUT[2] = {"OUTPUT1","OUTPUT2"};



void init_info_table( void )
{
	int i;
	Info_Table *inf;

	inf = &info[0];
	for( i=0; i< MAX_INFO_TYPE; i++, inf++ )
	{
		switch( i )
		{
			case OUT:
				inf->address = (S8_T *)outputs;
				inf->size = sizeof( Str_out_point );
				inf->max_points =  MAX_OUTS;
				break;
			case IN:
				inf->address = (S8_T *)inputs;
				inf->size = sizeof( Str_in_point );
				inf->max_points =  MAX_INS;
				break;
			case VAR:
//				inf->address = (S8_T *)vars;
//				inf->size = sizeof( Str_variable_point );
//				inf->max_points =  MAX_VARS;
				break;
			case CON:
//				inf->address = (S8_T *)controllers;
//				inf->size = sizeof( Str_controller_point );
//				inf->max_points = MAX_CONS;
				break;
			case WRT:
//				inf->address = (S8_T *)weekly_routines;
//				inf->size = sizeof( Str_weekly_routine_point );
//				inf->max_points = MAX_WR;
				break;
			case AR:
				inf->address = (S8_T *)annual_routines;
				inf->size = sizeof( Str_annual_routine_point );
				inf->max_points = MAX_AR;
				break;
//			case PRG:
//				inf->address = (S8_T *)programs;
//				inf->size = sizeof( Str_program_point );
//				inf->max_points = MAX_PRGS;
//				break;
		/*	case TBL:
				inf->address = (S8_T *)custom_tab;
				inf->size = sizeof( Str_table_point );
				inf->max_points = MAX_TBLS;
				break; */
			case TZ:
				inf->address = (S8_T *)totalizers;
				inf->size = sizeof( Str_totalizer_point );
				inf->max_points = MAX_TOTALIZERS;
				break;
			case AMON:
//				inf->address = (S8_T *)monitors;
//				inf->size = sizeof( Str_monitor_point );
//				inf->max_points = MAX_MONITORS;
				break;
			case GRP:
//				inf->address = (S8_T *)control_groups;
//				inf->size = sizeof( Control_group_point );
//				inf->max_points = MAX_GRPS;
				break;
		/*	case ARRAY:
				inf->address = (S8_T *)arrays;
				inf->size = sizeof( Str_array_point );
				inf->max_points = MAX_ARRAYS;
				break;	*/
			case ALARMM:          // 12
//				inf->address = (S8_T *)alarms;
//				inf->size = sizeof( Alarm_point );
//				inf->max_points = MAX_ALARMS;
				break;
			case ALARM_SET:         //15
//				inf->address = (S8_T *)alarms_set;
//				inf->size = sizeof( Alarm_set_point );
//				inf->max_points = MAX_ALARMS_SET;
				break;
			case UNIT:
				inf->address = (S8_T *)digi_units;
				inf->size = sizeof( Units_element );
				inf->max_points = MAX_DIG_UNIT;
				break;										  
			case USER_NAME:
				inf->address = (S8_T *)&passwords;
				inf->size = sizeof( Password_point );
				inf->max_points = MAX_PASSW;
				break;	  
			case WR_TIME:
//				inf->address = (S8_T *)wr_times;
//				inf->size = 9*sizeof( Wr_one_day );
//				inf->max_points = MAX_SCHEDULES_PER_WEEK;
				break;
			case AR_DATA:               // 17 ar_dates[MAX_AR][AR_DATES_SIZE];
//				inf->address = (S8_T *)ar_dates;
//				inf->size = AR_DATES_SIZE;
//				inf->max_points = MAX_AR;
				break;
			default:
				break;	
		}
	}
}


void init_panel(void)
{	 	
	uint16_t i,j;
	Str_points_ptr ptr;
//	ind_alarms_set = 0;
//	ind_alarms = 0;

//	just_load = 0;
//	miliseclast_cur = 0;


  memset(inputs, '\0', MAX_INS *sizeof(Str_in_point) );
	ptr.pin = inputs;
	for( i=0; i< MAX_INS; i++, ptr.pin++ )
	{
		ptr.pin->value = 0;  
//		memcpy(&ptr.pin->description[0],'\0',21);
		ptr.pin->filter = 0;  /* (3 bits; 0=1,1=2,2=4,3=8,4=16,5=32, 6=64,7=128,)*/
		ptr.pin->decom = 0;	   /* (1 bit; 0=ok, 1=point decommissioned)*/
		ptr.pin->sen_on = 1;/* (1 bit)*/
		ptr.pin->sen_off = 1;  /* (1 bit)*/
		ptr.pin->control = 1; /*  (1 bit; 0=OFF, 1=ON)*/
		ptr.pin->auto_manual = 0; /* (1 bit; 0=auto, 1=manual)*/
		ptr.pin->digital_analog = 1; /* (1 bit; 1=analog, 0=digital)*/
		ptr.pin->calibration_sign = 1;; /* (1 bit; sign 0=positiv 1=negative )*/
		ptr.pin->calibration_increment = 1;; /* (1 bit;  0=0.1, 1=1.0)*/
		ptr.pin->calibration_hi = 0;  /* (8 bits; -25.6 to 25.6 / -256 to 256 )*/
		ptr.pin->calibration_lo = 0; 
//		memcpy(ptr.pin->label,'\0',9);		
	}

	memset(outputs,'\0', MAX_OUTS *sizeof(Str_out_point) );
	ptr.pout = outputs;
	for( i=0; i<MAX_OUTS; i++, ptr.pout++ )
	{
		ptr.pout->value = 0; 		
		ptr.pout->range = 0;
		ptr.pout->digital_analog = 0;
		ptr.pout->auto_manual = 0;
	} 

//	memset(controllers,'\0',MAX_CONS*sizeof(Str_controller_point));
//	memset(programs,'\0',MAX_PRGS *sizeof(Str_program_point));
//	ptr.pprg = programs;
//	for( i = 0; i < MAX_PRGS; i++, ptr.pprg++ )
//	{
//		ptr.pprg->on_off = 1;
//		ptr.pprg->auto_manual = 0;
//		ptr.pprg->bytes = 0;
////		memcpy(ptr.pprg->description,'\0',21);	
//	} //test by chelsea	
	Code_total_length = 0;
////	memset(prg_code, '\0', MAX_PRGS * CODE_ELEMENT);
//// 	for(i = 0;i < MAX_PRGS;i++)	
////	{	for(j = 0;j < CODE_ELEMENT;j++)	
////		 prg_code[i][j] = 0;
////	}	
	
//	total_length = 0;
//	memset(vars,'\0',MAX_VARS*sizeof(Str_variable_point));
//	ptr.pvar = vars;
	for( i=0; i < MAX_VARS; i++, ptr.pvar++ )
	{
		ptr.pvar->value = 0;
		ptr.pvar->auto_manual = 0;
		ptr.pvar->digital_analog = 1; //analog point 
		ptr.pvar->unused = 2; 
		ptr.pvar->range = 0;
//		memcpy(ptr.pvar->description,'\0',21);
	}

//	memset( control_groups,'\0', MAX_GRPS * sizeof( Control_group_point) );
//	memset( aux_groups,'\0', MAX_GRPS * sizeof( Aux_group_point) );
//	memset( group_data, '\0', MAX_ELEMENTS * sizeof( Str_grp_element) );
	total_elements = 0;
	group_data_length = 0;
//	ptr.pgrp = control_groups;		
//	for( i=0; i<MAX_GRPS; i++, ptr.pgrp++ )
//	{
//		ptr.pgrp->update = 15;
//	}

//	memset(controllers,'\0',MAX_CONS*sizeof(Str_controller_point));
//	memset(con_aux,'\0',MAX_CONS*sizeof(Con_aux));		
//	memset(&passwords,'\0',sizeof(Password_point) * MAX_PASSW);

/*	memset(custom_tab,'\0',MAX_TBLS*16*sizeof(Tbl_point));	*/

	memset(network_points_list,0,sizeof(network_points_list));
	number_of_network_points=0;
	memset(remote_points_list,0,sizeof(remote_points_list));
	number_of_remote_points=0;	
	
//	memset(progrom_timer,0,sizeof(Str_pro_timer) * MAX_PRO_TIMER);
	num_of_time = 0;
	program_index = 0;
	timer_type = 0;
	timer_index = 0;



//	memset( weekly_routines,'\0',MAX_WR*sizeof(Str_weekly_routine_point));
	//ptr.pwr = weekly_routines;
//	for( i=0; i<MAX_WR; i++, ptr.pwr++ )
//	{
//		ptr.pwr->value = 1;
//		ptr.pwr->auto_manual = 1;
//	}


//	memset( wr_times,'\0',MAX_WR*9*sizeof(Wr_one_day ));
//	memset( annual_routines,'\0',MAX_AR*sizeof(Str_annual_routine_point));
//	memset( ar_dates,'\0',MAX_AR*46*sizeof(S8_T));
	
	
	memset( totalizers,'\0',MAX_TOTALIZERS*sizeof(Str_totalizer_point));

//	memset(arrays,'\0',MAX_ARRAYS*sizeof(Str_array_point));
//	memset(arrays_data,'\0',MAX_ARRAYS*sizeof(S32_T *));
	memset(digi_units,'\0',MAX_DIG_UNIT*sizeof(Units_element));


//	memset( monitors,'\0',MAX_MONITORS*sizeof(Str_monitor_point));		
//	memset( backup_monitors,'\0',MAX_MONITORS*sizeof(Str_monitor_point));
#if  STORE_TO_SD
//	memset( mon_aux,'\0',MAX_MONITORS*sizeof(Mon_aux) );   
	//mon_block = mon_data_buf;  // tbd: changed by chelsea
	memset( mon_block,'\0',2 * MAX_MONITORS*MAX_MON_ELEMENT*sizeof(Str_mon_element) );
#endif

//	ptr.pmon = monitors;

//	for( i=0; i<MAX_MONITORS; i++, ptr.pmon++ )
//	{
//	//	ptr.pmon->second_interval_time = 0;
//		ptr.pmon->minute_interval_time = 15;
//	//	ptr.pmon->hour_interval_time = 0;
//	//	ptr.pmon->double_flag=1;
//	
//	}

//	free_mon_blocks = MAX_MONITORS;
//	Graphi_data = Garphi_data_buf;
//	memset( Graphi_data,'\0',sizeof(Mon_Data));
		
//	memset(alarms,0,MAX_ALARMS * sizeof(Alarm_point));
//	ptr.palrm = alarms;
//	for( i=0; i<MAX_ALARMS; i++, ptr.palrm++ )
//	{
//		ptr.palrm->alarm = 0;
//		ptr.palrm->ddelete = 0;
//	}
//	memset(alarms_set,'\0',MAX_ALARMS_SET*sizeof(Alarm_set_point));
	//Updata_Clock();
	RTC_Get();
	update_timers();

	memset(input_raw,0,2 * MAX_INS);
	memset(output_raw,0,2 * MAX_OUTS);
	memset(chip_info,0,12);
	init_info_table();

//	memset(Remote_tst_db,0,sizeof(Str_Remote_TstDB) * SUB_NO);
	memset(panelname,0,20);

	flag_E2_changed = 0;
	changed_index = 0;
}

void Initial_Panel_Info(void)
{
	memset(&Panel_Info,35,sizeof(Str_Panel_Info));
	Panel_Info.reg.mac[0] = 0x0c;
	#if 0
	memset(&Panel_Info,0,sizeof(Str_Panel_Info));
	memset(&Setting_Info,0,sizeof(Str_Setting_Info));

	//Panel_Info.reg.instance = swap_word(Instance);
	
//		Panel_Info.reg.mac[0] = Station_NUM;
//	Panel_Info.reg.mac[0] = Modbus.ip_addr[0];
//	Panel_Info.reg.mac[1] = Modbus.ip_addr[1];
//	Panel_Info.reg.mac[2] = Modbus.ip_addr[2];
//	Panel_Info.reg.mac[3] = Modbus.ip_addr[3];
	Panel_Info.reg.mac[4] = 0xBA;
	Panel_Info.reg.mac[5] = 0xc0;
		

//	Panel_Info.reg.serial_num[0] = Modbus.serialNum[0];
//	Panel_Info.reg.serial_num[1] = Modbus.serialNum[1];
//	Panel_Info.reg.serial_num[2] = Modbus.serialNum[2];
//	Panel_Info.reg.serial_num[3] = Modbus.serialNum[3];

//	Panel_Info.reg.modbus_addr = Modbus.address;
//	Panel_Info.reg.modbus_port = swap_word(Modbus.tcp_port);
//	Panel_Info.reg.hw = swap_word(Modbus.hardRev);
//	Panel_Info.reg.sw = SW_REV % 100 + (U16_T)((SW_REV / 100) << 8);

//		if(Modbus.protocal == BAC_IP)	
//	Panel_Info.reg.panel_number = Modbus.ip_addr[3];
//		else if(Modbus.protocal == BAC_MSTP)
	Panel_Info.reg.panel_number	= Station_NUM;

//	Setting_Info.reg.ip_addr[0] = Modbus.ip_addr[0];
//	Setting_Info.reg.ip_addr[1] = Modbus.ip_addr[1];
//	Setting_Info.reg.ip_addr[2] = Modbus.ip_addr[2];
//	Setting_Info.reg.ip_addr[3] = Modbus.ip_addr[3];
//	Setting_Info.reg.subnet[0] = Modbus.subnet[0];
//	Setting_Info.reg.subnet[1] = Modbus.subnet[1];
//	Setting_Info.reg.subnet[2] = Modbus.subnet[2];
//	Setting_Info.reg.subnet[3] = Modbus.subnet[3];
//	Setting_Info.reg.getway[0]= Modbus.getway[0];
//	Setting_Info.reg.getway[1]= Modbus.getway[1];
//	Setting_Info.reg.getway[2]= Modbus.getway[2];
//	Setting_Info.reg.getway[3]= Modbus.getway[3];
//	Setting_Info.reg.mac_addr[0] = Modbus.mac_addr[0];
//	Setting_Info.reg.mac_addr[1] = Modbus.mac_addr[1];
//	Setting_Info.reg.mac_addr[2] = Modbus.mac_addr[2];
//	Setting_Info.reg.mac_addr[3] = Modbus.mac_addr[3];
//	Setting_Info.reg.mac_addr[4] = Modbus.mac_addr[4];
//	Setting_Info.reg.mac_addr[5] = Modbus.mac_addr[5];
//	Setting_Info.reg.mac_addr[6] = Modbus.mac_addr[6];
//	Setting_Info.reg.tcp_type = Modbus.tcp_type;
//	Setting_Info.reg.mini_type = Modbus.mini_type;

//	Setting_Info.reg.en_username = Modbus.en_username;
//	Setting_Info.reg.cus_unit = Modbus.cus_unit;
//	Setting_Info.reg.usb_mode = Modbus.usb_mode;

//	Setting_Info.reg.network_number = 1;// Modbus.network_number;
//	Setting_Info.reg.panel_type = MINIPANEL;

//	Setting_Info.reg.pro_info.harware_rev = HW_REV;
//	Setting_Info.reg.pro_info.firmware0_rev = SW_REV % 100 + (U16_T)((SW_REV / 100) << 8);
//	Setting_Info.reg.pro_info.bootloader_rev = Modbus.IspVer;

//	Setting_Info.reg.com_config[0] = Modbus.com_config[0];
//	Setting_Info.reg.com_config[1] = Modbus.com_config[1];
//	Setting_Info.reg.com_config[2] = Modbus.com_config[2];

//	Setting_Info.reg.refresh_flash_timer = Modbus.refresh_flash_timer;
//	Setting_Info.reg.en_plug_n_play = Modbus.external_nodes_plug_and_play;

////	Setting_Info.reg.clear_tstat_db = Modbus.clear_tstat_db;
//	Setting_Info.reg.com_baudrate[0] = uart0_baudrate;
//	Setting_Info.reg.com_baudrate[1] = uart1_baudrate;
//	Setting_Info.reg.com_baudrate[2] = uart2_baudrate;

	Setting_Info.reg.panel_number	= Station_NUM;
	Setting_Info.reg.en_panel_name = 1;
	memcpy(Setting_Info.reg.panel_name,panelname,20);
	Panel_Info.reg.product_type = 32 ;
	#endif
//#if  defined(MINI)
//	if(Modbus.mini_type == MINI_BIG)
//	{
//		
//			Panel_Info.reg.product_type = PRODUCT_MINI_BIG;
//	}
//	else
//	{
//		Panel_Info.reg.product_type = PRODUCT_MINI_SMALL;
//	}
//#else 
////	Panel_Info.reg.product_type = PRODUCT_CM5;
//#endif

//	flag_Moniter_changed = 0;
//	count_monitor_changed = 0;
}




void update_timers( void )
{
	int i, year;

//	year = RTC.Clk.year;
//	if( ( year & '\x03' ) == '\x0' )
//		month_length[1] = 29;
//	else
//		month_length[1] = 28;

	/* seconds since the beginning of the day */

   

//	ora_current_sec = 3600L * RTC.Clk.hour;
//	ora_current_sec += 60 * RTC.Clk.min;
//	ora_current_sec += RTC.Clk.sec;

//	RTC.Clk.day_of_year = 0;
//	for( i=0; i<RTC.Clk.mon - 1; i++ )
//	{
//		RTC.Clk.day_of_year += month_length[i];
//	}
//	RTC.Clk.day_of_year += ( RTC.Clk.day - 1 );

///*	timestart = 0;*/ /* seconds since the beginning of the year */
//	timestart = 86400L * RTC.Clk.day_of_year; /* 86400L = 3600L * 24;*/
//	timestart += ora_current_sec;

//	time_since_1970 = 0; /* seconds since 1970 */
//	if( RTC.Clk.year < 70 )
//		year = 100 + RTC.Clk.year;
	for( i = 70; i<year; i++ )
	{
		time_since_1970 += 31536000L;
		if( !( i & 3 ) )
			time_since_1970 += 86400L;
	}
  	time_since_1970 += timestart;
	time_since_1970 -= (3600 * 8);

	time_since_1970 -= 1000; // ?????????????????????

	timestart = 0; /* seconds since the beginning */

}


void Bacnet_Initial_Data(void)
{
	init_panel(); // for test now
}

#if READ_WRITE_PROPERTY 

void Read_bacnet_value_from_buf(void)
{
	U8_T max_ins;
	U8_T max_aos;
	U8_T max_dos;
//	U8_T index;
	U8_T i;
	U8_T priority;	
		//if((AV_Present_Value[0] == Modbus.address) || (AV_Present_Value[0] == 0)) // internal AV, DI ,AV ...
	if(1)	
	{
		
//			if(Modbus.mini_type == BIG)
//			{
//				max_ins = BIG_MAX_AIS;
//			    max_dos = BIG_MAX_DOS;
//				max_aos	= BIG_MAX_AOS;				
//			}
//			else if(Modbus.mini_type == SMALL)
//			{
//				max_ins = SMALL_MAX_AIS;
//			    max_dos = SMALL_MAX_DOS;
//				max_aos	= SMALL_MAX_AOS;			
//			}
//			else if(Modbus.mini_type == TINY)
//			{
//				max_ins = TINY_MAX_AIS;
//			    max_dos = TINY_MAX_DOS;
//				max_aos	= TINY_MAX_AOS;			
//			}
		// AV
//		   	for(i = 1;i < MAX_AVS;i++)
//			{
//				AV_Present_Value[i] = swap_double(vars[i].value) / 100; 
//			}

		// AI 			
			for(i = 0;i < max_ins;i++)
			{
				
				if(inputs[i].range > 0)
					AI_Present_Value[i] = swap_double(inputs[i].value) / 100;
				else
					AI_Present_Value[i] = input_raw[i];

			}
		// AO
			for(i = 0;i < max_aos;i++)
			{
				if(outputs[i + max_dos].auto_manual == 0)
					priority = 15; // AUTO
				else
					priority = 7; // manual

				if(outputs[i + max_dos].range > 0)
					AO_Present_Value[i][priority] = swap_double(outputs[i + max_dos].value)/ 100;
				else
					AO_Present_Value[i][priority] = output_raw[i + max_dos];
			}

		// BO
			for(i = 0;i < max_dos;i++)
			{	
				if(outputs[i].auto_manual == 0)
					priority = 15; // AUTO
				else
					priority = 7; // manual

				if(outputs[i].range > 0)
					BO_Present_Value[i][priority] = outputs[i].control;
			}
			
		}
}


void wirte_bacnet_value_to_buf(uint8_t type,uint8_t priority,uint8_t i)
{
//	U8_T index;
//	U8_T max_dos;
//   	if(Modbus.mini_type == BIG)
//	{
//	    max_dos = BIG_MAX_DOS;
//	}
//	else if(Modbus.mini_type == SMALL)
//	{
//	    max_dos = SMALL_MAX_DOS;
//	}
//	else if(Modbus.mini_type == TINY)
//	{
//	    max_dos = TINY_MAX_DOS;
//	}
//#if 0
//	for(index = 0;index < sub_no;index++)
//	{
//		if(AV_Present_Value[0] == tst_info[index].tst_db.id)
//			break;
//	}
//#endif
//	if((AV_Present_Value[0] == Modbus.address) ||(AV_Present_Value[0] == 0) ) // internal AV, DI ,AV ...
	if(1)
	{
		switch(type)
		{
			case AV:
//				if(priority <= 8)	// manual
//					vars[i].auto_manual = 1;	
//				else 	// auto
//					vars[i].auto_manual = 0;
//												
//				if(vars[i].auto_manual == 1)
//					vars[i].value = swap_double(AV_Present_Value[i]);

				break;
			case AI:
				if(priority <= 8)	// manual
					inputs[i].auto_manual = 1;	
				else // auto
					inputs[i].auto_manual = 0;
						
				if(inputs[i].auto_manual == 1)
					inputs[i].value = swap_double(AI_Present_Value[i]);

				break;
			case BO:
				if(priority <= 8)	// manual
					outputs[i].auto_manual = 1;	
				else 	// auto
					outputs[i].auto_manual = 0;
					
				if(outputs[i].auto_manual == 1)
					outputs[i].control = BO_Present_Value[i][priority];
				break;
			case AO:				
//				if(priority <= 8)	// manual
//					outputs[i + max_dos].auto_manual = 1;	
//				else 	// auto
//					outputs[i + max_dos].auto_manual = 0;
//				if(outputs[i + max_dos].auto_manual == 1)
//				{
//					output_raw[i + max_dos] = AO_Present_Value[i][outputs[i + max_dos].auto_manual];
//				}
				break;
	
			default:
			break;
		}
			
	}
}
#endif
