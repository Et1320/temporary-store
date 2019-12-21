#include<reg52.h>
#include "1602.h"
#include<intrins.h>



void LCD_Init()
{
	yanshi(15); //�ӳ�15ms���ȴ�LCD��Դ�ȶ�,ʹ����빤��״̬
	LCD_IO = 0x00;
	LCD_Write_Command(LCD_DISPLAY_DOUBLE_LINE); 
	yanshi(5); 
	LCD_Write_Command(LCD_DISPLAY_DOUBLE_LINE); 
	yanshi(5); 
	LCD_Write_Command(LCD_DISPLAY_DOUBLE_LINE); //��ʾģʽ����Ϊ������ʾ��8λ���ݽӿڣ�5*8����
	yanshi(5);
	LCD_Write_Command(LCD_AC_AUTO_INCREMENT | LCD_MOVE_DISENABLE); //���ݶ���д������,AC�Զ���һ,���治��
	yanshi(5);
	LCD_Write_Command(LCD_DISPLAY_ON | LCD_CURSOR_OFF); //��ʾ������겻��ʾ
	yanshi(5);
	LCD_Write_Command(LCD_CLEAR_SCREEN); //���LCD��ʾ���� 
}
/************�ӳٺ���*******************************/
void DelayUs(uchar us)//delay us
{
	unsigned char uscnt;
	uscnt=us>>1; /* Crystal frequency in 12MHz*/
	while(--uscnt);
}
void yanshi(uchar ms)//delay Ms
{
	while(--ms)
{
	DelayUs(250);
	DelayUs(250);
	DelayUs(250);
	DelayUs(250);
}
}
/************LCD1602дָ��*******************************/
void LCD_Write_Command(uchar com)
{
	LCD_Check_Busy();
	LCD_RS = LOW;
	LCD_RW = LOW;
	_nop_(); //һ��_nop_();��һ���������ڣ���1us
	LCD_EN = HIGH;
	LCD_IO = com;
	LCD_EN = LOW;
}
/********************************************************/
/*****************LCD1602д����**************************/
void LCD_Write_Data(uchar dat)
{
	LCD_Check_Busy();
	LCD_RS = HIGH;
	LCD_RW = LOW;
	_nop_();
	LCD_EN=HIGH;
	LCD_IO=dat;
	LCD_EN=LOW;
}
/********************************************************/
/**********************��ʾһ���ֽ�**************************/
void LCD_Char(uchar x, uchar line, uchar dat) //�ӵ�x��ʼдһ���ֽ�
{
	unsigned char address;
	if (line == LINE1) // line=0��Ϊ��һ��
	address = LINE1_HEAD + x;
	else // ����Ϊ�ڶ���
	address = LINE2_HEAD + x;
	LCD_Write_Command(address); 
	LCD_Write_Data(dat);
}
/********************************************************/
/******************LCD1602��ʾ�ַ���*********************/
void LCD_Str(uchar x,uchar line,uchar *Str) //�ӵ�line�еĵ�xλ�ÿ�ʼ��ʾ�ַ���
{
	uchar i = x;
	
	if (line == LINE1)
	{
	while( *Str != '\0')
		LCD_Char(i++, 0, *Str++);
	}
	else 
	{
	while( *Str != '\0')
	LCD_Char(i++, 1, *Str++);
	}
}
/***************************LCDæµ״̬*******************************/
void LCD_Check_Busy(void) //���LCD״̬,�����ǲ��ǻ���æ��
{
	do
	{
	LCD_EN=0;
	LCD_RS=0;
	LCD_RW=1;
	LCD_IO=0xff;
	LCD_EN=1;
	}
	while (LCD_BUSY==1);
	
	LCD_EN=0;
}
/*****************��Ļ��˸********************/
void LCD_Flash(uchar time)
{ 
	//����ͣ��ʱ��
	LCD_Write_Command(LCD_DISPLAY_OFF); //�ر���ʾ
	yanshi(time); 
	//��ʱ
	LCD_Write_Command(LCD_DISPLAY_ON); //����ʾ
	yanshi(time);
	LCD_Write_Command(LCD_DISPLAY_OFF); //�ر���ʾ
	yanshi(time); 
	//��ʱ
	LCD_Write_Command(LCD_DISPLAY_ON); //����ʾ
	yanshi(time);
}
