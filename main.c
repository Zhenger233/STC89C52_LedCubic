#include <reg51.h>
#include <intrins.h>
#define NOP() _nop_()

sbit MOSIO=P3^4;//pin14
sbit R_CLK=P3^5;//pin12
sbit S_CLK=P3^6;//pin11
sbit OE=P3^7;

//void fakeDelay(unsigned int i);
void HC595SendData(unsigned char SendVal);
void pwm(char);
void tim0init();
void delay(unsigned int);

unsigned int pp=0;
unsigned char Led=0xff;
char i;

void main( )
{
	tim0init();


    while(1)
    {
			HC595SendData(Led);
	HC595SendData(Led);
        
    }
}

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
//void fakeDelay(unsigned int i)
//{ 
//	int temp=0;
//    unsigned int j;
//    for(i;i>0;i--)
//        temp++;
//}

void delay(unsigned int i)
{
	unsigned int temp=pp;
	while(1)
	{
		if((pp==temp+i)%60000)break;
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
			S_CLK=0;
        NOP();
        NOP();
        S_CLK=1;
			delay(20);
    }

    R_CLK=0;
    NOP();
    NOP();
    R_CLK=1;
}


void tim0init()
{
	TMOD=0x01;		 //模式设置，00000001，可见采用的是定时器0，工作与模式1（M1=0，M0=1）。
	TR0=1;			 //打开定时器
	TH0=(65536-102)/256;		 //定时器设置，每隔100微秒发起一次中断。
	TL0=(65536-102)%256;
	ET0=1;			 //开定时器0中断
	EA=1;			 //开总中断
}

void time0() interrupt 1
{	TH0=(65536-102)/256;//1ms
	TL0=(65536-102)%256;
	pp++;
	if(pp==60000)pp=0;
}