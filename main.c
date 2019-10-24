#include <reg52.h>
#include <intrins.h>

typedef unsigned int uint;
typedef unsigned char uchar;
typedef unsigned long ulong;

sbit RS=P1^5;
sbit RW=P1^6;
sbit LED=P2^0;
sbit RCK=P2^6;
sbit SCK=P2^7;
sbit OE1=P2^5;
sbit OE2=P2^3;
sbit OE3=P2^2;
sbit OE4=P2^1;
sbit KEY1=P3^2;
sbit KEY2=P3^3;
sbit MOSIO=P2^4;
sbit lcden=P1^7;
sbit LEDGND1=P1^4;
sbit LEDGND2=P1^1;
sbit LEDGND3=P1^2;
sbit LEDGND4=P1^3;
sbit lcdData0=P1^0;

uchar ledMod=0;
uchar uartNum=0;
ulong keyNum=0;
uchar BREATHE=20;
uchar table2[]="MODEL ";
uchar table1[]="Zhenger.LEDCUBIC";

void keyInit();
void lcdInit();
void ledInit();
void uartInit();
void pwmPurple();
void delay(uint);
void breathe(uint);
void oneByOne(uint);
void writeCom(uchar);
void writeData(uchar);
void layerSwitch(uchar);
void hc595SendData(uint);

void main( )
{
	LED=1;
	keyInit();  //
	lcdInit();  //
	ledInit();  //
	uartInit(); //串口初始化
	LED=0;
	
    while(1)
    {
		hc595SendData(0x55);hc595SendData(0x55);hc595SendData(0x55);hc595SendData(0x55);
		hc595SendData(0xaa);hc595SendData(0xaa);hc595SendData(0xaa);hc595SendData(0xaa);
    }
}

void keyInit()
{
	EA=1;
	EX0=1;
	IT0=1;
	EX1=1;
	IT1=1;
}

void lcdInit()
{
	uchar a;
    RW=0;
	writeCom(0x38);   //显示模式设置：16×2显示，5×7点阵，8位数据接口
	delay(2);
	writeCom(0x0f);   //显示模式设置
	delay(2);
	writeCom(0x06);   //显示模式设置：光标右移，字符不移
	delay(2);
	writeCom(0x01);   //清屏幕指令，将以前的显示内容清除
	delay(2);

	writeCom(0x80);
	for(a=0;a<16;a++)
	{
		writeData(table1[a]);
		delay(1);
	}
	writeCom(0x80+0x40);
	for(a=0;a<6;a++)
	{
		writeData(table2[a]);
		delay(1);
	}
	writeData('0');
}

void ledInit()
{
	OE1=OE2=OE3=OE4=0;	
	LEDGND1=LEDGND2=LEDGND3=LEDGND4=0;
}

void uartInit()
{
	TMOD = 0x20;  	//T1工作模式2  8位自动重装
	TH1 = 0xfd;
	TL1 = 0xfd; 	//比特率9600
	TR1 = 1;		//启动T1定时器
	SM0 = 0;
	SM1 = 1; 		//串口工作方式1 10位异步
	REN = 1;		//串口允许接收
	EA  = 1;		//开总中断
	ES  = 1;		//串口中断打开
	PS  = 1;		//高优先级
}

void delay(uint t)
{
	uint X,Y;
	LED=1;
	for(Y=0;Y<t;Y++)
		for(X=50;X>0;X--);
	LED=0;
		for(Y=0;Y<t;Y++)
		for(X=50;X>0;X--);
}

void writeCom(uchar com)
{
	P0=com;
	lcdData0=com&1;
	RS=0;
	lcden=0;
	delay(1);
	lcden=1;
	delay(1);
	lcden=0;
}

void writeData(uchar d)
{
	P0=d;
	lcdData0=d&1;
	RS=1;
	lcden=0;
	delay(1);
	lcden=1;
	delay(1);
	lcden=0;
}

void oneByOne(uint t)
{
	uint i,j,k;
	for(k=1;k<16;k*=2)
	{
		layerSwitch(k);
		for(i=1;i<8;i++)
		{
			hc595SendData(1<<i);
			delay(t);
			for(j=1;j<4;j++){hc595SendData(0);delay(t);}
		}
		hc595SendData(1);
		delay(t);
		for(j=1;j<4;j++){hc595SendData(0);delay(t);}
	}
	hc595SendData(0);hc595SendData(0);hc595SendData(0);hc595SendData(0);
	ledInit();
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
	uchar temp;
	hc595SendData(num);
	hc595SendData(num);
	hc595SendData(num);
	hc595SendData(num);

	for(temp=0;temp<BREATHE;temp++)
	{
		OE1=OE2=OE3=OE4=1;
		delay(temp);
		OE1=OE2=OE3=OE4=0;
		delay(BREATHE-temp);
	}
	for(temp=BREATHE;temp>0;temp--)
	{
		OE1=OE2=OE3=OE4=1;
		delay(temp);
		OE1=OE2=OE3=OE4=0;
		delay(BREATHE-temp);
	}
}

void hc595SendData(uint SendVal)
{
    unsigned char i;
	LED=1;
    for(i=0;i<8;i++)
    {
        if((SendVal<<i)&0x80)MOSIO=1;
        else MOSIO=0; 
		SCK=0;
        _nop_();_nop_();
        SCK=1;
    }
	RCK=0;
	_nop_();_nop_();
	RCK=1;
	LED=0;
}

void pwmPurple()
{
	uint i,j;
	for(j=0;j<5;j++)
	{
		for(i=1;i<BREATHE;i++)
		{
			hc595SendData(0x55);hc595SendData(0x55);hc595SendData(0x55);hc595SendData(0x55);
			delay(i);
			hc595SendData(0xaa);hc595SendData(0xaa);hc595SendData(0xaa);hc595SendData(0xaa);
			delay(BREATHE-i);
		}
		delay(100);
		for(i=BREATHE;i>0;i--)
		{
			hc595SendData(0x55);hc595SendData(0x55);hc595SendData(0x55);hc595SendData(0x55);
			delay(i);
			hc595SendData(0xaa);hc595SendData(0xaa);hc595SendData(0xaa);hc595SendData(0xaa);
			delay(BREATHE-i);
		}
		delay(100);
	}
}

void key1Scan() interrupt 0
{
	if(KEY1==0)
	{
		delay(5);
		LEDGND1=LEDGND2=LEDGND3=LEDGND4=1;
		if(KEY1==0)
		{
			ES=0;
			SBUF = ledMod;	
			while(!TI);
			TI = 0;
			ES=1;
		}
		while(!KEY1||!KEY2);
		LEDGND1=LEDGND2=LEDGND3=LEDGND4=0;
	}
}

void key2Scan() interrupt 2
{
	uchar i;
	if(KEY2==0)
	{
		delay(5);
		if(KEY2==0)
		{
			switch(keyNum%4)
			{
				case 0:
				{
					ledMod=1;
					writeCom(0x80+0x46);
					writeData(ledMod+'0');
					keyNum++;
					for(i=1;i<5;i++)
					{
						breathe(0xaa);
						delay(500);
					}
					break;
				}
				case 1:
				{
					ledMod=2;
					writeCom(0x80+0x46);
					writeData(ledMod+'0');
					keyNum++;
					for(i=1;i<5;i++)
					{
						delay(500);
						breathe(0x55);
					}
					break;
				}
				case 2:
				{
					ledMod=3;
					writeCom(0x80+0x46);
					writeData(ledMod+'0');
					keyNum++;
					oneByOne(300);
					break;
				}
				case 3:
				{
					ledMod=4;
					writeCom(0x80+0x46);
					writeData(ledMod+'0');
					keyNum++;
					pwmPurple();
					break;
				}
			}
			SBUF = keyNum%4+1;	
			while(!TI);
			TI = 0;					
		}
		while(!KEY1||!KEY2);
		writeCom(0x80+0x46);
		writeData('0');
	}
}

void uart() interrupt 4
{
	
	uchar i;
	if(RI)
	{
		ledMod=SBUF;
		RI = 0;
		switch(ledMod%5)
		{
			case 0:
			{
				ledMod=0;
				writeCom(0x80+0x46);
				writeData(ledMod+'0');
				hc595SendData(0x55);hc595SendData(0x55);hc595SendData(0x55);hc595SendData(0x55);
				hc595SendData(0xaa);hc595SendData(0xaa);hc595SendData(0xaa);hc595SendData(0xaa);
				break;
			}
			case 1:
			{
				ledMod=1;
				writeCom(0x80+0x46);
				writeData(ledMod+'0');
				keyNum++;
				for(i=1;i<5;i++)
				{
					breathe(0xaa);
					delay(500);
				}
				break;
			}
			case 2:
			{
				ledMod=2;
				writeCom(0x80+0x46);
				writeData(ledMod+'0');
				keyNum++;
				for(i=1;i<5;i++)
				{
					delay(500);
					breathe(0x55);
				}
				break;
			}
			case 3:
			{
				ledMod=3;
				writeCom(0x80+0x46);
				writeData(ledMod+'0');
				oneByOne(300);
				break;
			}
			case 4:
			{
				ledMod=4;
				writeCom(0x80+0x46);
				writeData(ledMod+'0');
				pwmPurple();
				break;
			}
		}
	}
}

/*
***STC89C52
* 1-P1.0-1602DATA0
* 2-P1.1-LEDGND2
* 3-P1.2-LEDGND3
* 4-P1.3-LEDGND4
* 5-P1.4-LEDGND1
* 6-P1.5-1602RS
* 7-P1.6-1602RW
* 8-P1.7-1602EN
* 9-RST-KEY0
* 10-RXD-CH340RXD
* 11-TXD-CH340TXD
* 12-P3.2-KEY1
* 13-P3.3-KEY2
* 18-XTAL1
* 19-XTAL2
* 20-GND
* 21-P2.0-LEDWHITE
* 22-P2.1-595OE4_13
* 23-P2.2-595OE3_13
* 24-P2.3-5950E2_13
* 25-P2.4-595DS_14
* 26-P2.5-595OE1_13,HIGH-OFF
* 27-P2.6-595RCK_12,UP-LINEMOVE
* 28-P2.7-595SCK_11,UP-BITMOVE
* 29-PSEN-595SCLR_10,ALWAYSHIGH

* 32-P0.7-1602DATA7
* 33-P0.6-1602DATA6
* 34-P0.5-1602DATA5
* 35-P0.4-1602DATA4
* 36-P0.3-1602DATA3
* 37-P0.2-1602DATA2
* 38-P0.1-1602DATA1
* 40-VCC
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
