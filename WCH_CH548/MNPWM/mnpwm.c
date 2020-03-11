#include "mnpwm.h"
#include ".\Public\CH549.H"
#include ".\Public\DEBUG.H"

/*左轮正转*/
void MOTOR_LEFT_Forward(int a)
{
  motor_left_1 = 0;
	 motor_left_2 = 1;
	 mDelaymS(a);
	 motor_left_1 = 0;
	 motor_left_2 = 0;
	 mDelaymS(100-a);
}
/*右轮正转*/
void MOTOR_RIGHT_Foreward(int a)
{
    motor_right_1 = 0;
	 motor_right_2 = 1;
	 mDelaymS(a);
	 motor_right_1 = 0;
	 motor_right_2 = 0;
	 mDelaymS(100-a);
}
/*左轮反转*/
void MOTOR_LEFT_Inversion(int a)
{
  motor_left_1 = 1;
	 motor_left_2 = 0;
	 mDelaymS(a);
	 motor_left_1 = 0;
	 motor_left_2 = 0;
	 mDelaymS(100-a);
}

/*右轮反转*/
void MOTOR_RIGHT_Inversion(int a)
{
  motor_right_1 = 1;
	 motor_right_2 = 0;
	 mDelaymS(a);
	 motor_right_1 = 0;
	 motor_right_2 = 0;
	 mDelaymS(100-a);
}
/*左轮停*/
void MOTOR_LEFT_Stop(void)
{
   motor_left_1 = 0;
	 motor_left_2 = 0;
}
/*右轮停*/
void MOTOR_RIGHT_Stop(void)
{
	 motor_right_1 = 0;
	 motor_right_2 = 0;
}


