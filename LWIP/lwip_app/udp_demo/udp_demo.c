#include "udp_demo.h" 
#include "lwip_comm.h"
#include "lwip/api.h"


static struct netconn *udp_netconn;
u8 udp_recvbuf[UDP_RX_BUFSIZE];			//UDP���ջ�����
//�����͵�����
const u8 *udp_sendbuf="Explorer STM32F407 UDP send data\r\n";

 
void udp_test(void)
{
	err_t err;
	struct ip_addr udp_ipaddr;
	struct netbuf  *recvbuf;
	struct netbuf  *sendbuf;	
 	u8 key;  

	udp_netconn=netconn_new(NETCONN_UDP);  	//����һ��UDP����
	udp_netconn->recv_timeout=10;  			//���ճ�ʱ����
	if(udp_netconn!=NULL)  //����UDP���ӳɹ�
	{
		netconn_bind(udp_netconn,IP_ADDR_ANY,UDP_PORT);	//��UDP_PORT�˿�
		IP4_ADDR(&udp_ipaddr,lwipdev.remoteip[0],lwipdev.remoteip[1],lwipdev.remoteip[2], lwipdev.remoteip[3]); //����Ŀ��IP��ַ
		err=netconn_connect(udp_netconn,&udp_ipaddr,UDP_PORT); 	//���ӵ�Զ�������˿�
		if(err==ERR_OK)//�����
		{
			while(1)
			{
				key=KEY_Scan(0);
				if(key==KEY0_PRES)//��������
				{
					sendbuf=netbuf_new();
					netbuf_alloc(sendbuf,strlen((char *)udp_sendbuf));
					sendbuf->p->payload=(void*)udp_sendbuf;   		//ָ��udp_sentbuf����
					err = netconn_send(udp_netconn,sendbuf);//��netbuf�е����ݷ��ͳ�ȥ
					if(err!=ERR_OK)printf("����ʧ��\r\n"); 
					netbuf_delete(sendbuf);  //ɾ��buf				
				}
				if(key==WKUP_PRES)break;//�˳�udp����.
				err=netconn_recv(udp_netconn,&recvbuf);//�鿴�Ƿ���յ�����
				if(err==ERR_OK)
				{
					if(recvbuf->p->tot_len>=UDP_RX_BUFSIZE) //udp���ݴ���udp���ջ�����
					{
						((char*)recvbuf->p->payload)[UDP_RX_BUFSIZE-1]=0;//ĩβ���������
						memcpy(udp_recvbuf,recvbuf->p->payload,UDP_RX_BUFSIZE);//��������
					}else
					{
						memcpy(udp_recvbuf,recvbuf->p->payload,recvbuf->p->tot_len);
						udp_recvbuf[recvbuf->p->tot_len] = 0;
					}
					printf("���յ����Ͷ˷���������:%s\r\n",udp_recvbuf);//��ӡ���յ�������
					netbuf_delete(recvbuf);      //ɾ��buf
				} 
				delay_ms(10);
			}
		}
	}
	if(udp_netconn)
	{
		netconn_disconnect(udp_netconn);//�ر�UDP����
		netconn_delete(udp_netconn);//ɾ��udp����	
	}
}

  
























