#ifndef _MNPWM_H_
#define _MNPWM_H_
#include ".\Public\CH549.H"

/*引脚定义*/
sbit motor_left_1 = P2^2;
sbit motor_left_2 = P2^3;
sbit motor_right_1 = P2^0;
sbit motor_right_2 = P2^1;

/*  可调用函数*/
void MOTOR_LEFT_Forward(int a);//左轮正转
void MOTOR_RIGHT_Foreward(int a);   //右轮正转
void MOTOR_LEFT_Stop(void);   //左轮停
void MOTOR_RIGHT_Stop(void);   //右轮停
void MOTOR_LEFT_Inversion(int a);  //左轮反转
void MOTOR_RIGHT_Inversion(int a);  //右轮反转

#endif