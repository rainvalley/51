在之前的按键扫描程序中，按键一直按下时会进入死循环并占用机器时间。此时MCU无法处理其他事务，会造成单片机与外设的通信出现问题，因此考虑修改程序并加入长按功能。

# 核心算法
```
uchar trg=0;
uchar cont=0;
void Scan_Key_4(void)
{
	uchar read_data = KEY^(0xff); 
	trg = read_data&(read_data^cont); 
	cont = read_data; 
}
```

# 解释
在核心算法中，trg（Triger）代表触发，cont（Continue）代表连续按下。该程序能做到按下按键时trg对应位立刻变化为1，当按住按键时trg在第二次扫描时翻转为0，即仅在单次触发时响应。当按住按键时，cont为1，松开为0。

<iframe src="//player.bilibili.com/player.html?aid=499022980&bvid=BV1SK411J7NB&cid=218834984&page=1" scrolling="no" border="0" frameborder="no" framespacing="0" allowfullscreen="true"> </iframe>

* 假定单次按下P3^0对应按键，扫描时KEY=0xfe，read_data=0x01，trg=0x01，cont=0x01。
* 假定长按P3^0对应按键，第一次扫描KEY=0xfe，read_data=0x01，trg=0x01，cont=0x01；第二次扫描时KEY=0xfe，read_data=0x01，trg=0x00，cont=0x01。当按键不松开时trg恒等于0，cont恒等于1，即验证前文所述：仅在单次触发时响应。
* 假定按下的是其他按键，例如P3^1，当trg=0x02，cont=0x02时即代表响应。

即：cont与按键状态相同，trg仅在按下时变化为1。

# 应用
将按键扫描函数放入中断服务函数，定时扫描，如果需要长按的话`Key_Fun();`也需要放入中断以便计数。
* 短按键触发
```
if(trg&0x01)
{
    Fun();
}
```
* 仅按下时触发
```
if(cont&0x01)
{
    Fun();
}
```
* 长按2s触发
```
void Key_Fun()
{
	static uchar count = 0;
	if(cont&0x01) //当S7长按时开始计数count，50次（1s）后运行Fun()
	{
		count++;
		if(count==50)
		{
			Fun();
			count=0;
		}
	}
	else //当S7非长按时清零count，避免运行时计数混乱
	{
		count=0;
	}
}
```

完整代码如下：
```
#include<stc15f2k60s2.h>
typedef unsigned int uint;
typedef unsigned char uchar;
uchar code dig_code[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0x88,0x80,0xc6,0xc0,0x86,0x8e,0xbf,0x7f,0xff};
uchar trg=0;
uchar cont=0;
uchar dis=0;
void Key_Fun();

void Delay(uint t)
{
	while(t--);
}

void Select(uchar channel)
{
	switch(channel)
	{
		case 4: 
			P2=(P2&0X1f)|0X80;//不改变P2其他IO口电平的情况下改变138译码器输入
			break;
		case 5: 
			P2=(P2&0X1f)|0Xa0;
			break;
		case 6: 
			P2=(P2&0X1f)|0Xc0;
			break;
		case 7: 
			P2=(P2&0X1f)|0Xe0;
			break;
	}
		
}

void Display(uchar pos,uchar dig)
{
	Delay(200);
	P0=0XFF;
	Select(6);
	P0=0X01<<pos;
	Select(7);
	P0=dig_code[dig];
}

void Init_Sys()
{
	Select(5);
	P0=0X00;
	Select(4);
	P0=0XFF;
}

void Timer0Init(void)		//50毫秒@11.0592MHz
{
	AUXR &= 0x7F;		//定时器时钟12T模式
	TMOD &= 0xF0;		//设置定时器模式
	TL0 = 0x00;		//设置定时初值
	TH0 = 0x4C;		//设置定时初值设置定时初值
	TF0 = 0;		//清除TF0标志
	TR0 = 1;		//定时器0开始计时
	ET0 = 1;
	EA = 1;
}
/* 异或：相同时为0，不同时为1；假设P3^0按下
单次按下：
read_data=1；trg=1；cont=1；
长按：
read_data=1，trg=0，cont=1；
*/

void Scan_Key_4(void)
{
	uchar read_data = P3^(0xff); //读取到低四位中电平变化为0（按下）的位并赋值为1
	trg = read_data&(read_data^cont); //与长按标志位取异或后相与，做到长按第一次扫描时trag=1，第二次扫描时trag=0
	cont = read_data; //直接取read_data的值，按下时恒为1（第一步中已做处理）
}

void Ser_Timer0() interrupt 1
{
	Scan_Key_16(); //独立按键扫描
	Key_Fun(); //按键功能实现
}

void Key_Fun()
{
	static uchar count = 0;
	if(cont&0x01) //当S7长按时开始计数count，20次（1s）后取反dis
	{
		count++;
		if(count==20)
		{
			dis=~dis;
			count=0;
		}
	}
	else //当S7非长按时清零count，避免运行时计数混乱
	{
		count=0;
	}
}

int main()
{
	Init_Sys();
	P3 |= 0X0F;
	Timer0Init();
	while(1)
	{
		Display(0,cont_row);
		Display(1,cont_column);
		if(dis)
		{
			Display(2,2);
		}
	}
}
```

# 其他
没错，这个按键扫描程序中并没有考虑抖动（一次按键判定为多次或者漏判）。当扫描时间增加到50ms时，抖动的影响几乎可以忽略，因为此时的扫描时长远大于抖动可能出现的时间。我也写了一个实时扫描的对比程序，然而在我的验证过程中并没有出现误判的情况。