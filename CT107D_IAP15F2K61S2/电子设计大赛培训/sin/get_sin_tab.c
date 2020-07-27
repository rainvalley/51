#include<math.h>
#include<stdio.h>

int sin_tab[60];
void getSinTab(int point,int maxnum)
{
	int i=0;
	float x;
	float jiao;
	jiao=360.000/point;
	for(i=0;i<point;i++)
	{
		x=jiao*i;
		x=x*0.01744;
		sin_tab[i]=(maxnum/2)*sin(x)+(maxnum/2);
	}
}

int main()
{
	getSinTab(60,256);
	int i=0;
	for(i;i<60;i++)
	{
		printf("%d,",sin_tab[i]);	
	}
}
