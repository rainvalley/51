# 准备
在前文关于C51基础的叙述中，强调了左移，右移，以及取反符号的使用，巧妙地使用如上位移符号能够极大简化程序的设计。

## 正向移动
计算`0xff << 1`与`0xff << 2`：
* 0xff << 1 = 1111 1111 << 1 = 1111 1110 = 0xfe（注意溢出的将会被舍去）
* 0xff << 2 = 1111 1111 << 2 = 1111 1100 = 0xfc 
* 将0xff（1111 1111）对应LED灯，LED灯应当出于全部熄灭的状态。
* 将0xfe（1111 1110）对应LED灯，仅最低位的LED灯将会被点亮（注意位与端口的对应关系）。
* 将0xfc（1111 1100）对应LED灯，8颗LED灯中的低两位将会被点亮，如下：

![LED](https://pic.rmb.bdstatic.com/bjh/0e4f265a91657ff4932a5106662fb6dc.png)

由此快速变化便有了正向流水灯的效果。

## 反向移动
计算`~(0xff << 1)`与`~(0xff << 2)`，参考前文正向移动的叙述（注意括号的优先级）：
* ~(0xff << 1) = ~(1111 1111 << 1) = 0000 0001 = 0x01
* ~(0xff << 2) = ~(1111 1111 << 2) = 0000 0011 = 0x03 
* 将0x00（0000 0000）对应LED灯，所有灯全部点亮。
* 将0x01（0000 0001）对应LED灯，最低一位的LED灯熄灭。
* 将0x03（0000 0011）对应LED灯，最低两位的LED灯熄灭。

多次循环后可有如下效果：
![LED反向](https://pic.rmb.bdstatic.com/bjh/f05cba93ce395460e312bcebcf00259b.png)

将正向流水灯与反向流水灯组合便可完成流水灯的设计，效果见[文末](#效果)。
# 程序
```
/*
	2020/7/26 22:55 --- RainCorn
	8颗LED灯均连接在P0端口，低电平点亮，通过位移运算符改变端口电平。
*/
#include <reg52.h>
typedef unsigned int uint;
typedef unsigned char uchar;

void delay(uchar t) //延时函数，通过不断的运行死循环占用机器时间，进而达到延时的效果。
{
	uchar i = 255;
	while (t--)
	{
		while (i--);
	}
}
void ledrunning()
{
	uchar i;
	for (i = 0; i < 8; i++)
	{
		P0 = 0xff << i; //左移函数，每次循环均将已点亮小灯向右移动一位
		delay(200);
	}
	for (i = 0; i < 8; i++)
	{
		P0 = ~(0xff << i); //取反
		delay(200);
	}
}

void main()
{
	while (1)
	{
		ledrunning();
	}
}
```
# 效果
<iframe src="//player.bilibili.com/player.html?aid=413947133&bvid=BV16V411B7fr&cid=216973139&page=1" scrolling="no" border="0" frameborder="no" framespacing="0" allowfullscreen="true"> </iframe>