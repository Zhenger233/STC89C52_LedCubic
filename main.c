/*
***STC89C52
* 2-P1.1-LEDGND2
* 3-P1.2-LEDGND3
* 4-P1.3-LEDGND4
* 5-P1.4-LEDGND1
* 6-P1.5-1602RS
* 7-P1.6-1602RW
* 8-P1.7-1602EN
* 9-RST-KEY0
* 12-P3.2-KEY1
* 13-P3.3-KEY2
* 18-XTAL1
* 19-XTAL2
* 20-GND
* 22-P2.1-595OE4_13
* 23-P2.2-595OE3_13
* 24-P2.3-5950E2_13
* 25-P2.4-595DS_14
* 26-P2.5-595OE1_13,HIGH-OFF
* 27-P2.6-595RCK_12,UP-LINEMOVE
* 28-P2.7-595SCK_11,UP-BITMOVE
* 29-PSEN-595SCLR_10,ALWAYSHIGH
* 31-VCC
* 32-P0.7-1602DATA7
* 33-P0.6-1602DATA6
* 34-P0.5-1602DATA5
* 35-P0.4-1602DATA4
* 36-P0.3-1602DATA3
* 37-P0.2-1602DATA2
* 38-P0.1-1602DATA1
* 39-P0.0-1602DATA0
*
***LCD1602
* 1-GND
* 2-VCC
* 3-VL
* 4-RS
* 5-RW
* 6-EN
* 7-DATA0
* 8-DATA1
* 9-DATA2
* 10-DATA3
* 11-DATA4
* 12-DATA5
* 13-DATA6
* 14-DATA7
* 15-VCC
* 16-GND
*
***74HC595
* 1-OUT0-LEFTRED
* 2-OUT1-RIGHTBLUE
* 3-OUT2-RIGHTRED
* 4-OUT3
* 5-OUT4
* 6-OUT5
* 7-OUT6
* 8-GND
* 9-STRINGOUT
* 10-MR-ALWAYSLOW
* 11-SCK
* 12-LCK
* 13-OE
* 14-DATAIN
* 15-OUT7
* 16-VCC
*/

#include <reg52.h>
#include <intrins.h>
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
#define NOP() _nop_()
#define mian main
typedef unsigned int uint;
typedef unsigned char uchar;
typedef unsigned long ulong;

sbit KEY1=P3^2;
sbit KEY2=P3^3;
sbit RCK=P2^6;
sbit SCK=P2^7;
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
sbit rs=P1^5;
sbit rw=P1^6;
sbit lcden=P1^7;
sbit LEDGND1=P1^4;
sbit LEDGND2=P1^1;
sbit LEDGND3=P1^2;
sbit LEDGND4=P1^3;
sbit LED=P2^0;

uchar table1[]="Hello world!";
uchar table2[]=" LEDCUBE ";
unsigned int led=0xff;
unsigned long tt=0;
unsigned long keyNum=0;
char keyNumString[20];
uchar a,g;
uchar modifyFlag=0;
uint temp=0;
unsigned char i;
uchar LEDBREATHE;

void HC595SendData(unsigned int SendVal);
void tim0Init();
//void fakeDelay(unsigned int i);
void pwm(char);
void keyInit();
char* itoa(unsigned long num,char* str,int radix);
void lcdInit();
void write_date(unsigned char date);
void write_com(uchar com);
void ledInit();
void delayT(long);
void delay(uint);
void light(char l,ulong d1,ulong d2,ulong d3,ulong d4);
void breathe(uint);
void oneByOne(uint);
void layerSwitch(uchar l);

void main( )
{
	LED=1;
	//tim0Init();
	keyInit();
	//lcdInit();
	ledInit();
	LED=0;
    while(1)
    {
		oneByOne(500);
    }
	
}

//void light(uchar l,ulong d1,ulong d2,ulong d3,ulong d4)
//{
//	send32(d1);
//	layerSwitch(l);
//	send32(d2);
//	layerSwitch(l);
//	send32(d3);
//	layerSwitch(l);
//	send32(d4);
//	layerSwitch(l);
//}

void delay(uint t)
{
	uint X,Y;
	LED=1;
	tempIO=1;
	for(Y=0;Y<t;Y++)
		for(X=50;X>0;X--);
	tempIO=0;
	LED=0;
		for(Y=0;Y<t;Y++)
		for(X=50;X>0;X--);
}

//void fakeDelay(uint t)
//{
//	uchar x;
//	for(t;t>=0;t--)
//		for(x=110;x>=0;x--)g++;
//}

void oneByOne(uint t)
{
	uint i,j,k;
	for(k=1;k<16;k*=2)
	{
		layerSwitch(k);
		for(i=1;i<8;i++)
		{
			HC595SendData(1<<i);
			delay(t);
			for(j=1;j<4;j++){HC595SendData(0);delay(t);}
		}
		HC595SendData(1);
		delay(t);
		for(j=1;j<4;j++){HC595SendData(0);delay(t);}
	}
}

void layerSwitch(uchar l)
{
	LEDGND1=!(l&1);
	LEDGND2=!(l&2);
	LEDGND3=!(l&4);
	LEDGND4=!(l&8);
}

void breathe(uint num)
{
	HC595SendData(num);
	HC595SendData(num);
	HC595SendData(num);
	HC595SendData(num);
		
	LEDBREATHE=20;
	for(temp=0;temp<LEDBREATHE;temp++)
	{
		OE1=OE2=OE3=OE4=1;
		delay(temp);
		OE1=OE2=OE3=OE4=0;
		delay(LEDBREATHE-temp);
	}
		for(temp=LEDBREATHE;temp>0;temp--)
	{
		OE1=OE2=OE3=OE4=1;
		delay(temp);
		OE1=OE2=OE3=OE4=0;
		delay(LEDBREATHE-temp);
	}
}

void ledInit()
{
	//uint a=0;
	OE1=OE2=OE3=OE4=0;	
	LEDGND1=LEDGND2=LEDGND3=LEDGND4=0;
	//for(a=1;a<=16;++a)HC595SendData(0xff);
}

void HC595SendData(unsigned int SendVal)
{
	
    unsigned char i;
	LED=1;
    for(i=0;i<8;i++)
    {
        if((SendVal<<i)&0x80)MOSIO=1;
        else MOSIO=0;
				//MOSIO=1;  
			SCK=0;
        NOP();NOP();
        SCK=1;
    }
	RCK=0;
	NOP();NOP();
	RCK=1;
	LED=0;
}

void keyInit()
{
	EA=1;
	EX0=1;
	IT0=1;
	EX1=1;
	IT1=1;
}



void key1Scan() interrupt 0
{
	char i;
		if(KEY1==0)
		{
			delay(5);
			if(KEY1==0)
			{
				//HC595SendData(0xff);
				if(keyNum&1)
				{
					keyNum++;
					for(i=1;i<5;i++)
					{
						breathe(0xaa);
						delay(500);
					}
						
				}
				else
				{
					keyNum++;
					for(i=1;i<5;i++)
					{
						delay(500);
						breathe(0x55);
					}
					
				}					
			}
			while(!KEY1||!KEY2);
		}
	

}

void key2Scan() interrupt 2
{
	//modifyFlag=1;
	if(KEY2==0)
		{
			delay(5);
			if(KEY2==0)
			{
				unsigned char i;
				for(i=0;i<8;i++)
				{
					MOSIO=0;  
					SCK=0;
					NOP();NOP();
					SCK=1;
				}
				RCK=0;
				NOP();NOP();
				RCK=1;
				keyNum++;
				keyNum++;
			}
			while(!KEY1||!KEY2);
		}
}

//void tim0Init()
//{
//	TMOD=0x00;		 //模式设置，00000001，可见采用的是定时器0，工作与模式1（M1=0，M0=1）。
//	TH0=(8192-921)/32;		 //定时器设置，每隔1ms发起一次中断。
//	TL0=(8192-921)%32;
//	TR0=1;			 //打开定时器
//	ET0=1;			 //开定时器0中断
//	EA=1;			 //开总中断
//}

//void time0() interrupt 1
//{	TH0=(8192-921)/32;		 //定时器设置，每隔5ms发起一次中断。
//	TL0=(8192-921)%32;
//	tt++;
//}



//void fakeDelay(unsigned int t)
//{ 
//	unsigned int i,j;
//	for(i=t;i>0;i--)
//		for(j=110;j>0;j--)NOP();
//}
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



//void delay(uint x)
//{
//	uint a,b;
//	for(a=x;a>0;a--)
//		for(b=10;b>0;b--); 
//}

//void write_com(uchar com)
//{
//	P0=com;
//	rs=0;
//	lcden=0;
//	delay(10);
//	lcden=1;
//	delay(10);
//	lcden=0;
//	
//}

//void write_date(uchar date)
//{
//	P0=date;
//	rs=1;
//	lcden=0;
//	delay(10);
//	lcden=1;
//	delay(10);
//	lcden=0;
//	
//}

//void lcdInit()
//{
//    rw=0;
//	write_com(0x38);   //显示模式设置：16×2显示，5×7点阵，8位数据接口
//	delay(20);
//	write_com(0x0f);   //显示模式设置
//	delay(20);
//	write_com(0x06);   //显示模式设置：光标右移，字符不移
//	delay(20);
//	write_com(0x01);   //清屏幕指令，将以前的显示内容清除
//	delay(20);	
//	
//		write_com(0x80);	 //将第一个字符写在向右偏移17个字符处，为后面的由右向左划入做准备。
//	delay(20);

//	for(a=0;a<14;a++)
//	{
//		write_date(table1[a]);
//		delay(20);		
//	}
//	
//	write_com(0xc0);
//	delay(50);
//	for(a=0;a<14;a++)
//	{
//		write_date(table2[a]);
//		delay(40);		
//	}
//}

//void delayT(long t)
//{
//	unsigned long temp=tt;
//	while(1)
//	{
//		if(tt>=(temp+t))break;
//	}
//	tt=0;
//}


//char* itoa(unsigned long num,char* str,int radix)
//{
//	char temp;//临时变量，交换两个值时用到
//	char index[]="0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";//索引表
//	unsigned unum;//存放要转换的整数的绝对值,转换的整数可能是负数
//	int i=0,j,k;//i用来指示设置字符串相应位，转换之后i其实就是字符串的长度；转换后顺序是逆序的，有正负的情况，k用来指示调整顺序的开始位置;j用来指示调整顺序时的交换。
// 
//	//获取要转换的整数的绝对值
//	if(radix==10&&num<0)//要转换成十进制数并且是负数
//	{
//		unum=(unsigned)-num;//将num的绝对值赋给unum
//		str[i++]='-';//在字符串最前面设置为'-'号，并且索引加1
//	}
//	else unum=(unsigned)num;//若是num为正，直接赋值给unum
// 
//	//转换部分，注意转换后是逆序的
//	do
//	{
//		str[i++]=index[unum%(unsigned)radix];//取unum的最后一位，并设置为str对应位，指示索引加1
//		unum/=radix;//unum去掉最后一位
// 
//	}while(unum);//直至unum为0退出循环
// 
//	str[i]='\0';//在字符串最后添加'\0'字符，c语言字符串以'\0'结束。
// 
//	//将顺序调整过来
//	if(str[0]=='-') k=1;//如果是负数，符号不用调整，从符号后面开始调整
//	else k=0;//不是负数，全部都要调整
// 
//	
//	for(j=k;j<=(i-1)/2;j++)//头尾一一对称交换，i其实就是字符串的长度，索引最大值比长度少1
//	{
//		temp=str[j];//头部赋值给临时变量
//		str[j]=str[i-1+k-j];//尾部赋值给头部
//		str[i-1+k-j]=temp;//将临时变量的值(其实就是之前的头部值)赋给尾部
//	}
// 
//	return str;//返回转换后的字符串
//}