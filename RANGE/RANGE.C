#include "RANGE.H"
#include "user_data.h"
int16_t Calculate(uint16_t data_in, uint16_t * x_axis, uint16_t* y_axis, uint16_t point) ;
const long  tab_int[10] = { 11875, 21375, 10000, 18000, 10000, 18000,10000, 18000, 10000, 18000 };

const long  limit[10][2] = { { -40000L, 150000L }, { -40000L, 302000L },
							{ -40000L, 120000L }, { -40000L, 248000L },
							{ -40000L, 120000L }, { -40000L, 248000L },
							{ -40000L, 120000L }, { -40000L, 248000L },
							{ -50000L, 110000L }, { -58000L, 230000L }
						  };


const u16  def_tab[5][17] = {
 /* 3k termistor YSI44005 -40 to 150 Deg.C or -40 to 302 Deg.F */
	{ 233*4,  211*4, 179*4, 141*4, 103*4, 71*4, 48*4, 32*4,
		21*4, 14*4, 10*4, 7*4, 5*4, 4*4, 3*4, 2*4, 1*4 },

 /* 10k termistor GREYSTONE -40 to 120 Deg.C or -40 to 248 Deg.F */  // type2
	{ 246*4, 238*4, 227*4, 211*4, 191*4, 167*4, 141*4, 115*4,
	 92*4, 72*4, 55*4, 42*4, 33*4, 25*4, 19*4, 15*4, 12*4 },

 /* 3k termistor GREYSTONE -40 to 120 Deg.C or -40 to 248 Deg.F */
	{ 233*4, 215*4, 190*4, 160*4, 127*4, 96*4, 70*4, 50*4,
		35*4, 25*4, 18*4, 13*4, 9*4, 7*4, 5*4, 4*4, 3*4 },

 /* 10k termistor KM -40 to 120 Deg.C or -40 to 248 Deg.F */ // type3
	{ 1020, 970, 926, 860, 764, 645, 506, 360,
		92*4, 72*4, 55*4, 158, 120, 100, 19*4, 15*4, 12*4 },

 /* 3k termistor AK -40 to 150 Deg.C or -40 to 302 Deg.F */
	{ 246*4, 238*4, 227*4, 211*4, 191*4, 167*4, 141*4, 115*4,
		92*4, 72*4, 55*4, 42*4, 33*4, 25*4, 19*4, 15*4, 12*4 }
};




//u32 get_input_value_by_range( u8 range, u16 raw )
//{
//	int index;
//	long val;
//	int work_var;
//	int ran_in;
//	int delta = MIDDLE_RANGE;
//	u16 *def_tbl;
//	u8 end = 0;
//	range--;
//	ran_in = range;
//	range >>= 1;
//	def_tbl = ( u16 * )&def_tab[range];

//	if( raw <= def_tbl[NO_TABLE_RANGES] )
//		return limit[ran_in][1];
//	if( raw >= def_tbl[0] )
//		return limit[ran_in][0];
//	index = MIDDLE_RANGE;

//	while( !end )
//	{
//		if( ( raw >= def_tbl[index] ) && ( raw <= def_tbl[index-1] ) )
//		{
//			index--;
//			delta = def_tbl[index] - def_tbl[index+1];
//			if( delta )
//			{
//				work_var = (int)( ( def_tbl[index] - raw ) * 100 );
//				work_var /= delta;
//				work_var += ( index * 100 );
//				val = tab_int[ran_in];
//				val *= work_var;
//				val /= 100;
//				val += limit[ran_in][0];
//			}
//			return val;
//		}
//		else
//		{
//			if( !delta )
//				end = 1;
//			delta /= 2;
//			if( raw < def_tbl[index] )
//				index += delta;
//			else
//				index -= delta;
//			if( index <= 0 )
//				return limit[ran_in][0];
//			if( index >= NO_TABLE_RANGES )
//				return limit[ran_in][1];
//		}
//	}
//	return 0 ;
//}

//signed int RangeConverter(u8 function,  signed int para,signed int cal,unsigned char i)
//{
//	s16  siAdcResult;
//	u8  ucFunction;
//	s16   siInput;
//	s16   uiCal;
//	s16 siResult;
//	u8  loop;
//	u8 point = 0 ;
//	u8 bAnalogInputStatus =0;
//	ucFunction = function;
//	siInput = para;
//	uiCal = cal;	 
//		switch(ucFunction)
//		{
//		case UNUSED:
//			siResult=siInput ;
//		 break ;
//		 case OFF_ON:
//		 case CLOSED_OPEN:
//		 case STOP_START:
//		 case DISABLED_ENABLED:
//		 case NORMAL_ALARM:
//		 case NORMAL_HIGH:
//		 case NORMAL_LOW:
//		 case NO_YES:
//		 case COOL_HEAT: 
//		 case UNOCCUPIED_OCCUPIED:
//		 case LOW_HIGH:	 	 
//		  siResult = (siInput > 512 ) ? 1 : 0;	 	 
//		 break ;	 
//		 case ON_OFF:
//		 case OPEN_CLOSED:
//		 case START_STOP:
//		 case ENABLED_DISABLED:
//		 case ALARM_NORMAL:
//		 case HIGH_NORMAL:
//		 case LOW_NORMAL:
//		 case YES_NO:
//		 case HEAT_COOL:
//		 case OCCUPIED_UNOCCUPIED:
//		 case HIGH_LOW:	
//			  siResult = (siInput > 512 ) ? 0 : 1;	 	 
//		 break ;
//		 case custom_digital1:
//		 case custom_digital2:	 
//		 case custom_digital3:
//		 case custom_digital4:
//		 case custom_digital5:
//		 case custom_digital6:
//		 case custom_digital7:
//		 case custom_digital8:
//			siResult = (siInput < 512 ) ? 0 : 1;	
//		 break ;	
//			case Y3K_40_150DegC:
//			case Y3K_40_300DegF:
//			case R3K_40_150DegC:
//			case R3K_40_300DegF:
//			case R10K_40_120DegC:
//			case R10K_40_250DegF:
//			case KM10K_40_120DegC:
//			case KM10K_40_250DegF:
//			case A10K_50_110DegC:
//			case A10K_60_200DegF:		
//				siResult = get_input_value_by_range( ucFunction, siInput );
//				break;

//			case V0_5:
//				siResult =  ( siInput * 5000L ) >> 10;
//			break ;
//			case V0_10_IN:
//				siResult = (siInput * 10000) >> 10;
//			break; 
//			case I0_100Amps:
//				siResult = ( 100000L * (u32)siInput ) >> 10;
//			break;
//			case I0_20ma:
//			siResult = ( 20000L * (u32)siInput ) >> 10;
//			break;
//			case I0_20psi:
//			siResult = ( 20000L * (u32)siInput ) >> 10;
//			break;
//			case N0_3000FPM_0_10V:
//			siResult = ( 2700000L * (u32)siInput ) >> 10;
//			break;
//			case P0_100_0_5V:
//			siResult = ( 100000L * (u32)siInput ) >> 10;
//			break;
//			case P0_100_4_20ma:
//			siResult = 100000L * ( (u32)siInput - 255 ) / 768;
//			break;
//			case table1:
//			while(modbus.customer_table1_val[loop]!=0 || modbus.customer_table1_vol[loop]!=0)	
//			{
//				loop++ ;
//				point++ ;
//				if(loop == 11) break ;
//			}
//			siResult=Calculate(siInput, modbus.customer_table1_vol, modbus.customer_table1_vol,  point);
//			break;
//			case table2:
//			while(modbus.customer_table2_val[loop]!=0 || modbus.customer_table2_vol[loop]!=0)	
//			{
//				loop++ ;
//				point++ ;
//				if(loop == 11) break ;
//			}
//			siResult=Calculate(siInput, modbus.customer_table2_vol, modbus.customer_table2_vol,  point);	
//			break;
//			case table3:
//			while(modbus.customer_table3_val[loop]!=0 || modbus.customer_table3_vol[loop]!=0)	
//			{
//				loop++ ;
//				point++ ;
//				if(loop == 11) break ;
//			}
//			siResult=Calculate(siInput, modbus.customer_table3_vol, modbus.customer_table3_vol, point);	
//			break;	
//			case table4:
//			while(modbus.customer_table4_val[loop]!=0 || modbus.customer_table4_vol[loop]!=0)	
//			{
//				loop++ ;
//				point++ ;
//				if(loop == 11) break ;
//			}
//			siResult=Calculate(siInput, modbus.customer_table4_vol, modbus.customer_table4_vol,  point);		
//			break ;
//			case table5:
//			while(modbus.customer_table5_val[loop]!=0 || modbus.customer_table5_vol[loop]!=0)	
//			{
//				loop++ ;
//				point++ ;
//				if(loop == 11) break ;
//			}
//			siResult=Calculate(siInput, modbus.customer_table5_vol, modbus.customer_table5_vol,  point);	
//			break ;
//		}

// 	return siResult;
//}

//int16_t Calculate(uint16_t data_in, uint16_t * x_axis, uint16_t* y_axis, uint16_t point)
//{
//	int16_t data_out = 0 ;
//	uint8_t  Loop ;
//	uint16_t Y1, Y2 ;
//	uint16_t  X1 , X2 ;

//	if(point < 2)
//	{
//		//AS2_SendBlock(AS2_ptr, "point < 2\n\r", strlen("point < 2\n\r"))  ;
//		return data_in ;
//	}
//	//if(Ad >co2_buf[(modbus.Co2_Cal_Point-1)].Cal_ad_value)	//((x - x1)*y2 -(x2 - x)*y1)/(x2 - x1) //ad reading is too big and out of calibration range, we use the last two biggest calibration ponits to calculate the ppm
//	if(data_in > x_axis[point - 1])
//	{	
//		X2 = x_axis[point - 1] ;
//		Y2 = y_axis[point - 1] ;
//		X1 = x_axis[point - 2] ;
//		Y1 = y_axis[point - 2] ;		
//	}
//	else if(data_in <x_axis[0]) 		//ad reading is too small and out of calibration range, we use the first two smallest calibration ponits to calculate the ppm
//	{	
//		
//		X1 = x_axis[0] ;
//		Y1 = y_axis[0] ;
//		X2 = x_axis[1] ;	
//		Y2 = y_axis[1] ;
//		
//	}
//	else
//	{	
//		Loop = 0 ;
//		while(1)
//		{
//			if(data_in >x_axis[Loop])
//			{
//				Loop++;	
//			}
//			else
//			{
//				break;
//			}	
//		}
//		X2 = x_axis[Loop] ;
//		X1 = x_axis[Loop - 1] ;
//		Y2 = y_axis[Loop] ;
//		Y1 = y_axis[Loop - 1] ;
//	}
//	data_out = (Y1*((int32_t)X2 - data_in)+ Y2*((int32_t)data_in - X1))/(X2 - X1) ;
//	//if(data_out <0) data_out = 0 ;

//	return data_out ;
//}

