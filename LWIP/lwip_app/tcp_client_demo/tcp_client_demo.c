#include "tcp_client_demo.h"
#include "lwip_comm.h"
#include "lwip/api.h"
 
static struct netconn *tcp_clientconn;			//TCP CLIENT�������ӽṹ��
u8 tcp_client_recvbuf[TCP_CLIENT_RX_BUFSIZE];	//TCP�ͻ��˽��ջ�����
//�����͵�����
const u8 *tcp_client_sendbuf="Explorer STM32F407 TCP Client send data\r\n";

void tcp_client_test(void)
{
	err_t err; 
	ip_addr_t server_ipaddr,loca_ipaddr;
	u16 client_port,loca_port;
	struct netbuf *recvbuf;
	u8 key;
 	u8 rval=0;//�˳���־
	u8 t=0;
	
	client_port=TCP_CLIENT_PORT;
	//����Զ��IP��ַ
	IP4_ADDR(&server_ipaddr,lwipdev.remoteip[0],lwipdev.remoteip[1],lwipdev.remoteip[2],lwipdev.remoteip[3]);
	tcp_clientconn=netconn_new(NETCONN_TCP); //����һ��TCP����
	while(tcp_clientconn&&rval==0)//�����ɹ�
	{
		err=netconn_connect(tcp_clientconn,&server_ipaddr,client_port);	//���ӷ����� 
		key=KEY_Scan(0);
		if(key==WKUP_PRES)rval=1;//�˳�tcp client����.	
		if(err==ERR_OK)//�����ɹ�
		{ 
			tcp_clientconn->recv_timeout=10;
			netconn_getaddr(tcp_clientconn,&loca_ipaddr,&loca_port,1); 		//��ȡ����IP����IP��ַ�Ͷ˿ں�
			printf("�����Ϸ�����%d.%d.%d.%d,�����˿ں�Ϊ:%d\r\n",lwipdev.remoteip[0],lwipdev.remoteip[1],lwipdev.remoteip[2],lwipdev.remoteip[3],loca_port);
			while(1)
			{
				key=KEY_Scan(0);
				if(key==KEY0_PRES)//��������
				{
					err=netconn_write(tcp_clientconn ,tcp_client_sendbuf,strlen((char*)tcp_client_sendbuf),NETCONN_COPY); //����tcp_server_sentbuf�е�����
					if(err!=ERR_OK)printf("����ʧ��\r\n");				
				}
				if(key==WKUP_PRES)//�˳�tcp server����.
				{
					rval=1;
					break;
				}
				err=netconn_recv(tcp_clientconn,&recvbuf);//�鿴�Ƿ���յ�����
				if(err==ERR_OK)  //���յ�����
				{		 
					if((recvbuf->p->tot_len)>=TCP_CLIENT_RX_BUFSIZE) //udp���ݴ���tcp���������ջ�����
					{
						((char*)recvbuf->p->payload)[TCP_CLIENT_RX_BUFSIZE-1]=0;//ĩβ���������
						memcpy(tcp_client_recvbuf,recvbuf->p->payload,TCP_CLIENT_RX_BUFSIZE);//��������
					}else
					{
						memcpy(tcp_client_recvbuf,recvbuf->p->payload,recvbuf->p->tot_len);
						tcp_client_recvbuf[recvbuf->p->tot_len]=0;//ĩβ��ӽ�����
					}
					printf("������%d.%d.%d.%d��������:\r\n%s\r\n",lwipdev.remoteip[0],lwipdev.remoteip[1],lwipdev.remoteip[2],lwipdev.remoteip[3],tcp_client_recvbuf);
					netbuf_delete(recvbuf);
				}else if(err==ERR_CLSD)
				{
					netconn_close(tcp_clientconn);//�ر�tcp_clientconn����
					printf("������%d.%d.%d.%d�Ͽ�����\r\n",lwipdev.remoteip[0],lwipdev.remoteip[1],lwipdev.remoteip[2],lwipdev.remoteip[3]);
					break;
				}
				delay_ms(10);
			}
		}else//����ʧ��.
		{
			netconn_close(tcp_clientconn);			//�ر�tcp_clientconn����
			netconn_delete(tcp_clientconn); 		//ɾ��tcp_clientconn����
			tcp_clientconn=netconn_new(NETCONN_TCP);//����һ����TCP����
		}
		delay_ms(10);
		t++;
		if(t==100)
		{
			t=0;
			printf("TCP Client �������ӷ�����...\r\n");
		}		
	}
	if(tcp_clientconn!=NULL)//��tcp_clientconn���Ӵ��ڵ������ɾ������
	{
 		netconn_close(tcp_clientconn);  	//�ر�tcp_clientconn����
		netconn_delete(tcp_clientconn); 	//ɾ��tcp_clientconn����
	} 
}  




























