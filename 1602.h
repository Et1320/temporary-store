#ifndef _1602_H_
#define _1602_H_

/***********************LCD1602�������Ŷ���**************************/
#define LCD_IO P0 //����P0����LCD1602�����ݿ����
sbit LCD_RS = P2^5; //���ݡ�����ѡ��� 1Ϊ����
sbit LCD_RW = P2^4; //����дѡ��� 1Ϊ��
sbit LCD_EN = P2^3; //ʹ�ܿ���
sbit LCD_BUSY=LCD_IO^7; //æµ��־λ
/**********************������صĶ�*********************************/
#define HIGH 1
#define LOW 0 
#define TURE 1
#define FALSE 0
#define uchar unsigned char
#define uint unsigned int
/**********************���뷽ʽ����**************************** */
#define LCD_AC_AUTO_INCREMENT 0x06 //���ݶ���д������,AC�Զ���һ
#define LCD_AC_AUTO_DECREASE 0x04 //���ݶ���д������,AC�Զ���һ
#define LCD_MOVE_ENABLE 0x05 //���ݶ���д����,����ƽ��
#define LCD_MOVE_DISENABLE 0x04 //���ݶ���д����,���治��
#define LCD_GO_HOME 0x02 //AC=0,��ꡢ�����HOMEλ
//������ʾ����꼰��˸������
#define LCD_DISPLAY_ON 0x0C //��ʾ��
#define LCD_DISPLAY_OFF 0x08 //��ʾ��
#define LCD_CURSOR_ON 0x0A //�����ʾ
#define LCD_CURSOR_OFF 0x08 //��겻��ʾ
#define LCD_CURSOR_BLINK_ON 0x09 //�����˸
#define LCD_CURSOR_BLINK_OFF 0x08 //��겻��˸
//��ꡢ�����ƶ�,��Ӱ��DDRAM
#define LCD_LEFT_MOVE 0x18 //LCD��ʾ����һλ
#define LCD_RIGHT_MOVE 0x1C //LCD��ʾ����һλ
#define LCD_CURSOR_LEFT_MOVE 0x10 //�������һλ
#define LCD_CURSOR_RIGHT_MOVE 0x14 //�������һλ
//������ʽ����
#define LCD_DISPLAY_DOUBLE_LINE 0x38 //������ʾ��8λ���ݶ˿�
#define LCD_DISPLAY_SINGLE_LINE 0x30 //������ʾ
#define LCD_CLEAR_SCREEN 0X01 //����
/***********************LCD1602��ַ���******************************/
#define LINE1_HEAD 0x80 //��һ��DDRAM��ʼ��ַ 
#define LINE2_HEAD 0xc0 //�ڶ���DDRAM��ʼ��ַ
#define LINE1 0 //��һ��
#define LINE2 1 //�ڶ���
#define LINE_LENGTH 16 //ÿ�е�����ַ�����


/*************************�����Ǻ�����������*************************/
extern void LCD_Init(void); //LCD1602��ʼ��
extern void LCD_Write_Command(uchar); // д����
extern void LCD_Write_Data(uchar); //д����

extern void LCD_Flash(uchar); //LCD ������˸
extern void LCD_Char(uchar, uchar, uchar); //�ڵڼ��еڼ�����ʾһ���ַ�
extern void LCD_Str(uchar, uchar, char *); //�ӵڼ��еڼ��п�ʼ��ʾһ���ַ���
void LCD_Num(uchar x,uchar y,uint dataw);  

extern void yanshi(uchar); //�ӳ� ��
extern void DelayUs(uchar); //�ӳ� ΢��
extern void LCD_Check_Busy(void); //LCD æµ���


/******************************************************************/
#endif
