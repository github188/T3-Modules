
#include "tcp_modbus.h"

u8 tcp_server_databuf[500];   	//�������ݻ���	  
u8 tcp_server_sta;				//�����״̬
//[7]:0,������;1,�Ѿ�����;
//[6]:0,������;1,�յ��ͻ�������
//[5]:0,������;1,��������Ҫ����

u8 tcp_server_sendbuf[500];
u16 tcp_server_sendlen;


//����һ��TCP ������Ӧ�ûص�������
//�ú���ͨ��UIP_APPCALL(tcp_demo_appcall)����,ʵ��Web Server�Ĺ���.
//��uip�¼�����ʱ��UIP_APPCALL�����ᱻ����,���������˿�(1200),ȷ���Ƿ�ִ�иú�����
//���� : ��һ��TCP���ӱ�����ʱ�����µ����ݵ�������Ѿ���Ӧ��������Ҫ�ط����¼�
void tcp_server_appcall(void)
{
	u8 send_flag = 0;
 	struct tcp_demo_appstate *s = (struct tcp_demo_appstate *)&uip_conn->appstate;
	if(uip_aborted())tcp_server_aborted();		//������ֹ
 	if(uip_timedout())tcp_server_timedout();	//���ӳ�ʱ   
	if(uip_closed())tcp_server_closed();		//���ӹر�	   
 	if(uip_connected())tcp_server_connected();	//���ӳɹ�	    
	if(uip_acked())tcp_server_acked();			//���͵����ݳɹ��ʹ� 

	if(uip_newdata())//�յ��ͻ��˷�����������
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
		
//		if((tcp_server_sta & (1 << 6)) == 0)	//��δ�յ�����
//		{
////	    	strcpy((char*)tcp_server_databuf, uip_appdata);
//			modbus_data_cope((u8*)uip_appdata, uip_len, 1);
//			tcp_server_sta |= 1 << 6;			//��ʾ�յ��ͻ�������
//			
//			sprintf((char*)tcp_server_databuf, "TCP Server OK.............\r\n");	 
//			tcp_server_sta |= 1 << 5;			//�����������Ҫ����
//		}
	}
	
//	/*else*/ if(tcp_server_sta & (1 << 5))			//��������Ҫ����
//	{
//		s->textptr = tcp_server_databuf;
//		s->textlen = strlen((const char*)tcp_server_databuf);
//		tcp_server_sta &= ~(1 << 5);			//������
//	}
	
	//����Ҫ�ط��������ݵ�����ݰ��ʹ���ӽ���ʱ��֪ͨuip�������� 
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

//��ֹ����				    
void tcp_server_aborted(void)
{
	tcp_server_sta &= ~(1 << 7);				//��־û������
	uip_log("tcp_server aborted!\r\n");			//��ӡlog
}

//���ӳ�ʱ
void tcp_server_timedout(void)
{
	tcp_server_sta &= ~(1 << 7);				//��־û������
	uip_log("tcp_server timeout!\r\n");			//��ӡlog
}

//���ӹر�
void tcp_server_closed(void)
{
	tcp_server_sta &= ~(1 << 7);				//��־û������
	uip_log("tcp_server closed!\r\n");			//��ӡlog
}

//���ӽ���
void tcp_server_connected(void)
{								  
	struct tcp_demo_appstate *s = (struct tcp_demo_appstate *)&uip_conn->appstate;
	//uip_conn�ṹ����һ��"appstate"�ֶ�ָ��Ӧ�ó����Զ���Ľṹ�塣
	//����һ��sָ�룬��Ϊ�˱���ʹ�á�
 	//����Ҫ�ٵ���Ϊÿ��uip_conn�����ڴ棬����Ѿ���uip�з�����ˡ�
	//��uip.c �� ����ش������£�
	//		struct uip_conn *uip_conn;
	//		struct uip_conn uip_conns[UIP_CONNS]; //UIP_CONNSȱʡ=10
	//������1�����ӵ����飬֧��ͬʱ�����������ӡ�
	//uip_conn��һ��ȫ�ֵ�ָ�룬ָ��ǰ��tcp��udp���ӡ�
	tcp_server_sta |= 1 << 7;					//��־���ӳɹ�
  	uip_log("tcp_server connected!\r\n");		//��ӡlog
	s->state = STATE_CMD; 						//ָ��״̬
	s->textlen = 0;
//	s->textptr = "Connect to STM32 Board Successfully!\r\n";
//	s->textlen = strlen((char *)s->textptr);
}

//���͵����ݳɹ��ʹ�
void tcp_server_acked(void)
{						    	 
	struct tcp_demo_appstate *s = (struct tcp_demo_appstate *)&uip_conn->appstate;
	s->textlen = 0;								//��������
	uip_log("tcp_server acked!\r\n");			//��ʾ�ɹ�����		 
}

//�������ݸ��ͻ���
void tcp_server_senddata(void)
{
	struct tcp_demo_appstate *s = (struct tcp_demo_appstate *)&uip_conn->appstate;
	//s->textptr : ���͵����ݰ�������ָ��
	//s->textlen �����ݰ��Ĵ�С����λ�ֽڣ�		   
	if(s->textlen > 0)
		uip_send(s->textptr, s->textlen);//����TCP���ݰ�	 
}