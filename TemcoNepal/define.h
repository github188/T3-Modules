#ifndef _DEFINE_H
#define _DEFINE_H

#define STM32F10X



#ifdef STM32F10X
#define far 
#define xdata
#define idata
#endif

#define READ_WRITE_PROPERTY 1
#define SOFTREV    5

#define MODBUS 0 
#define BAC_MSTP 1
#define TCP_IP 2
#define BAC_IP 3
#define BAC_PTP 4
#define BAC_GSM 5

#define DEFAULT_FILTER  5
#define T322AI				43
//#define T38AI8AO6DO   	44
//#define T3PT12				45

//#define SW_VER 20150112


#define	BAUDRATE_9600			9600			//0
#define	BAUDRATE_19200			19200			//1
#define	BAUDRATE_38400			38400			//2
#define	BAUDRATE_57600			57600			//3
#define	BAUDRATE_115200			115200		//4



#ifdef  T3PT12
#define HW_VER			1
#define MAX_AI_CHANNEL	 12
#define MAX_AIS         MAX_AI_CHANNEL
#define PRODUCT_ID 		T3PT12
#define MAX_OUTS				1
#define MAX_AOS					1



#endif





#ifdef T322AI 
#define PRODUCT_ID 		T322AI
#define HW_VER			8
#define MAX_AI_CHANNEL	 22
#define MAX_AIS         MAX_AI_CHANNEL
#define MAX_INS  MAX_AIS 
#define MAX_OUTS				1
#define MAX_AOS					1
//IO��������	 
#define CHA_SEL0				PAout(0)	//SCL
#define CHA_SEL1				PAout(1)	//SDA	 
#define CHA_SEL2				PAout(2)		//����SDA
#define CHA_SEL3				PAout(3)		//����SDA
#define CHA_SEL4				PAout(4)

#define RANGE_SET0			PCout(8)
#define RANGE_SET1			PCout(9)
#endif 

#ifdef T38AI8AO6DO 
#define SWITCH_NUM 14

#define PRODUCT_ID 		T38AI8AO6DO
#define HW_VER			8
#define MAX_AI_CHANNEL	 8
#define MAX_AIS         MAX_AI_CHANNEL
#define MAX_INS  				MAX_AIS 

#define MAX_AO					8
#define MAX_DO					6
#define MAX_OUTS				(MAX_AO+MAX_DO)



#define  SW_OFF  0
#define  SW_HAND 1
#define  SW_AUTO 2

////IO��������	 
//#define CHA_SEL0				PAout(0)	//SCL
//#define CHA_SEL1				PAout(1)	//SDA	 
//#define CHA_SEL2				PAout(2)		//����SDA
//#define CHA_SEL3				PAout(3)		//����SDA
//#define CHA_SEL4				PAout(4)

//#define RANGE_SET0			PCout(8)
//#define RANGE_SET1			PCout(9)



//#define PRODUCT_ID 		T38AI8AO6DO
//#define HW_VER			8
//#define MAX_AI_CHANNEL	 8
//#define MAX_AIS         MAX_AI_CHANNEL
//#define MAX_INS  MAX_AIS

//#define MAX_AOS					8
//#define MAX_DOS					6 
//#define MAX_OUTS				(MAX_AOS+MAX_DOS)


//IO��������	 
#define CHA_SEL0				PCout(0)		//		PIN15
#define CHA_SEL1				PCout(1)	//	 		PIN16
#define CHA_SEL2				PCout(2)		//	PIN13



#define CHA_SEL4				PAout(7)
#define RANGE_SET0			PAout(5)
#define RANGE_SET1			PAout(4)
#endif 




#endif 

