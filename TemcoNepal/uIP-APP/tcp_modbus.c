
#include "tcp_modbus.h"

u8 tcp_server_databuf[500];   	//发送数据缓存	  
u8 tcp_server_sta;				//服务端状态
//[7]:0,无连接;1,已经连接;
//[6]:0,无数据;1,收到客户端数据
//[5]:0,无数据;1,有数据需要发送

u8 tcp_server_sendbuf[500];
u16 tcp_server_sendlen;


//这是一个TCP 服务器应用回调函数。
//该函数通过UIP_APPCALL(tcp_demo_appcall)调用,实现Web Server的功能.
//当uip事件发生时，UIP_APPCALL函数会被调用,根据所属端口(1200),确定是否执行该函数。
//例如 : 当一个TCP连接被创建时、有新的数据到达、数据已经被应答、数据需要重发等事件
void tcp_server_appcall(void)
{
	u8 send_flag = 0;
 	struct tcp_demo_appstate *s = (struct tcp_demo_appstate *)&uip_conn->appstate;
	if(uip_aborted())tcp_server_aborted();		//连接终止
 	if(uip_timedout())tcp_server_timedout();	//连接超时   
	if(uip_closed())tcp_server_closed();		//连接关闭	   
 	if(uip_connected())tcp_server_connected();	//连接成功	    
	if(uip_acked())tcp_server_acked();			//发送的数据成功送达 

	if(uip_newdata())//收到客户端发过来的数据
	{
		
		memcpy(&tcp_server_databuf[0], uip_appdata,uip_len);		
		// check modbus data
		if(tcp_server_databuf[6] == modbus.address 
		|| ((tcp_server_databuf[6] == 255))
		)
		{	
			send_flag = 1;
			responseCmd(1,tcp_server_databuf);
			internalDeal(1, tcp_server_databuf);
		}
		
//		if((tcp_server_sta & (1 << 6)) == 0)	//还未收到数据
//		{
////	    	strcpy((char*)tcp_server_databuf, uip_appdata);
//			modbus_data_cope((u8*)uip_appdata, uip_len, 1);
//			tcp_server_sta |= 1 << 6;			//表示收到客户端数据
//			
//			sprintf((char*)tcp_server_databuf, "TCP Server OK.............\r\n");	 
//			tcp_server_sta |= 1 << 5;			//标记有数据需要发送
//		}
	}
	
//	/*else*/ if(tcp_server_sta & (1 << 5))			//有数据需要发送
//	{
//		s->textptr = tcp_server_databuf;
//		s->textlen = strlen((const char*)tcp_server_databuf);
//		tcp_server_sta &= ~(1 << 5);			//清除标记
//	}
	
	//当需要重发、新数据到达、数据包送达、连接建立时，通知uip发送数据 
	if(uip_rexmit() || uip_newdata() || uip_acked() || uip_connected() || uip_poll())
	{
		if(send_flag == 1)
		{
			s->textptr = tcp_server_sendbuf;
			s->textlen = tcp_server_sendlen;

		}
		tcp_server_senddata();
		
	}
}

//终止连接				    
void tcp_server_aborted(void)
{
	tcp_server_sta &= ~(1 << 7);				//标志没有连接
	uip_log("tcp_server aborted!\r\n");			//打印log
}

//连接超时
void tcp_server_timedout(void)
{
	tcp_server_sta &= ~(1 << 7);				//标志没有连接
	uip_log("tcp_server timeout!\r\n");			//打印log
}

//连接关闭
void tcp_server_closed(void)
{
	tcp_server_sta &= ~(1 << 7);				//标志没有连接
	uip_log("tcp_server closed!\r\n");			//打印log
}

//连接建立
void tcp_server_connected(void)
{								  
	struct tcp_demo_appstate *s = (struct tcp_demo_appstate *)&uip_conn->appstate;
	//uip_conn结构体有一个"appstate"字段指向应用程序自定义的结构体。
	//声明一个s指针，是为了便于使用。
 	//不需要再单独为每个uip_conn分配内存，这个已经在uip中分配好了。
	//在uip.c 中 的相关代码如下：
	//		struct uip_conn *uip_conn;
	//		struct uip_conn uip_conns[UIP_CONNS]; //UIP_CONNS缺省=10
	//定义了1个连接的数组，支持同时创建几个连接。
	//uip_conn是一个全局的指针，指向当前的tcp或udp连接。
	tcp_server_sta |= 1 << 7;					//标志连接成功
  	uip_log("tcp_server connected!\r\n");		//打印log
	s->state = STATE_CMD; 						//指令状态
	s->textlen = 0;
//	s->textptr = "Connect to STM32 Board Successfully!\r\n";
//	s->textlen = strlen((char *)s->textptr);
}

//发送的数据成功送达
void tcp_server_acked(void)
{						    	 
	struct tcp_demo_appstate *s = (struct tcp_demo_appstate *)&uip_conn->appstate;
	s->textlen = 0;								//发送清零
	uip_log("tcp_server acked!\r\n");			//表示成功发送		 
}

//发送数据给客户端
void tcp_server_senddata(void)
{
	struct tcp_demo_appstate *s = (struct tcp_demo_appstate *)&uip_conn->appstate;
	//s->textptr : 发送的数据包缓冲区指针
	//s->textlen ：数据包的大小（单位字节）		   
	if(s->textlen > 0)
		uip_send(s->textptr, s->textlen);//发送TCP数据包	 
}