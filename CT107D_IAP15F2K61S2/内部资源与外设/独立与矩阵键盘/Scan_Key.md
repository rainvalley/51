在之前的按键扫描程序中，使用Delay进行消抖，同时按键一直按下时会进入死循环并占用机器时间。此时MCU无法处理其他事务，会造成单片机与外设的通信出现问题，因此考虑修改程序并加入长按功能。

# 传统扫描
传统的扫描程序存在无意义占用系统资源的问题，对于实时性要求较高的系统不建议使用。
```
void Scan_Key()
{
	if(S7==0)
	{
        Delay(200); //消抖
        if(S7==0) //再次判断按键是否按下
        {
            count++; 
            while(S7==0) //避免按下按键不松开造成多次判断
            {
                Show_Count(count); //需要在死循环中加入数码管扫描程序
            }
        }
	}
}
```

# 状态机
![按键抖动](https://imgs.raincorn.top/imgs/20200801094450.png)
按键按下的全过程分为四个连续的状态：
* 按键未按下：`S7=1，S7_Down=0;`
* 按键按下的前沿抖动：`S7=1，S7_Down=0;`--->`S7=0，S7_Down=0;`
* 按键按下时：`S7=0，S7_Down=0;`-->`S7=0，S7_Down=1;`
* 按键松开：`S7=0，S7_Down=1;`-->`S7=1，S7_Down=1;`-->`S7=1，S7_Down=0;`
```
void Scan_Key()
{
    if(S7==0&&S7_Down==0) 
    {
        Delay(200);
        if(S7==0) //判断前沿抖动
        {
            S7_Down=1; //如果非抖动则S7_Down判断为1，即按下
        }
    }
    if(S7==1&&S7_Down==1) //按键松开并当S7_Down标志按下时，标志为松开按键
    {
        S7_Down=0;
    }
}
```
# 长短按扫描
优点是简短且高校，缺点是缺少消抖（可以通过增加扫描延迟到50ms解决），且程序可读性很差，不便于维护。
## 核心算法
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

## 解释
在核心算法中，trg（Triger）代表触发，cont（Continue）代表连续按下。该程序能做到按下按键时trg对应位立刻变化为1，当按住按键时trg在第二次扫描时翻转为0，即仅在单次触发时响应。当按住按键时，cont为1，松开为0。

<iframe src="//player.bilibili.com/player.html?aid=499022980&bvid=BV1SK411J7NB&cid=218834984&page=1" scrolling="no" border="0" frameborder="no" framespacing="0" allowfullscreen="true"> </iframe>

* 假定单次按下P3^0对应按键，扫描时KEY=0xfe，read_data=0x01，trg=0x01，cont=0x01。
* 假定长按P3^0对应按键，第一次扫描KEY=0xfe，read_data=0x01，trg=0x01，cont=0x01；第二次扫描时KEY=0xfe，read_data=0x01，trg=0x00，cont=0x01。当按键不松开时trg恒等于0，cont恒等于1，即验证前文所述：仅在单次触发时响应。
* 假定按下的是其他按键，例如P3^1，当trg=0x02，cont=0x02时即代表响应。

即：cont与按键状态相同，trg仅在按下时变化为1。

## 应用
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

# 状态转化
定时器定时2ms，每次中断扫描一次，并将按键的扫描值写入keybuff。8次扫描过后，如果keybuff全为1，则认为在16ms内按键状态均为松开，判断按键未按下；反之，当keybuff全为0则判断为按下，其他状态判断为抖动。

将按键的过程判断（按下，松开，抖动）转化为状态判断，即将时间点判断转化为时间段判断，判断时间段内的高低电平来去除某个时间点上抖动的影响，参考自[此](https://blog.csdn.net/Xiaomo_haa/article/details/86648920)。

## 独立按键
```
void Scan_Key_4(void)
{
	uint i;
	static uchar keybuff[] = {0xff,0xff,0xff,0xff};
	keybuff[0] = (keybuff[0]<<1)|S4;
	keybuff[1] = (keybuff[1]<<1)|S5;
	keybuff[2] = (keybuff[2]<<1)|S6;
	keybuff[3] = (keybuff[3]<<1)|S7;
	for(i = 0;i < 4;i ++)
	{
		if(keybuff[i] == 0xff)		//连续扫描8次都是1,40ms内都是弹起状态，按键已松开
		{
			KeyStat[i] = 1;
		}
		else if(keybuff[i] == 0x00)	//连续扫描8次都是0，40ms内都是按下状态，按键已按下
		{
			KeyStat[i] = 0;
		}
	}
}
```
## 矩阵按键
![](https://imgs.raincorn.top/imgs/20200801104754.png)
```
sbit KEY_IN_1 = P4^4;
sbit KEY_IN_2 = P4^2;
sbit KEY_IN_3 = P3^5;
sbit KEY_IN_4 = P3^4;
sbit KEY_OUT_1 = P3^0;
sbit KEY_OUT_2 = P3^1;
sbit KEY_OUT_3 = P3^2;
sbit KEY_OUT_4 = P3^3;
void Scan_Key_16(void)
{
	uchar i;
	static uchar keyout = 0;		//矩阵按键扫描输出行索引
	static uchar keybuff[4][4] = {{0xff,0xff,0xff,0xff},{0xff,0xff,0xff,0xff},{0xff,0xff,0xff,0xff},{0xff,0xff,0xff,0xff}};	//矩阵按键扫描缓存区
	
	keybuff[keyout][0] = (keybuff[keyout][0] << 1) | KEY_IN_1;		//将每一行的4个按键值移入缓存区
	keybuff[keyout][1] = (keybuff[keyout][1] << 1) | KEY_IN_2;
	keybuff[keyout][2] = (keybuff[keyout][2] << 1) | KEY_IN_3;
	keybuff[keyout][3] = (keybuff[keyout][3] << 1) | KEY_IN_4;
		
	//消抖后更新按键状态
	for(i = 0;i < 4;i ++)
	{
		if((keybuff[keyout][i] & 0x0f) == 0x00)
			KeySta[keyout][i] = 0;		//连续4次扫描值都是0，即4×4ms内都是按下状态，认为按键已平稳按下
		else if((keybuff[keyout][i] & 0x0f) == 0x0f)
			KeySta[keyout][i] = 1;		//连续4次扫描值都是1，即4×4ms内都是松开状态，认为按键已稳定弹起
	}
	
	//执行下一次的扫描输出
	keyout ++;
	keyout = keyout & 0x03;		//索引加到4就归零
	switch(keyout)						//根据索引，释放当前输出引脚，拉低下次的输出引脚
	{
		case 0:KEY_OUT_4 = 1;KEY_OUT_1 = 0;break;
		case 1:KEY_OUT_1 = 1;KEY_OUT_2 = 0;break;
		case 2:KEY_OUT_2 = 1;KEY_OUT_3 = 0;break;
		case 3:KEY_OUT_3 = 1;KEY_OUT_4 = 0;break;
		default:break;
	}

}
```