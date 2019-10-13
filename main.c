/*
* 29-PSEN-595SCLR_10,ALWAYSHIGH
* 28-P2.7-595SCK_11,UP-BITMOVE
* 27-P2.6-595RCK_12,UP-LINEMOVE
* 26-P2.5-595OE1_13,HIGH-OFF
* 25-P2.4-595DS_14
* 24-P2.3-5950E2_13
* 23-P2.2-595OE3_13
* 22-P2.1-595OE4_13
* 09-RST-KEY0
* 12-P3.2-KEY1
* 13-P3.3-KEY2
*/
//#define TEST
#define INT int
#define UNSIGNED unsigned
#define CHAR char
#define SBIT sbit
#define VOID void
#define LONG long
#define IF if
#define FOR for
#define WHILE while
#define INTERRUPT interrupt
#include <reg52.h>
#include <intrins.h>
#define NOP() _nop_()

sbit KEY1=P3^2;
sbit KEY2=P3^3;
sbit RCK=P2^7;
sbit SCK=P2^6;
sbit OE1=P2^5;
#ifdef TEST
sbit OE2=P2^0;//TEMP FOR TEST!!
#else
sbit OE2=P2^3;
#endif
sbit OE3=P2^2;
sbit OE4=P2^1;
sbit MOSIO=P2^4;
sbit tempIO=P1^0;

void HC595SendData(unsigned char SendVal);
void tim0Init();
void delay(unsigned int);
void fakeDelay(unsigned int i);
void pwm(char);
void keyInit();

unsigned char led=0xff;
unsigned int tt=0;

void main( )
{
	tim0Init();
	keyInit();

	OE1=OE2=OE3=OE4=0;	

    while(1)
    {
		;
		
    }
	
}

void HC595SendData(unsigned char SendVal)
{
    unsigned char i;
    for(i=0;i<8;i++)
    {
        if((SendVal<<i)&0x80)MOSIO=1;
        else MOSIO=0;
				//MOSIO=1;  
			SCK=0;
        NOP();
        NOP();
        SCK=1;
		fakeDelay(2);
    }
    RCK=0;
    fakeDelay(1);
    RCK=1;
}

void keyInit()
{
	EA=1;
	EX0=1;
	IT0=1;
}

void tim0Init()
{
	TMOD=0x01;		 //模式设置，00000001，可见采用的是定时器0，工作与模式1（M1=0，M0=1）。
	TR0=1;			 //打开定时器
	TH0=(65536-102)/256;		 //定时器设置，每隔100微秒发起一次中断。
	TL0=(65536-102)%256;
	ET0=1;			 //开定时器0中断
	EA=1;			 //开总中断
}

void keyScan() interrupt 0
{
	if(KEY1==0||KEY2==0)
		{
			//fakeDelay(5);
			if(KEY1==0||KEY2==0)
			{
				HC595SendData(led);
			}
			while(!KEY1||!KEY2);
		}
}

void time0() interrupt 1
{	TH0=(65536-102)/256;//1ms
	TL0=(65536-102)%256;
	tt++;
	if(tt==60000)tt=0;
}

void delay(unsigned int i)
{
	unsigned int temp=tt;
	while(1)
	{
		if((tt==temp+i)%60000)break;
	}
}

void fakeDelay(unsigned int t)
{ 
	unsigned int i,j;
	for(i=t;i>0;i--)
		for(j=110;j>0;j--)NOP();
}
/*

void pwm(char a)
{
	//char i,temp=0;
//	for(i=1;i<=a;i++)
//	{
//		HC595SendData(Led);
//		HC595SendData(Led);
//	}
//	for(i=a;i<=100;i++)
//	{OE=1;temp++;}
//	OE=0;
	HC595SendData(Led);
	HC595SendData(Led);
	delay(a);
	OE=1;
	delay(100-a);
	OE=0;
}
*/