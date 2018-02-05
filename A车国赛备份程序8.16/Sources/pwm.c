#include "all.h"

/*************************************************************************
//功    能：直流电动机正转
//入口参数：unsigned int duty，占空比值
//出口参数：无
*************************************************************************/
void Motor_forward (float duty_l,float duty_r)//最大是2M/5000 
{
   //电机要亲自测试，这里可以触发中断，P,H,J口
  PWME_PWME4 = 1;						// 使能IN1
	PWME_PWME6 = 1;
  PWME_PWME5= 0;						// 禁止IN2
	PWME_PWME7= 0;	
  DDRP_DDRP5 = 1;						// PTP_PTP3为输出,考虑去掉
	DDRP_DDRP7 = 1;
  PTP_PTP5 = 0;						// 输出0
	PTP_PTP7 = 0;
  PWMDTY4 = duty_r;
  PWMDTY6 = duty_l;
}

/*************************************************************************
//功    能：直流电动机反转
//入口参数：unsigned int duty，占空比值
//出口参数：无
*************************************************************************/

void Motor_back(int duty)
{
   
	PWME_PWME5 = 1;						// 使能IN1
	PWME_PWME7 = 1;
  PWME_PWME4= 0;						// 禁止IN2
	PWME_PWME6= 0;	
  DDRP_DDRP4 = 1;						// PTP_PTP3为输出,考虑去掉
	DDRP_DDRP6 = 1;
  PTP_PTP4 = 0;						// 输出0
	PTP_PTP6 = 0;
  PWMDTY5 = duty;
	PWMDTY7 = duty;
}

/*************************************************************************
//功    能：直流电动机停车
//入口参数：无
//出口参数：无
*************************************************************************/
void Motor_brake(void)
{
    PWME_PWME4 = 0;
    PWME_PWME6 = 0;  		// 禁止特殊功能
    DDRP_DDRP4 = 1;
    DDRP_DDRP6 = 1;   		// 端口为输出
    PTP_PTP4 = 0;
    PTP_PTP6 = 0;       		// 端口输出0
}

void steering(int duty)
{
    PWMDTY01 = duty;
}	// 端口输出0


