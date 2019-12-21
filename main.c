 #include <reg52.H> 
 #include "DHT11.h" 
 #include <stdio.h>
 #include "string.h"
 #include "adc0832.h"
 #include "1602.h"


sbit LED1=P1^2;
sbit LED2=P1^3;
sbit DJ=P1^4;
sbit SEV_PWM = P0^0;
sbit SEH_PWM = P0^1;

#define uchar unsigned char 
#define uint unsigned int

#define	  SlaveAddress   0x46 //����������IIC�����еĴӵ�ַ,����ALT  ADDRESS��ַ���Ų�ͬ�޸�
                              //ALT  ADDRESS���Žӵ�ʱ��ַΪ0xA6���ӵ�Դʱ��ַΪ0x3A
typedef   unsigned char BYTE;
typedef   unsigned short WORD;

BYTE    xdata BUF[8];                         //�������ݻ�����   	
uchar   ge,shi,bai,qian,wan;            //��ʾ����
int     dis_data;                       //����

unsigned char idata Rxbuff[50],Rxnum;
unsigned char	Recwifi_data[5];
char *strx=0;
char clinetid;//����ID
unsigned char getflag;//��ȡ��־
unsigned char xdata Line1[]="STATUS:  OPEN   ";
unsigned char xdata Line2[]="USER:    001    ";
unsigned char count = 0;                      
unsigned char SEV_count = 15;                                 
unsigned char SEH_count = 15;                      /* ?????????T0?T1??? */

char MIMA[6] = {'9','9','9','9','9','9'};
unsigned char hw_Flag = 0;
unsigned char lock_Flag = 0;
unsigned int num = 0;
void InitUART(void)			 //���Ǵ��ڵĻ������ã��������Ĳ�������9600.��Щ�������Ǳ�׼�ġ�
{
    TMOD = 0x20;
    SCON = 0x50;
    TH1 = 0xFD;
    TL1 = TH1;
    PCON = 0x00;
    EA = 1;
    ES = 1;
    TR1 = 1;
	
		
		IP=0x08;
		PCON &= 0x7F;
		TH0=0xff;
		TL0=0xa4;
		
		ET0 = 1;
		TR0=1;
}
void Time0_Init() interrupt 1
{
		TR0 = 0;
		TH0 = 0xff;
		TL0 = 0xa4;
		
		if(count <= SEH_count)
		{
				SEH_PWM = 1;
		}
		else
		{
				SEH_PWM = 0;
		}               
		
		if(count <=SEV_count)
		{
				SEV_PWM = 1;
		}
		else
		{
				SEV_PWM = 0;
		}
		count++;
		if(count>=200)
		{
				count = 0;
		}
		TR0 = 1;
}

 void Clear_Buffer(void)//��ջ���
{
		int i =0;
	for(i=0;i<50;i++)
	{
		Rxbuff[i] = 0;
	}
		Rxnum=0;
}
 
/******************************************************************************/
void delayms(unsigned int x)
{
		unsigned int i;
		while(x--)
			for(i=80;i>0;i--);
}
/*
 * UART ����һ�ֽ�
*/
void UART_send_byte(char dat)
{
		SBUF = dat;
		while (TI == 0);
		TI = 0;
}

/*
 * UART �����ַ���
*/
void Send_Str(unsigned char *buf)
{
		while (*buf != '\0')
		{
				UART_send_byte(*buf++);
		}
}

void Printfclear(void)
{
		delayms(1000);
		Send_Str(Rxbuff);
		delayms(50);
		Clear_Buffer();
}

void ESP8266_SERVER(void)//����������
{

		Send_Str("AT+RST\r\n");             //ESP8266��λ
		Printfclear();
		Send_Str("AT\r\n");
		Printfclear();
		Send_Str("AT+RATE=9600\r\n");
		Printfclear();
		Send_Str("AT+AP=CMCC-GHPN,y7n2qrya\r\n");
		Printfclear();
		Send_Str("AT+DEVICEID=aqwsdr12347\r\n");		//�豸ID aqwsdr12345 ��Ҫ�ͻ��Լ��޸����ID�������û�����ͻ
		Printfclear();
		Send_Str("AT+KEEPALIVE=120\r\n");        //MQTT����� 0-3600��
		Printfclear();
		Send_Str("AT+MQTTCLOUD=58.87.121.189,61613,admin,password\r\n");   //������IP:58.87.121.189 port:61613 mqtt�˺�:admin ���룺password
		Printfclear();
		Clear_Buffer();	
		Send_Str("AT+GSERVERSTATUS\r\n");//����������
		
		delayms(500);	
		Send_Str(Rxbuff);
		delayms(50);
		strx=strstr((const char*)Rxbuff,(const char*)"ok");//�ȴ��������
		while(strx==NULL)
		{		Clear_Buffer();
				Send_Str("AT+GSERVERSTATUS\r\n");//����������
				delayms(500);
				Send_Str(Rxbuff);
				delayms(50);
				strx=strstr((const char*)Rxbuff,(const char*)"ok");//�ȴ��������
		}
		Clear_Buffer();	
		Send_Str("AT+SUB=nydev/123456,0\r\n");               //�������� dev/123456
		Printfclear();
}


void Get_Clinet(void)//�ж��Ƿ��ȡ���˿ͻ��˽���
{
		uchar i;
		strx=strstr((const char*)Rxbuff,(const char*)"AT+RECTOPIC=nydev/123456");//�����ݷ���
		if(strx)//������ȡ����
		{
				clinetid=strx[5];
				strx=strstr((const char*)Rxbuff,(const char*)",");//�����ݷ���
				for(i=0;i<8;i++)
				Recwifi_data[i]=strx[1+i];		  //��ȡ�ֻ���LED�Ŀ���
				if(Recwifi_data[0]=='0'&&Recwifi_data[1]=='0')//����
				{
						LED1=1;
						hw_Flag = 1;
						lock_Flag = 1;
						SEV_count = 30;
						SEH_count = 30;
						MIMA[0] = Recwifi_data[2];
						MIMA[1] = Recwifi_data[3];
						MIMA[2] = Recwifi_data[4];
						MIMA[3] = Recwifi_data[5];
						MIMA[4] = Recwifi_data[6];
						MIMA[5] = Recwifi_data[7];
						num = 0;
				}
				else	if( Recwifi_data[0]=='1'&&Recwifi_data[1]=='1'		)//����
				{
						LED1=0 ;
						hw_Flag = 0;
						lock_Flag = 0;
						SEV_count = 15;
						SEH_count = 15;
						num = 0;
				}
				Printfclear();	
     }
}


void Send_DATA(uchar *buffer)
{
    Send_Str("AT+PUB=nyapp/123456,");
		Send_Str(buffer);
		Send_Str(",0\r\n");
    delayms(200);	
		Printfclear();
}


void main()
{
		char Tx_Buf[9];
		

		delayms(500);
		delayms(500);
		delayms(1000);	  	//��ʱһ��ʱ�䣬��WIFIģ���ȶ�
		
		LCD_Init();         //��ʼ��Һ����
		InitUART();					//��ʼ������
		ESP8266_SERVER();		//��ʼ��ESP8266
		LCD_Write_Command(0x01); //��ʾ����
		LCD_Str(0,0,Line1);//д���һ����Ϣ
		LCD_Str(0,1,Line2);//д��ڶ�����Ϣ
		while(1)	
		{	  
				delayms(50);
				Get_Clinet();
				if(num++>20)
				{
						num = 0;
						Tx_Buf[0]='T';	//֡ͷ
						if(hw_Flag==0)
						{
							Tx_Buf[1] = '0'; 
						}
						else
						{
							Tx_Buf[1] = '1';
						}
						if(lock_Flag==0)
						{
							Tx_Buf[2] = '0'; 
						}
						else
						{
							Tx_Buf[1] = '1';
						}

						Tx_Buf[3] = MIMA[0];
						Tx_Buf[4] = MIMA[1];
						Tx_Buf[5] = MIMA[2]; 
						Tx_Buf[6] = MIMA[3];
						Tx_Buf[7] = MIMA[4]; 
						Tx_Buf[8] = MIMA[5]; 
						
						Send_DATA(Tx_Buf)	;//��������
				}	
		}
}

/*****************���ڽ����жϺ���,��������ģ�������*********************/
void UARTInterrupt(void) interrupt 4
{	
		if(RI)
    {
				ES=0;
        RI = 0;
				Rxbuff[Rxnum++]=SBUF;
				if(Rxnum>=50)
				Rxnum=0;
				ES=1;
    }
}
 