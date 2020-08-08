#include<stdio.h>
typedef  union

{

    long  x[2];

    int     y[4];

    char  z[8];

}TRY;

TRY tr;

int main(  )

{

 printf("%d\n",sizeof(tr));

 return 0;
}