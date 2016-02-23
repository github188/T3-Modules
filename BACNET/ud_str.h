#ifndef UD_STR_H
#define UD_STR_H

#include <stdint.h>
#include <stdbool.h>
#include "types.h"
//#include "flash_schedule.h"
//#include "alarm.h"
//#include "product.h"
//#include "COMMSUB.H"   // for add tstat table


#define MAXFRAMEBUFFER			      490 
#define MAX_SEND_FRAMES             1//5



#define MAX_MON_ELEMENT  100

#define FLASH_BUFFER_LENGTH    2048//8192 //  8k 10000

#define  CODE_ELEMENT	400
#define  MAX_IO_POINTS	MAX_AI_CHANNEL
#define  MAX_VARS		64 





#define MAX_MONITORS           5//16

#define MAX_TOTALIZERS         2  /* MAX_IO_POINTS */

#define MAX_ELEMENTS        240    /* total number of group element allowed */

#define MAX_GRPS               16
#define MAX_ICONS              16
#define MAX_WR                  8
#define MAX_SCHEDULES_PER_WEEK  9
#define MAX_INTERVALS_PER_DAY	 4
#define MAX_AR                  4
#define AR_DATES_SIZE          46
#define MAX_PRGS               16
#define MAX_TBLS                5
//#define PROGRAMS_POOL_SIZE 		0x2800  // 640 * 16
#define MAX_ALARMS             16
#define MAX_ALARMS_SET         16
#define MAX_ARRAYS             16
#define MAX_ARRAYS_DATA		 2048

#define MAX_DIG_UNIT               8
#define MAX_INFO_TYPE		       20
#define MAX_VIEWS		            3
#define MAX_PASSW		            8
#define MAX_STATIONS           32
#define MAXREMOTEPOINTS		   64//30
#define MAX_REMOTE_POINTS      30
#define MAXNETWORKPOINTS       32
#define TABLE_BANK_LENGTH      15

#define MON_BUFFER_SIZE         FLASH_BUFFER_LENGTH
#define MAX_POINTS_IN_MONITOR   14
#define MAX_MONITOR_POINTS      MAX_MONITORS * MAX_POINTS_IN_MONITOR

#define MAX_ANALOG_SAMPLES_PER_BLOCK  90
#define MAX_DIGITAL_SAMPLES_PER_BLOCK 72

#define MAX_PRO_TIMER   30

//#define NAME_SIZE 17

typedef enum
	{
		OUT=0, IN, VAR, CON, WRT, AR, PRG, TBL, TZ, AMON, GRP, ARRAY, ALARMM = 12,
		UNIT, USER_NAME, ALARM_SET = 15, WR_TIME, AR_DATA, TSTAT, GRP_POINT, MAX_POINT_TYPE
	}	Point_type_equate;


typedef enum { 
		 READOUTPUT_T3000          = OUT+1,  /* read outputs */
		 READINPUT_T3000           = IN+1,   /* read inputs  */
		 READVARIABLE_T3000        = VAR+1,        /* read variables*/
		 READCONTROLLER_T3000      = CON+1,        /* read controllers*/
		 READWEEKLYROUTINE_T3000   = WRT+1,         /* read weekly routines*/
		 READANNUALROUTINE_T3000   = AR+1,         /* read annual routines*/
		 READPROGRAM_T3000         = PRG+1,        /* read programs       */
		 READTABLE_T3000           = TBL+1,        /* read tables         */
         READTOTALIZER_T3000       = TZ+1,         /* read totalizers     */
		 READMONITOR_T3000         = AMON+1,       /* read monitors       */
		 READSCREEN_T3000          = GRP+1,        /* read screens        */
		 READARRAY_T3000           = ARRAY+1,      /* read arrays         */
		 READALARM_T3000		   = ALARMM+1,       /* read alarm         */
		 READUNIT_T3000			   = UNIT+1,		 /* read UNIT         */
		 READUSER_T3000			   = USER_NAME + 1,
//		 READARRAYVALUE_T3000      = AYVALUE+1,    /* read array elements */

		 READPROGRAMCODE_T3000     = 16,           /* read program code   */
		 READTIMESCHEDULE_T3000    = WR_TIME+1,    /* read time schedule  */
		 READANNUALSCHEDULE_T3000  = AR_DATA+1,    /* read annual schedule*/
		 READ_AT_CMD = 90,

		 READPOINTINFOTABLE_T3000  = 24,           /* read pointinfo table*/
		 UPDATEMEMMONITOR_T3000    = 23,           /* read monitor updates*/
		 READMONITORDATA_T3000     = 22,           /* read monitor data   */
		 READINDIVIDUALPOINT_T3000 = 20,           /* read individual point*/
		 READGROUPELEMENT_T3000    = 25,           /* read point info      */
		 TIME_COMMAND              = 21,           /* read time            */
		 CLEARPANEL_T3000          = 28,           /* clear panel          */
		 SEND_ALARM_COMMAND        = 32,
		 READTSTAT_T3000		   = 33,		  // add by chelsea
		 READGROUPELEMENTS_T3000   = 91,           /* read group elements */
		 READREMOTEPOINT		   = 40,			/* read remote point */

		 WRITEOUTPUT_T3000         = 100+OUT+1,  /* write outputs          */
		 WRITEINPUT_T3000          = 100+IN+1,   /* write inputs           */
		 WRITEVARIABLE_T3000       = 100+VAR+1,        /* write variables  */
		 WRITECONTROLLER_T3000     = 100+CON+1,        /* write controllers*/
		 WRITEWEEKLYROUTINE_T3000  = 100+WRT+1,         /* write weekly routines*/
		 WRITEANNUALROUTINE_T3000  = 100+AR+1,         /* write annual routines*/
		 WRITEPROGRAM_T3000        = 100+PRG+1,        /* write programs       */
		 WRITETABLE_T3000          = 100+TBL+1,        /* write tables         */
     	 WRITETOTALIZER_T3000      = 100+TZ+1,         /* write totalizers     */
		 WRITEMONITOR_T3000        = 100+AMON+1,       /* write monitors       */
		 WRITESCREEN_T3000         = 100+GRP+1,        /* write screens        */
		 WRITEARRAY_T3000          = 100+ARRAY+1,      /* write arrays         */
		 WRITEALARM_T3000		   = 100+ALARMM+1,       /* write alarm         */
		 WRITEUNIT_T3000		   = 100+UNIT+1,		 /* write UNIT         */
		 WRITEUSER_T3000		   = 100+USER_NAME + 1,
		 WRITETIMESCHEDULE_T3000   = 100+WR_TIME+1,    /* write time schedule  */
		 WRITEANNUALSCHEDULE_T3000 = 100+AR_DATA+1,    /* write annual schedule*/
		 WRITEPROGRAMCODE_T3000    = 100+16,           /* write program code    */
		 WRITEINDIVIDUALPOINT_T3000 = 100+READINDIVIDUALPOINT_T3000, /* write individual point*/
		 WRITETSTAT_T3000		   = 133,		   // add by chelsea
		 WRITEGROUPELEMENTS_T3000   = 191,           /* write group elements */
		 WRITE_AT_CMD = 190,
		 WRITEREMOTEPOINT		   = 140,			/* write remote point */

	#if 0
		 COMMAND_50                = 50,
		 READ_COMMAND_50           = 50,
		 WRITE_COMMAND_50          = 150,
		 STATION_LIST_COMMAND      = 21,
		 SAVEPROGRAM_COMMAND       = 30,
		 LOADPROGRAM_COMMAND       = 31,
		 DEFAULT_PRG_COMMAND       = 32, 
		 READFILES_COMMAND         = 40,  /* read the files of type define in*/
																					/* a previous SETFILETYPE_COMMAND command*/
																					/* from the current path.                */
																					/* The data returned are an array of type*/
																					/*  int8_t [13];                           */
		 READDIRECTORIES_COMMAND   = 41,  /* read the directories name from            */
																					/* the current path.                     */
																					/* The data returned are of type         */
																					/*  (*int8_t)[13];                         */
		 GETCURRENTPATH_COMMAND    = 42,  /* get the current path                      */
																					/* The command is a read command:        */
																					/*  command  = READ_COMMAND_50           */
																					/*  arg      = GETCURRENTPATH_COMMAND    */
		 SETCURRENTPATH_COMMAND    = 43,  /* set the current path for the subsequent   */
																					/* read directories and read files command*/
																					/* The command is a write command:        */
																					/*  command  = WRITE_COMMAND_50           */
																					/*  arg      = SETCURRENTPATH_COMMAND     */
		 SETFILETYPE_COMMAND       = 44,  /* set the file type (ex. "*.*", "*.prg").    */
																					/* the next read files command will       */
																					/* return only the files of type          */
																					/* set in a SETFILETYPE_COMMAND command.  */
																					/* The command is a write command:        */
																					/*  command  = WRITE_COMMAND_50           */
																					/*  arg      = SETFILETYPE_COMMAND        */
		 ALARM_NOTIFY_COMMAND       = 51,
		 SEND_INFO_COMMAND          = 52,
		 SEND_WANTPOINTS_COMMAND    = 72,
		 SEND_NETWORKPOINTS_COMMAND = 73,


		 TABLEPOINTS_COMMAND       = 75,
		 PANEL_INFO1_COMMAND       = 110,
		 PANEL_INFO2_COMMAND       = 111,
		 MINICOMMINFO_COMMAND      = 112,
		 PANELID_COMMAND           = 113,
		 ICON_NAME_TABLE_COMMAND   = 114,
		 WRITEDATAMINI_COMMAND     = 116,
		 SENDCODEMINI_COMMAND      = 117,
		 SENDDATAMINI_COMMAND      = 118,
		 READFLASHSTATUS_COMMAND   = 119,
		 READSTATUSWRITEFLASH_COMMAND = 120,
	#endif		
		RESTARTMINI_COMMAND       = 121,
		 WRITEPRGFLASH_COMMAND     = 122,
		 OPENSCREEN_COMMAND        = 123,


		 GET_PANEL_INFO			 = 99,
		 READ_SETTING			 = 98,
		 READ_TSTAT_DB			 = 97,
 		 WRITE_SETTING	= 198,
		 WRITE_SUB_ID_BY_HAND = 199,


} CommandRequest;

typedef enum { OUTPUT=1, INPUT, VARIABLE, CONTROLLER, WEEKLY_ROUTINE,
	ANNUAL_ROUTINE, PROGRAM, TABLES, TOTALIZER, ANALOG_MONITOR,
	CONTROL_GROUP, ARRAYS, ALARM, UNITs, USERS, COD, CONTROL_GROUP_DATA=19,
	AMON_DATA=22, DMON_DATA, MONITOR=40, DESCRIPTORS=47, ARRAYS2
	} Command_type_equate;

//typedef enum  {
//	unused, degC, degF, FPM, Pa, KPa, psi, in_w, Watts, KW, KWH,
//	Volts, KV, Amps, ma, CFM, Sec, Min, Hours, Days, time_unit, ohms,
//	procent, RH, ppm, counts,	Open, CFH, GPM, GPH, GAL, CF, BTU, CMH,
//	custom1, custom2, custom3, custom4, custom5, custom6, custom7, custom8
//	} Analog_units_equate;	



//typedef enum { UNUSED=0,
//	OFF_ON, CLOSED_OPEN, STOP_START, DISABLED_ENABLED,
//	NORMAL_ALARM, NORMAL_HIGH, NORMAL_LOW, NO_YES,
//	COOL_HEAT, UNOCCUPIED_OCCUPIED, LOW_HIGH,
//	ON_OFF , OPEN_CLOSED, START_STOP, ENABLED_DISABLED,
//	ALARM_NORMAL, HIGH_NORMAL, LOW_NORMAL, YES_NO,
//	HEAT_COOL, OCCUPIED_UNOCCUPIED, HIGH_LOW,
//	custom_digital1, custom_digital2, custom_digital3, custom_digital4,
//	custom_digital5, custom_digital6, custom_digital7, custom_digital8
//	} Digital_units_equate;


typedef enum { Y3K_40_150DegC=31, Y3K_40_300DegF, R10K_40_120DegC,
	R10K_40_250DegF, R3K_40_150DegC, R3K_40_300DegF, KM10K_40_120DegC,
	KM10K_40_250DegF, A10K_50_110DegC, A10K_60_200DegF, V0_5, I0_100Amps,
	I0_20ma, I0_20psi, N0_2_32counts, N0_3000FPM_0_10V, P0_100_0_5V,
	P0_100_4_20ma/*, P0_255p_min*/, V0_10_IN, table1, table2, table3, table4,
	table5, HI_spd_count} Analog_input_range_equate;	
	



typedef enum { not_used,KM_10K,I_4_20ma,V_0_10,V_0_5V,V_0_24AC,TST_Normal} Analog_input_new_range_equate;


typedef enum { not_used_output, V0_10, P0_100_Open, P0_20psi, P0_100,
						P0_100_Close, I_0_20ma }Analog_output_range_equate;


typedef enum { MINIPANEL,T5,T6,T7,T3 } PanelType;


typedef struct
{
	U8_T number;
	U8_T point_type;
}	Point;

typedef struct
	{
		U8_T 	panel_type;
		U8_T	active_panels[4];
		U16_T	desc_length;
		U16_T 	version_number;
		U8_T 	panel_number;
		S8_T 	panel_name[17];
		U16_T	network_number;
		S8_T	network_name[17];
	} Panel_Net_Info;   /* PanelInfo1 */

typedef struct {

/*	Point_Net point_name;  5 uint8_ts*/

	U8_T number;
	U8_T point_type;
	U8_T panel;
	S16_T	network_number;
	S32_T point_value;
	U8_T auto_manual;  	 /* 0=auto, 1=manual*/
	U8_T digital_analog;  /* 0=digital, 1=analog*/
	U8_T description_label;  /* 0=display description, 1=display label*/
	U8_T security	  ;  /* 0-3 correspond to 2-5 access level*/
	U8_T decomisioned;  /* 0=normal, 1=point decommissioned*/

	U8_T units ;

} Point_info; 		/*  5+4+1+1=11*/


typedef struct
{
	uint8_t number;
	uint8_t point_type;
	uint8_t panel;
}	Point_T3000;

typedef struct
{
	uint8_t number;
	uint8_t point_type;	 // first 3 bit for number
	uint8_t panel;
	uint8_t sub_id;
	uint8_t	network_number;
}	Point_Net;

typedef struct
{
	int8_t prg;
	int8_t type;
	int32_t time;
	uint16_t count;
	int8_t used; 
	uint16_t last_milisec;
	char message[20];
}Str_pro_timer;

typedef struct
{
	int8_t description[21]; 	       /* (21 uint8_ts; string)*/
	int8_t label[9];		       /* (9 uint8_ts; string)*/

	int32_t value;		       /* (4 uint8_ts; int32_t) */

	int8_t auto_manual;  /* (1 bit; 0=auto, 1=manual)*/
	int8_t digital_analog;  /* (1 bit; 0=digital, 1=analog)*/
	int8_t switch_status/*access_level*/;  /* (3 bits; 0-5)*/
	int8_t control ;  /* (1 bit; 0=off, 1=on)*/
	int8_t digital_control;  /* (1 bit)*/
	int8_t decom;  /* (1 bit; 0=ok, 1=point decommissioned)*/
	int8_t range;	/* (1 uint8_t ; output_range_equate)*/

	uint8_t m_del_low;  /* (1 uint8_t ; if analog then low)*/
	uint8_t s_del_high; /* (1 uint8_t ; if analog then high)*/
	uint16_t count ;//delay_timer;      /* (2 uint8_ts;  seconds,minutes)*/

} Str_out_point;  /* 45 */

//typedef enum  
//{
//	e_out_description = 0, 	       /* (21 uint8_ts; string)*/
//	e_out_label = 21,		       /* (9 uint8_ts; string)*/
//
//	e_out_value = 30,		       /* (4 uint8_ts; int32_t) */
//
//	e_out_auto_manual = 34,  /* (1 bit; 0=auto, 1=manual)*/
//	e_out_digital_analog = 35,  /* (1 bit; 0=digital, 1=analog)*/
//	e_out_switch_status = 36,/*access_level*/ /* (3 bits; 0-5)*/
//	e_out_control  = 37,  /* (1 bit; 0=off, 1=on)*/
//	e_out_digital_control = 38,  /* (1 bit)*/
//	e_out_decom = 39,  /* (1 bit; 0=ok, 1=point decommissioned)*/
//	e_out_range = 40,	/* (1 uint8_t ; output_range_equate)*/
//
//	e_out_m_del_low = 41,  /* (1 uint8_t ; if analog then low)*/
//	e_out_s_del_high = 42, /* (1 uint8_t ; if analog then high)*/
//	e_out_delay_timer = 43,      /* (2 uint8_ts;  seconds,minutes)*/
//}modbus_output;

//
typedef enum
{
	IN_OK = 0,IN_NOUSE = 1,IN_SHORT
}IN_STATS;


typedef struct
{

	int8_t description[21]; 	      /* (21 uint8_ts; string)*/
	int8_t label[9];		      	/* (9 uint8_ts; string)*/

	int32_t value;		     						/* (4 uint8_ts; int32_t)*/

	int8_t  filter;  /* */
	int8_t decom;/* (1 bit; 0=ok, 1=point decommissioned)*/
	int8_t sen_on;/* (1 bit)*/
	int8_t sen_off;  /* (1 bit)*/
	int8_t control; /*  (1 bit; 0=OFF, 1=ON)*/
	int8_t auto_manual; /* (1 bit; 0=auto, 1=manual)*/
	int8_t digital_analog ; /* (1 bit; 1=analog, 0=digital)*/
	int8_t calibration_sign; /* (1 bit; sign 0=positiv 1=negative )*/
	int8_t calibration_increment; /* (1 bit;  0=0.1, 1=1.0)*/
	int8_t calibration_hi; /* (5 bits - spare )*/ 
	uint8_t calibration_lo;  /* (8 bits; -25.6 to 25.6 / -256 to 256 )*/

	uint8_t range;	      /* (1 uint8_t ; input_range_equate)*/

} Str_in_point; /* 21+1+4+1+1+9 = 38 */

//typedef enum  
//{
//	e_in_description = 0,	      /* (21 uint8_ts; string)*/
//	e_in_label = 21,		      	/* (9 uint8_ts; string)*/
//	e_in_value = 30,		     						/* (4 uint8_ts; int32_t)*/
//	e_in_filter = 34,  /* (3 bits; 0=1,1=2,2=4,3=8,4=16,5=32, 6=64,7=128,)*/
//	e_in_decom = 35,/* (1 bit; 0=ok, 1=point decommissioned)*/
//	e_in_sen_on = 36,/* (1 bit)*/
//	e_in_sen_off = 37,  /* (1 bit)*/
//	e_in_control = 38, /*  (1 bit; 0=OFF, 1=ON)*/
//	e_in_auto_manua = 39, /* (1 bit; 0=auto, 1=manual)*/
//	e_in_digital_analog = 40 , /* (1 bit; 1=analog, 0=digital)*/
//	e_in_calibration_sign = 41, /* (1 bit; sign 0=positiv 1=negative )*/
//	e_in_calibration_increment = 42, /* (1 bit;  0=0.1, 1=1.0)*/
//	e_in_unused = 43, /* (5 bits - spare )*/
//	e_in_calibration = 44,  /* (8 bits; -25.6 to 25.6 / -256 to 256 )*/
//	e_in_range  = 45
//}modbus_input;

typedef struct
{
	int8_t description[21];	      /*  (21 uint8_ts; string)*/
	int8_t label[9];		      /*  (9 uint8_ts; string)*/

	int32_t value;		      /*  (4 uint8_ts; float)*/

	uint8_t auto_manual;  /*  (1 bit; 0=auto, 1=manual)*/
	uint8_t digital_analog;  /*  (1 bit; 1=analog, 0=digital)*/
	uint8_t control	;
	uint8_t unused	;
	uint8_t range ; /*  (1 uint8_t ; variable_range_equate)*/

	
}	Str_variable_point; /* 21+9+4+1+1 = 36*/


//typedef struct
//{
//	Point_T3000 input;	        /* (3 uint8_ts; point)*/
//	int32_t input_value; 	        /* (4 uint8_ts; int32_t)*/
//	int32_t value;		              /* (4 uint8_ts; int32_t)*/
//	Point_T3000 setpoint;	      /* (3 uint8_ts; point)*/
//	int32_t setpoint_value;	      /* (4 uint8_ts; float)*/
//	uint8_t units;    /* (1 uint8_t ; Analog_units_equate)*/

//	uint8_t auto_manual; /* (1 bit; 0=auto, 1=manual)*/
//	uint8_t action; /* (1 bit; 0=direct, 1=reverse)*/
//	uint8_t repeats_per_min; /* (1 bit; 0=repeats/hour,1=repeats/min)*/
//	uint8_t sample_time; /* (1 bit)*/
//	uint8_t prop_high; /* (4 bits; high 4 bits of proportional bad)*/

//	uint8_t proportional;

//	uint8_t reset;	      /* (1 uint8_t ; 0-255)*/
//	uint8_t bias;	      /* (1 uint8_t ; 0-100)*/
//	uint8_t rate;	      /* (1 uint8_t ; 0-2.00)*/

//}	Str_controller_point; /* 3+4+4+3+4+1+1+4 = 24*/

typedef struct {
	int32_t	old_err;
	int32_t 	error_area;
	int32_t 	oi;
	}	Con_aux;

//typedef struct
//{
//	int8_t description[21];		     /* (21 uint8_ts; string)*/
//	int8_t label[9];		      	     /*	(9 uint8_ts; string)*/

//	uint8_t value ;  /* (1 bit; 0=off, 1=on)*/
//	uint8_t auto_manual;  /* (1 bit; 0=auto, 1=manual)*/
//	uint8_t override_1_value;  /* (1 bit; 0=off, 1=on)*/
//	uint8_t override_2_value;  /* (1 bit; 0=off, 1=on)*/
//	uint8_t off  ;
//	uint8_t unused	; /* (11 bits)*/

//	Point_T3000 override_1;	     /* (3 uint8_ts; point)*/
//	Point_T3000 override_2;	     /* (3 uint8_ts; point)*/

//} Str_weekly_routine_point; /* 21+9+2+3+3 = 38*/

 enum
{
	e_week_description = 0,		     /* (21 uint8_ts; string)*/
	e_week_label = 21,		      	     /*	(9 uint8_ts; string)*/

	e_week_value = 30,  /* (1 bit; 0=off, 1=on)*/
	e_week_auto_manual = 31,  /* (1 bit; 0=auto, 1=manual)*/
	e_week_override_1_value = 32,  /* (1 bit; 0=off, 1=on)*/
	e_week_override_2_value = 33,  /* (1 bit; 0=off, 1=on)*/
	e_week_off = 34,
	e_week_unused = 35, /* (11 bits)*/

	e_week_override_1 = 36,	     /* (3 uint8_ts; point)*/
	e_week_override_2 = 39	     /* (3 uint8_ts; point)*/

}; 


typedef struct
{
	uint8_t	minutes;		/* (1 uint8_t ; 0-59)	*/
	uint8_t	hours; 		/* (1 uint8_t ; 0-23)	*/

} Time_on_off;				/* (size = 2 uint8_ts)	*/

typedef struct
{
	Time_on_off	time[2*MAX_INTERVALS_PER_DAY];

} Wr_one_day;		/* (size = 16 uint8_ts)	*/


typedef struct
{
	int8_t description[21]; 	    /* (21 uint8_ts; string)*/
	int8_t label[9];		      		/* (9 uint8_ts; string)*/

	uint8_t value		;  /* (1 bit; 0=off, 1=on)*/
	uint8_t auto_manual;/* (1 bit; 0=auto, 1=manual)*/
//	U8_T unused				: 14; 	/* ( 12 bits)*/
	uint8_t unused;

}	Str_annual_routine_point;   /* 21+9+2=32 uint8_ts*/

typedef enum
{
	e_ar_description = 0, 	    /* (21 uint8_ts; string)*/
	e_ar_label = 21,		      		/* (9 uint8_ts; string)*/

	e_ar_value = 30,  /* (1 bit; 0=off, 1=on)*/
	e_ar_auto_manual = 31,/* (1 bit; 0=auto, 1=manual)*/
//	U8_T unused				: 14; 	/* ( 12 bits)*/
	e_ar_unused = 32

} STR;   /* 21+9+2=32 uint8_ts*/

//typedef struct
//{
//	int8_t description[21]; 	      	  /* (21 uint8_ts; string)*/
//	int8_t label[9];			  /* (9 uint8_ts; string)*/  
//	uint16_t bytes;		/* (2 uint8_ts; size in uint8_ts of program)*/ 
//	uint8_t on_off;	//	      : 1; /* (1 bit; 0=off; 1=on)*/
//	uint8_t auto_manual;//	  : 1; /* (1 bit; 0=auto; 1=manual)*/
//	uint8_t com_prg;	//	    : 1; /* (6 bits; 0=normal use, 1=com program)*/
//	uint16_t real_byte;   //      : 8;

//} Str_program_point;	  /* 21+9+2+2 = 34 uint8_ts*/


typedef struct
{
	S8_T view_name[11];		/**/
	U8_T on_off;						/**/
	S32_T timerange;				/**/
}	Views;  			/* 11+1+4=16*/

typedef struct              /* 5 uint8_ts */
{
	U8_T pointno_and_value;    // bit0-bit6 point_no     value bit7
//	U8_T unused ;
//	U8_T value;

	U32_T          time;

} Digital_sample;        /* 5 uint8_ts */

//typedef struct
//{
//	S8_T label[9];		      	  					/* 9 uint8_ts; string */

//	Point_Net 	inputs[MAX_POINTS_IN_MONITOR];	/* 70 uint8_ts; array of Point_Net */
//	U8_T		range[MAX_POINTS_IN_MONITOR]; /* 14 uint8_ts */

//	U8_T second_interval_time; 				/* 1 uint8_t ; 0-59 */
//	U8_T minute_interval_time; 				/* 1 uint8_t ; 0-59 */
//	U8_T hour_interval_time;   				/* 1 uint8_t ; 0-255 */

//	U8_T max_time_length;      /* the length of the monitor in time units */

////	Views views[MAX_VIEWS];			/* 16 x MAX_VIEWS uint8_ts */

//	U8_T num_inputs  ;// :4; 	/* total number of points */
//	U8_T an_inputs ;//   :4; 	/* number of analog points */
//	U8_T unit 		;//		:2; 	/* 2 bits - minutes=0, hours=1, days=2	*/
////	U8_T ind_views	;//	:2; 	/* number of views */
//	U8_T wrap_flag	;//	:1;		/* (1 bit ; 0=no wrap, 1=data wrapped)*/
//	U8_T status		;//		:1;		/* monitor status 0=OFF / 1=ON */
//	U8_T reset_flag	;//	:1; 	/* 1 bit; 0=no reset, 1=reset	*/
//	U8_T double_flag;//	:1; 	/* 1 bit; 0= 4 uint8_ts data, 1= 1(2) uint8_ts data */

//}	Str_monitor_point; 		/* 9+70+14+3+1+48+2 = 133 uint8_ts */


//typedef struct
//{
//	U32_T next_sample_time;  	/* 4 uint8_ts of time type - used only for analog points*/
//	U8_T first_analog_block;
//	U8_T  current_analog_block;
//	U8_T  no_analog_blocks;
//	U8_T first_digital_block;
//	U8_T  current_digital_block;
//	U8_T  no_digital_blocks;

//  	U8_T  no_of_digital_points;
//	U8_T priority  ;//  :2; /* 1-low, 2-medium, 3-high */
//	U8_T start 	  ;//  :1; /* 1 bit	*/
//	U8_T saved	  ;//    :1; /* 1 bit	*/
//	U8_T active	 ;//   :1; /* 1 bit - 0=inactive, 1=active	*/

//	
//}Mon_aux; 		/* 4++3+1 = 8 uint8_ts */

//typedef struct              /* 645 uint8_ts */
//{
//	Point_Net     inputs[MAX_POINTS_IN_MONITOR]; /* 70 uint8_ts; array of Point_Net */

//	U8_T monitor;//	       :4; /* monitors' number */
//	U8_T no_points  ;//     :4; /* number of points in block */

//	U8_T second_interval_time; /* 1 U8_T ; 0-59 */
//	U8_T minute_interval_time; /* 1 U8_T ; 0-59 */
//	U8_T hour_interval_time;   /* 1 U8_T ; 0-255 */

//	U8_T priority	 ;//     :2; /* 0-block empty, 1-low, 2-medium, 3-high */
//	U8_T first_block  ;//   :1; /* 1 - this block is the first in the chain */
//	U8_T last_block  ;//    :1; /* 1 - this block is the last in the chain */
//	U8_T analog_digital;//  :1; /* 0 - analog, 1 - digital */
//	U8_T block_state ;//    :1; /* 0 = unused, 1 = used */
//	U8_T fast_sampling;//   :1; /* 0 = normal sampling 1 = fast sampling */
//	U8_T wrap_around ;//    :1; /* 1 - wrapped  */

//// added by chelsea
//   	U16_T    send_length;
//	U8_T   	 no_used2[6];


//	U32_T          start_time;

//	U16_T          index;      /* pointer to the new free location in block */
//														/* equal with the number of samples in block */
//	U8_T          next_block; /* pointer to the next block in chain
//	                              255 = last block in chain */
//	U8_T          block_no;      /* position of block in chain */
//  	U8_T      last_digital_state ;//: 14;
///*  U8_T      index_fast         :  2;*/
//	U8_T   	 no_used;

//	U32_T    dat[MAX_MON_ELEMENT];
////  	union {
////	U32_T           analog[MAX_ANALOG_SAMPLES_PER_BLOCK];   /* 140*4=560 uint8_ts */
////  	Digital_sample digital[MAX_DIGITAL_SAMPLES_PER_BLOCK]; /* 112*5=560 uint8_ts */
////  	U8_T           raw_byte[360];
////  	U16_T           raw_int[180];
////  	} dat;
//}	Monitor_Block;         /* 660uint8_ts */


typedef struct
{
	U8_T index;
	U32_T time;
	Point_Net point;
	U32_T value;
	U16_T mark;  // 0d 0a  end mark

}Str_mon_element;	// 16

//typedef struct              /* 85 uint8_ts */
//{
//Point_Net     inputs[MAX_POINTS_IN_MONITOR]; /* 70 uint8_ts; array of Point_Net */

//U8_T monitor;//	       :4; /* monitors' number */
//U8_T no_points ;//      :4; /* number of points in block */
///*	U8_T tenths_of_seconds    : 4;  4 bits ; 0-15 */
///*	U8_T second_interval_time : 6;  6 bits ; 0-59 */
///*	U8_T minute_interval_time : 6;  6 bits ; 0-59 */

//U8_T second_interval_time; /* 1 U8_T ; 0-59 */
//U8_T minute_interval_time; /* 1 U8_T ; 0-59 */
//U8_T hour_interval_time;   /* 1 U8_T ; 0-255 */

//U8_T priority	  ;//     :2; /* 0-block empty, 1-low, 2-medium, 3-high */
//U8_T first_block  ;//   :1; /* 1 - this block is the first in the chain */
//U8_T last_block    ;//  :1; /* 1 - this block is the last in the chain */
//U8_T analog_digital;//  :1; /* 0 - analog, 1 - digital */
//U8_T block_state ;//    :1; /* 0 = unused, 1 = used */
//U8_T fast_sampling;//   :1; /* 0 = normal sampling 1 = fast sampling */
//U8_T wrap_around  ;//   :1; /* 1 - wrapped  */

//// added by chelsea
//	U16_T    send_length;
//U8_T   	 no_used2[6];


//S32_T          start_time;

//U16_T          index;      /* pointer to the new free location in block */
//													/* equal with the number of samples in block */
//U8_T          next_block; /* pointer to the next block in chain
//															255 = last block in chain */
//U8_T          block_no;      /* position of block in chain */


//	U8_T      last_digital_state ;//: 14;
//	U8_T     no_used       ;//    :  2;
///*  U8_T      index_fast         :  2;*/

//} Monitor_Block_Header;  /* 100 uint8_ts */


//typedef struct
//{
//	U8_T grpsize;       		/* 0-6 inputs per analog monitor*/
//	U8_T number_of_mon; 		/* 0-99 monitors with this definition*/
//	U8_T length;        		/* 0-255 data elements*/


//	U8_T digital_analog ;//:1;  /* 0-analog monitor, 1-digital monitor*/
//	U8_T unused		;//	    :3;
//	U8_T two_or_4_uint8_ts ;//:1;  /* 0=2 uint8_ts per analog sample, 1=4 uint8_ts*/
//	U8_T not_used	;//		  :3;

//	U8_T flag;

//} Monitor_element;		/* 3+1=4 definition used in Burke systems */
/**/

typedef struct
{
	S8_T label[9];		       /* point label: 9 uint8_ts */

	Point_Net point;         /* point to be monitored */
	S32_T start_time;         /* moment the totalizer was started - sec since 1970 */
	S32_T value_timeon;       /* analog : totalized value digital: total number of seconds the point was ON */
	S32_T time_of_last_check; /* digital: time the totalizer was checked */

	S32_T ratecount_OFF_ON_transitions; /* digital: number of transitions from OFF to ON since start_time */
																		 /* analog : number of samples in totalized value */
	S32_T value1;             /* analog: totalized value per rate */
	S32_T count;              /* analog: number of samples within rate */
	U8_T units;              /* (1 U8_T ; Analog_units_equate)*/

	U8_T digital_analog  ;//  : 1;   /* 0-digital monitor, 1-analog monitor */
	U8_T active         ;//   : 1;  /* 1=ACTIVE, 0=INACTIVE */
	U8_T reset          ;//   : 1;  /* 1=RESET, 0=DON'T CARE */
	U8_T last_state     ;//   : 1;  /* 1=ON, 0=OFF */
	U8_T rate          ;//    : 2;
	U8_T unused       ;//     : 2;

} Str_totalizer_point;  /* 9+5+4+4+4+4+1+2=33 uint8_ts */


//typedef struct
//{
//	S8_T description[21];				/* (21 uint8_ts; string)	*/
//	S8_T label[9];							/* (9 uint8_ts; string)	*/
//	S8_T picture_file[11];			/* (11 uint8_ts; string)	*/

//	U8_T update;                /* refresh time */
//	U8_T  mode     ;// :1;     /* text / graphic */
//	U8_T  xcur_grp	;//:15;

//	S16_T  ycur_grp;

//} Control_group_point;				/* (size = 46 uint8_ts)	*/

//typedef struct
//{
//	U8_T 	no_elements;				/* 8 bits ; no of elements in the group	*/
//	Str_grp_element	*address; /* pointers to each group */
//
//} Aux_group_point;


typedef struct {
	U8_T	filter_last;	/* last value of the filter field */
	U32_T	filter_sum;
	U16_T	average;      /* the filtered value */
	U16_T	last;         /* the previous sample - unfiltered */
	S16_T 	ticks;        /* pulses counted since last cleared */
}In_aux;



typedef struct {
	S8_T *address;
	U8_T size;
	U8_T max_points;
	U8_T label;
	U8_T desc;
	} Info_Table;

//typedef struct {

//	Point_Net	point;

//	U8_T 	modem  ;//   	  : 1;
//	U8_T 	printer	;//	    : 1;
//	U8_T 	alarm	;//		    : 1;
//	U8_T 	restored     ;// : 1;
//	U8_T 	acknowledged ;// : 1;
//	U8_T	ddelete		 ;//   : 1;
//	U8_T  type        ;//  : 2; /* DDDD */
//	U8_T  cond_type   ;//  : 4;
//	U8_T  level       ;//  : 4; /* DDDD */

//	U32_T 		alarm_time;
//	char 	  alarm_count;
//	//char     alarm_message[ALARM_MESSAGE_SIZE+1];
//	char              none[5];
//	U8_T panel_type   ;//  :4;
//	U8_T dest_panel_type;//:4;
//	U16_T      alarm_id;
//	uint8_t              prg;

//	//uint8_t alarm_panel;   /* (1 uint8_t ; 1-32, panel alarm originated from)	*/
//	uint8_t where1;	      /* (1 uint8_t ; panel# to send alarm to, 255 = all)	*/
//	uint8_t where2;	      /* (1 uint8_t ; panel# to send alarm to, 0 = none)	*/
//	uint8_t where3;	      /* (1 uint8_t ; panel# to send alarm to, 0 = none)	*/
//	uint8_t where4;	      /* (1 uint8_t ; panel# to send alarm to, 0 = none)	*/
//	uint8_t where5;	      /* (1 uint8_t ; panel# to send alarm to, 0 = none)	*/

//	U8_T where_state1 ;// :1;	      /* (1 uint8_t ; panel# to send alarm to, 255 = all)	*/
//	U8_T where_state2 ;// :1;	      /* (1 uint8_t ; panel# to send alarm to, 255 = all)	*/
//	U8_T where_state3 ;// :1;	      /* (1 uint8_t ; panel# to send alarm to, 255 = all)	*/
//	U8_T where_state4 ;// :1;	      /* (1 uint8_t ; panel# to send alarm to, 255 = all)	*/
//	U8_T where_state5 ;// :1;	      /* (1 uint8_t ; panel# to send alarm to, 255 = all)	*/
//	U8_T change_flag ;//  :2;
//	U8_T original    ;//  :1;
//	U8_T no         ;//   :8;

//} Alarm_point; /* 88 uint8_ts */




typedef struct
{
	 Byte direct;
	 char digital_units_off[12];       /*12 bytes; string)*/
	 char digital_units_on[12];        /*12 bytes; string)*/

} Units_element;  

typedef struct
{
	char name[16]; 		       /* (16 bytes; string)	*/
	char password[9]; 		   /* (9 bytes; string)	*/
	U8_T access_level;    	   /* (1 byte ; 0-255)	*/
	unsigned long rights_access;
	U8_T default_panel;
	U8_T default_group;
	/* 2*((MAX_GRPS%8)?MAX_GRPS/8+1:MAX_GRPS/8) */
	char screen_right[8*1];
	/* 2*((MAX_PRGS%8)?MAX_PRGS/8+1:MAX_PRGS/8) */
	char program_right[8*1];

}	Password_point;		/* ( size = 52 bytes );	*/


typedef struct
{
//	U8_T			  	state; 

	Point_Net point;            /* 5 bytes*/
	U32_T point_value;

	U8_T auto_manual;//	     : 1;  /* 0=auto, 1=manual*/
	U8_T digital_analog	;//	   : 1;  /* 0=digital, 1=analog*/
//	U8_T description_label;//	 : 3;  /* 0=display description, 1=display label*/
//	U8_T security;//		         : 2;  /* 0-3 correspond to 2-5 access level*/
	U8_T decomisioned;//		     : 1;  /* 0=normal, 1=point decommissioned*/
	U8_T units  ;//	           : 8;
/*		Point_info		info; 11 bytes */
/*		***!!! - I replaced Point_info with it's content  */
	U8_T count;//	             : 8;
//	U8_T read;//					       : 2;
//	U8_T write;//					     : 2;
	U8_T read_write;//		       : 2;	   0 - read only 1- writen
	U8_T change	;//			       : 2;

}	REMOTE_POINTS; /* 1+5+4+2+2=14 bytes */


typedef struct
{
	Point_Net point; /* 5 bytes*/

	long point_value;

	U8_T auto_manual	;//     : 1; /* 0=auto, 1=manual */
	U8_T digital_analog	;//   : 1; /* 0=digital, 1=analog */
	U8_T description_label;// : 3; /* 0=display description, 1=display label */
//	U8_T security	         : 2; /* 0-3 correspond to 2-5 access level */
	U8_T decomisioned;//	     : 1; /* 0=normal, 1=point decommissioned */

	U8_T units   ;//         : 8;

}	NETWORK_POINTS;   /* 5+4+2 = 11 bytes */


#define STR_ICON_1_NAME_LENGTH 20
#define STR_ICON_2_NAME_LENGTH 20

typedef union
{
 uint8_t all[70];
 struct
 {
  uint8_t label_status;
  unsigned int nSerialNum;
  uint8_t nScreen_index;
  uint16_t nLabel_index;
  uint8_t nMain_Panel;
  uint8_t nSub_Panel;
  uint8_t nPoint_type;
  uint8_t nPoint_number;
  uint16_t  nPoint_x;
  uint16_t  nPoint_y;
  unsigned int nclrTxt;
  uint8_t nDisplay_Type;
  uint8_t nIcon_size;
  uint8_t nIcon_place;
  char icon_name_1[STR_ICON_1_NAME_LENGTH];
  char icon_name_2[STR_ICON_2_NAME_LENGTH];
 }reg;
} Str_grp_element;//Str_label_point;

typedef union {
		Str_out_point             *pout;
		Str_in_point 			  *pin;
	//	In_aux 					  *pinx;
		Str_variable_point 		  *pvar;
		//Str_controller_point 	  *pcon;
//		Str_weekly_routine_point  *pwr;
		Str_annual_routine_point  *panr;
//		Str_program_point 		  *pprg;
//		Str_array_point 		  *pary;
//		Str_monitor_point		  *pmon;
	    Str_totalizer_point       *ptot;
//	    Monitor_Block             *pmb;
//	    Mon_aux                   *pmaux;
//		Control_group_point       *pgrp;
//		Aux_group_point           *pgaux;
//		Str_grp_element           *pgrel;
//		Alarm_point               *palrm;
	//	MiniInfo2 				  *pmi2;
	//	MiniCommInfo			  *pmci;
		Point_Net		          *pnet;
		Point_info                *ppinf;
	//	System_Name_Number        *psnn;
	//	Program_remote_points     *pprp;
	//	Info_Table                *pinf;
		REMOTE_POINTS             *prp;
		NETWORK_POINTS            *pnp;
	//	WANT_POINTS               *pwp;
//		TST_INFO				  *ptst;
	 } Str_points_ptr;


typedef struct
{
	 uint16_t  		total_length;        /*	total length to be received or sent	*/
	 uint8_t		command;
	 uint8_t		point_start_instance;
	 uint8_t		point_end_instance;
	 uint16_t		entitysize;

}Str_user_data_header;


typedef	union
{
	uint8_t all[20];
	struct 
	{ 	
	 uint16_t instance;
	 uint8_t mac[6];
	 uint8_t serial_num[4];
	 uint8_t modbus_addr;
	 uint8_t product_type;
	 uint8_t panel_number;
	 uint16_t modbus_port;
	 uint16_t sw;  
	 uint8_t hw;

	 uint8_t protocal;//noused[1];
			
	}reg;
}Str_Panel_Info;



typedef struct
{
	uint8 harware_rev;
	uint16 firmware0_rev;	// ASIX
	uint8 frimware1_rev;    // PIC
	uint8 frimware2_rev;	// C8051
	uint8 frimware3_rev;	// SM5964
	uint8 bootloader_rev;

	uint8 no_used[10];
}Str_Pro_Info;

typedef	union
{
	uint8_t all[100];

	struct 
	{ 	
	
	 uint8_t ip_addr[4];
	 uint8_t subnet[4];
	 uint8_t getway[4];		 
	 uint8_t mac_addr[6];

	 uint8_t tcp_type;   /* 1 -- DHCP, 0 -- STATIC */

	 uint8_t debug;

	 Str_Pro_Info pro_info;
//	 uint8_t com_config[3];
	 uint8_t refresh_flash_timer;
	 uint8_t en_plug_n_play;

	 uint8_t reset_default;	  // write 88

	 uint8_t  en_username;
	 uint8_t  cus_unit;

	 uint8_t usb_mode; 
	 uint8_t network_number;
	 uint8_t panel_type;

	 char panel_name[20];
	 uint8_t en_panel_name;
	 uint8_t panel_number;

	}reg;
}Str_Setting_Info;


typedef struct
{
	Byte primitive;
	Uint DNET;
	Byte DLEN;
	char D_MAC_ADR[6];
	Byte dest_LSAP;
	Uint SNET;
	Byte SLEN;
	char S_MAC_ADR[6];
	Byte source_LSAP;

} UNITDATA_PARAMETERS;

typedef struct {
	uint8_t FrameType;
	uint8_t Destination;
	uint8_t Source;
	Uint Length;
	uint8_t HeaderCRC;
	char Buffer[MAXFRAMEBUFFER+2+1];
	Uint DataCRC;
	uint8_t being_used ;
	uint8_t locked ;
	uint8_t port ;
	uint8_t reply_flag ;
	uint8_t number ;
	uint8_t delay ;

} FRAME_ENTRY;

//typedef struct {

//	 /* Timers for both protocols */
//	Uint HeartbeatTimer;
//	Uint InactivityTimer;
//	Uint SilenceTimer;
//	Uint ResponseTimer;     /* only for PTP */
//	Uint SendFrameTimer;

//	/* variables for both protocols */
//	uint8_t base_task;
//	uint8_t tx_wake_up;
///*	uint8_t Using_send_frame;*/
//	uint8_t Using_send_buffer;

//	 /* States for PTP */
//	uint8_t PTP_reception_state;       /* OS - original station for RS485 */
//	uint8_t PTP_transmission_state;    /* NS - next station for RS485     */
//	uint8_t PTP_connection_state;      /* sendinfo_flag for RS485         */

//	uint8_t Rec_frame_state;

// /* offset 20 */
//	uint8_t FrameType;
//	uint8_t Destination;
//	uint8_t Source;
//	Uint Length;
//	uint8_t HeaderCRC;
//	uint8_t PTP_DLE_mask;
//	Uint DataCRC;
//	Uint Index;
//	uint8_t Rx_work_byte;
//	char *DataBuffer;
////	FRAME_ENTRY *rec_frame;

// /* offset 38 */
//	/* PTP related flags */
//	uint8_t ack0received;//				    : 1; /* receivedpollformaster for RS485 */
//	uint8_t ack1received;//				    : 1; /* newpanelbehind for RS485 */
//	uint8_t nack0received;//				  : 1; /* panelOff for RS485 */
//	uint8_t nack1received;//				  : 1; /* receivedtoken for RS485 */
//	uint8_t transmission_blocked;//		: 1;
//	uint8_t TxSequence_number;//			: 1;
//	uint8_t RxSequence_number;//			: 1;
//	uint8_t reception_blocked;//			: 2; /* wantpointsentry for RS485 */
//	/* flags related to both protocols */
//	uint8_t rec_frame_ready;//			  : 1;
//	uint8_t received_invalid_frame;//	: 1;
//	uint8_t received_valid_frame;//		: 1;
//	uint8_t receive_frame_free;//		  : 1;
////	uint8_t send_frame_free;//        : 1;
////	uint8_t send_frame_locked;//			: 1;
////	uint8_t sending_frame_now;//			: 1;
//	/* MSTP related flags */
//	Uint     EventCount;

// /* offset 42 */
//	uint8_t RetryCount;//             : 3;
//	uint8_t FrameCount ;//            : 3;
//	uint8_t FirstToken ;//            : 3;
//	uint8_t replyflag ;//             : 2;
//	uint8_t validint  ;//             : 1;
//	uint8_t SoleMaster;//             : 1;
//	uint8_t UsedToken ;//             : 1;
//	uint8_t action ;//                : 1;
//	uint8_t netpointsflag ;//         : 1;

////	uint8_t ready_for_descriptors;//  : 4;
//	uint8_t rec_trigger_sequence ;//  : 4; /* only for PTP */
//                                       /* laststation_connected for RS485 */
////	uint8_t timeonnet;//              : 3;
////	uint8_t link_type;//              : 2;
////	uint8_t port   ;//                : 1;
////	uint8_t tx_end  ;//               : 1;
////	uint8_t first_send ;//            : 1;

////	char     nextpanelisoff;
////	uint32_t need_info;

//}STR_PTP;  


typedef struct
{
		 int   network;
		 Byte  router_address;
		 Byte  half_router_address;
		 Byte  status;
}  Reachable_Networks;

typedef struct
{
		Byte address;
		int  network;
		Reachable_Networks networks_list;
} Port_Def;

#define   PORT_ACTIVE      0x03
#define   PTP_INSTALLED    0x05
#define   PTP_ACTIVE       0x07


#define MAX_reachable_networks 2
#define MAX_Routing_table      2

/*
// status :   0  - free
//            0x01 Port_installed  ( 0000 0001 )
//            0x03 Port active     ( 0000 0011 )
//            0x05 PTP installed   ( 0000 0101 )
//            0x07 PTP active      ( 0000 0111 )
//            0x11 RS485 installed ( 0001 0001 )
//            0x13 RS485 active    ( 0001 0011 )
//            0x23 ETHERNET        ( 0010 0011 )
*/

typedef struct {
		char 		  status;
		Port_Def  Port;
	} Routing_Table;


typedef enum
	{
		DL_INVALID = -1, DL_UNITDATA_REQUEST, DL_UNITDATA_INDICATION,
		DL_UNITDATA_RESPONSE, DL_CONNECT_REQUEST, DL_CONNECT_INDICATION,
		DL_CONNECT_RESPONSE, DL_DISCONNECT_REQUEST, DL_TEST_REQUEST,
		DL_TEST_RESPONSE, DL_CONNECT_ATTEMPT_FAILED
	}
	DL_Primitives;
typedef enum { N_UNITDATArequest, N_UNITDATAindication } NETWORKPrimitive;
typedef enum
{
	Who_Is_Router_To_Network, I_Am_Router_To_Network,
	I_Could_Be_Router_To_Network,	Reject_Message_To_Network,
	Router_Busy_To_Network, Router_Available_To_Network,
	Initialize_Routing_Table, Initialize_Routing_Table_Ack,
	Establish_Connection_To_Network, Disconnect_Connection_To_Network,
	I_Am_Router_To_Network_Prop=0x80

} Network_Layer_Message_Type;
typedef enum { Q_BLOCKED, Q_ALMOST_BLOCKED, Q_NOT_BLOCKED } Blocked_States;
typedef enum
{
	HEARTBEAT_XOFF = 0, HEARTBEAT_XON, DATA_0, DATA_1, DATA_ACK_0_XOFF,
	DATA_ACK_1_XOFF, DATA_ACK_0_XON, DATA_ACK_1_XON, DATA_NAK_0_XOFF,
	DATA_NAK_1_XOFF, DATA_NAK_0_XON, DATA_NAK_1_XON, CONNECT_REQUEST,
	CONNECT_RESPONSE, DISCONNECT_REQUEST, DISCONNECT_RESPONSE,
	TEST_REQUEST, TEST_RESPONSE
}	Standard_Frame_Types;
typedef enum
	{
			RX_IDLE, RX_PREAMBLE, RX_HEADER, RX_DATA, RX_DATA_CRC
	} ReceiveFrameStates;

typedef enum
	{
		DISCONNECTED, INBOUND, OUTBOUND, CONNECTED, DISCONNECTING
	}
	PTP_CON_States;	//	 5

typedef enum { REC_IDLE, REC_READY, DATA, DATA_ACK, DATA_NAK } PTP_REC_States;//	3

typedef enum { TR_IDLE, TR_PENDING, TR_READY, TR_BLOCKED } PTP_TR_States;//	 4




#define USER_DATA_HEADER_LEN 7



//U16_T swap_word( U16_T dat );	//	  mGetPointWord2
U16_T convert_pointer_to_word( U8_T *iAddr );  //	 mGetPointWord
U32_T convert_pointer_to_double( U8_T *iAddr ) ;  // DoulbemGetPointWord
U32_T swap_double( U32_T dat ) ;  //DoulbemGetPointWord2
void Initial_Panel_Info(void);


U8_T check_remote_point_list(Point_Net *point,U8_T *index);
void put_remote_point_value( S16_T index, S32_T *val_ptr, S16_T prog_op );
void add_remote_point(U8_T panel,U8_T point_type,U8_T number,S32_T val_ptr/*,U8_T auto_manual,U8_T digital_analog*/);

typedef enum
{
 AV,AI,AO,BI,BO
}BACNET_type;

//void sub_send_string(U8_T *p, U16_T length,U8_T port);
void wirte_bacnet_value_to_buf(uint8_t type,uint8_t priority,uint8_t i);
void Read_bacnet_value_from_buf(void);

//void change_panel_number_in_code(U8_T old, U8_T new);




#endif
