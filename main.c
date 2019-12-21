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

#define	  SlaveAddress   0x46 //定义器件在IIC总线中的从地址,根据ALT  ADDRESS地址引脚不同修改
                              //ALT  ADDRESS引脚接地时地址为0xA6，接电源时地址为0x3A
typedef   unsigned char BYTE;
typedef   unsigned short WORD;

BYTE    xdata BUF[8];                         //接收数据缓存区   	
uchar   ge,shi,bai,qian,wan;            //显示变量
int     dis_data;                       //变量

unsigned char idata Rxbuff[50],Rxnum;
unsigned char	Recwifi_data[5];
char *strx=0;
char clinetid;//连接ID
unsigned char getflag;//获取标志
unsigned char xdata Line1[]="STATUS:  OPEN   ";
unsigned char xdata Line2[]="USER:    001    ";
unsigned char count = 0;                      
unsigned char SEV_count = 15;                                 
unsigned char SEH_count = 15;                      /* ?????????T0?T1??? */

char MIMA[6] = {'9','9','9','9','9','9'};
unsigned char hw_Flag = 0;
unsigned char lock_Flag = 0;
unsigned int num = 0;
void InitUART(void)			 //这是串口的基本配置，配置他的波特率是9600.这些参数都是标准的。
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

 void Clear_Buffer(void)//清空缓存
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
 * UART 发送一字节
*/
void UART_send_byte(char dat)
{
		SBUF = dat;
		while (TI == 0);
		TI = 0;
}

/*
 * UART 发送字符串
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

void ESP8266_SERVER(void)//服务器建立
{

		Send_Str("AT+RST\r\n");             //ESP8266复位
		Printfclear();
		Send_Str("AT\r\n");
		Printfclear();
		Send_Str("AT+RATE=9600\r\n");
		Printfclear();
		Send_Str("AT+AP=CMCC-GHPN,y7n2qrya\r\n");
		Printfclear();
		Send_Str("AT+DEVICEID=aqwsdr12347\r\n");		//设备ID aqwsdr12345 需要客户自己修改随机ID与其他用户不冲突
		Printfclear();
		Send_Str("AT+KEEPALIVE=120\r\n");        //MQTT存活期 0-3600秒
		Printfclear();
		Send_Str("AT+MQTTCLOUD=58.87.121.189,61613,admin,password\r\n");   //服务器IP:58.87.121.189 port:61613 mqtt账号:admin 密码：password
		Printfclear();
		Clear_Buffer();	
		Send_Str("AT+GSERVERSTATUS\r\n");//建立服务器
		
		delayms(500);	
		Send_Str(Rxbuff);
		delayms(50);
		strx=strstr((const char*)Rxbuff,(const char*)"ok");//等待建立完成
		while(strx==NULL)
		{		Clear_Buffer();
				Send_Str("AT+GSERVERSTATUS\r\n");//建立服务器
				delayms(500);
				Send_Str(Rxbuff);
				delayms(50);
				strx=strstr((const char*)Rxbuff,(const char*)"ok");//等待建立完成
		}
		Clear_Buffer();	
		Send_Str("AT+SUB=nydev/123456,0\r\n");               //订阅主题 dev/123456
		Printfclear();
}


void Get_Clinet(void)//判断是否获取到了客户端接入
{
		uchar i;
		strx=strstr((const char*)Rxbuff,(const char*)"AT+RECTOPIC=nydev/123456");//有数据返回
		if(strx)//正常获取数据
		{
				clinetid=strx[5];
				strx=strstr((const char*)Rxbuff,(const char*)",");//有数据返回
				for(i=0;i<8;i++)
				Recwifi_data[i]=strx[1+i];		  //获取手机对LED的控制
				if(Recwifi_data[0]=='0'&&Recwifi_data[1]=='0')//关锁
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
				else	if( Recwifi_data[0]=='1'&&Recwifi_data[1]=='1'		)//开锁
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
		delayms(1000);	  	//延时一段时间，让WIFI模块稳定
		
		LCD_Init();         //初始化液晶屏
		InitUART();					//初始化串口
		ESP8266_SERVER();		//初始化ESP8266
		LCD_Write_Command(0x01); //显示清屏
		LCD_Str(0,0,Line1);//写入第一行信息
		LCD_Str(0,1,Line2);//写入第二行信息
		while(1)	
		{	  
				delayms(50);
				Get_Clinet();
				if(num++>20)
				{
						num = 0;
						Tx_Buf[0]='T';	//帧头
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
						
						Send_DATA(Tx_Buf)	;//发送数据
				}	
		}
}

/*****************串口接收中断函数,接收蓝牙模块的数据*********************/
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
 