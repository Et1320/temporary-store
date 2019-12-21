//DHT11.c 文件
#ifndef __DHT11_H__
#define __DHT11_H__
#include<reg52.h>
#include<Time_Delay.h>
sbit bit11=P3^4;
unsigned char U8T_data_H,U8T_data_L,U8RH_data_H,U8RH_data_L,U8checkdata;
unsigned char F16T,F16RH;// read 8 bits onice
//--------------------------------
//-----温湿度读取子程序 ------------
//--------------------------------
//----以下变量均为全局变量--------
//----温度高 8 位== U8T_data_H------
//----温度低 8 位== U8T_data_L------
//----湿度高 8 位== U8RH_data_H-----
//----湿度低 8 位== U8RH_data_L-----
//----校验 8 位 == U8checkdata-----
//----调用相关子程序如下----------
//---- Delay();, Delay_10us();,COM();
//--------------------------------
char  COM(void)
	{
	//用于最终读取的温湿度数据
	char i,U8temp,U8comdata;
	for(i=0;i<8;i++)
		{
		while(!bit11);
		//表示读取的高电位延时大于 20 多 us 则读取的是 1 否则读取的是0
		Delay_us(35);
		U8temp=0;
		//通过 U8FLAG可判断bit11 显示数据的脉长
		if(bit11)U8temp=1;
		while(bit11);
		U8comdata<<=1;
		U8comdata|=U8temp;
		}//rof
	return    U8comdata;
	} 

void getDHT11(void)
{//主机拉低 18ms
GO1:bit11=0;
Delay_ms(20);
bit11=1;//总线由上拉电阻拉高 主机延时 20us
Delay_us(60); //主机设为输入
//  bit11=1;判断从机响应信号
//判断从机是否有低电平响应信号
if(!bit11)       //T !
{//如不响应则跳出，响应则向下运行
while(!bit11);
//wait DHT goto high
while(bit11);
//数据接收状态
U8RH_data_H=COM();
U8RH_data_L=COM();
U8T_data_H=COM();
U8T_data_L=COM();
U8checkdata=COM();
bit11=1;
//数据校验
if((U8T_data_H+U8T_data_L+U8RH_data_H+U8RH_data_L)!=U8checkdata)
goto  GO1;
}//fi
F16T=U8T_data_H+U8T_data_L/256;     //change integer to float
F16RH=U8RH_data_H+U8RH_data_L/256;
}
#endif