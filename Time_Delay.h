//延时函数文件
//以下为延时函数  this is fit to old C51 12MHz,12 devide freqency 
#ifndef __Time_Delay_H__
#define	__Time_Delay_H__
#include<reg52.h>
#include <intrins.h>
void Delay_ms(unsigned int n)//n 毫秒延时 
{
unsigned char j;
	while(n--)
	{
		for(j=0;j<125;j++);
	}
}
void Delay_us(unsigned char n)
	{
		n=n/2;
		while(--n);
	}//N us 延时函数
#endif