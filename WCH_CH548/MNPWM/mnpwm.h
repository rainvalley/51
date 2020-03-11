#ifndef _MNPWM_H_
#define _MNPWM_H_
#include ".\Public\CH549.H"

/*���Ŷ���*/
sbit motor_left_1 = P2^2;
sbit motor_left_2 = P2^3;
sbit motor_right_1 = P2^0;
sbit motor_right_2 = P2^1;

/*  �ɵ��ú���*/
void MOTOR_LEFT_Forward(int a);//������ת
void MOTOR_RIGHT_Foreward(int a);   //������ת
void MOTOR_LEFT_Stop(void);   //����ͣ
void MOTOR_RIGHT_Stop(void);   //����ͣ
void MOTOR_LEFT_Inversion(int a);  //���ַ�ת
void MOTOR_RIGHT_Inversion(int a);  //���ַ�ת

#endif