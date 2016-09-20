#include "tcp_server_demo.h" 
#include "lwip_comm.h"
#include "lwip/api.h"

static struct netconn *tcp_serverconn;			//TCP Server�������ӽṹ��(������)
static struct netconn *tcp_servernewconn;		//TCP Server�������ӽṹ��(�洢�����ɹ����������)

u8 tcp_server_recvbuf[TCP_SERVER_RX_BUFSIZE];	//TCP���������ջ�����
const u8 *tcp_server_sendbuf="Explorer STM32F407 TCP Server send data\r\n";

void tcp_server_test(void)
{
	err_t err;
	u8 remot_addr[4]; 
	ip_addr_t ipaddr;
	struct netbuf *recvbuf; 
	u16	port;
	u8 key;
	u8 rval=0;//�˳���־
	u8 t=0;
	
	tcp_serverconn=netconn_new(NETCONN_TCP);  //����һ��TCP����
	netconn_bind(tcp_serverconn,IP_ADDR_ANY,TCP_SERVER_PORT);  //��TCP_SERVER_PORT�˿�
	netconn_listen(tcp_serverconn);  		//�������ģʽ
	tcp_serverconn->recv_timeout=10;  		//��ֹ�����߳� 
	while(rval==0) 
	{
		err=netconn_accept(tcp_serverconn,&tcp_servernewconn);  //������������
		tcp_servernewconn->recv_timeout=10; 
		key=KEY_Scan(0);
		if(key==WKUP_PRES)rval=1;//�˳�tcp server����.	
		if(err==ERR_OK)//��Ҫ���������ӵ�����
		{ 
			netconn_getaddr(tcp_servernewconn,&ipaddr,&port,0); //��ȡԶ��IP��ַ�Ͷ˿ں�
			remot_addr[3]=ipaddr.addr>>24; 
			remot_addr[2]=ipaddr.addr>>16;
			remot_addr[1]=ipaddr.addr>>8;
			remot_addr[0]=ipaddr.addr;
			printf("����%d.%d.%d.%d �����Ϸ�����,�����˿ں�Ϊ:%d\r\n",remot_addr[0], remot_addr[1],remot_addr[2],remot_addr[3],port);
			while(1)//��������
			{
				key=KEY_Scan(0);
				if(key==KEY0_PRES)//��������
				{ 
					err=netconn_write(tcp_servernewconn ,tcp_server_sendbuf,strlen((char*)tcp_server_sendbuf),NETCONN_COPY); //����tcp_server_sendbuf�е�����
					if(err!=ERR_OK)printf("����ʧ��\r\n");	 
				} 
				if(key==WKUP_PRES)//�˳�tcp server����.
				{
					rval=1;
					break;
				}
				err=netconn_recv(tcp_servernewconn,&recvbuf);//�鿴�Ƿ���յ�����
				if(err==ERR_OK)  //���յ�����
				{		 
					if((recvbuf->p->tot_len)>=TCP_SERVER_RX_BUFSIZE) //�յ������ݴ���tcp���������ջ�����
					{
						((char*)recvbuf->p->payload)[TCP_SERVER_RX_BUFSIZE-1]=0;//ĩβ���������
						memcpy(tcp_server_recvbuf,recvbuf->p->payload,TCP_SERVER_RX_BUFSIZE);//��������
					}else
					{
						memcpy(tcp_server_recvbuf,recvbuf->p->payload,recvbuf->p->tot_len);
						tcp_server_recvbuf[recvbuf->p->tot_len]=0;//ĩβ���������
					}
					printf("�ͻ���%d.%d.%d.%d��������:%s\r\n",remot_addr[0], remot_addr[1],remot_addr[2],remot_addr[3],tcp_server_recvbuf);
					netbuf_delete(recvbuf);
				}else if(err==ERR_CLSD)
				{
					printf("����:%d.%d.%d.%d�Ͽ��������������\r\n",remot_addr[0], remot_addr[1],remot_addr[2],remot_addr[3]);
					netconn_close(tcp_servernewconn);   //�ر�tcp_servernewconn����
					netconn_delete(tcp_servernewconn);  //ɾ��tcp_servernewconn����
					break;
				} 
				delay_ms(10);
			}
		} 
		delay_ms(10);
		t++;
		if(t==100)
		{
			t=0;
			printf("TCP Server�ȴ�������...\r\n");
		}
	}
	if(tcp_serverconn!=NULL)//�������ӽṹ����Ч?
	{
		netconn_close(tcp_serverconn);   //�ر�tcp_serverconn����
		netconn_delete(tcp_serverconn);  //ɾ��tcp_serverconn����
	}
	if(tcp_servernewconn!=NULL)//�����ӽṹ����Ч?
	{
		netconn_close(tcp_servernewconn);   //�ر�tcp_servernewconn����
		netconn_delete(tcp_servernewconn);  //ɾ��tcp_servernewconn����
	}
}
 




















