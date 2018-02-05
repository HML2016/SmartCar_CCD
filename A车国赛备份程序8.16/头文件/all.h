#include"math.h"
#include"sci.h"
#include"pwm.h"
#include"ccd.h"
#include"def.h"
#include"control.h"
#include"init.h"
#include"chuli.h"
#include"motor.h"
#include"steer.h"
#include<hidef.h>      /* common defines and macros */
#include"derivative.h"      /* derivative-specific definitions */
#include<string.h>
#include<stdlib.h>
/* LED1 */
#define LED1_DDR            DDRK_DDRK3
#define LED1                PORTK_PK3
/* LED2 */
#define LED2_DDR            DDRK_DDRK7
#define LED2                PORTK_PK7
/* 函数执行时间测试*/
#define Time_DDR            DDRK_DDRK1
#define Time                PORTK_PK1
#define Time1                PORTK_PK2
#define Time2                PORTK_PK3
#define Time3                PORTK_PK4
#define STEER_MID 3045//舵机中间值
#define std_yuzhi 17
#define got_yuzhi 30                       
extern uint M_PACNT;
extern unsigned char Pixel[128];
float average(int n,float *x);
float spfh(int n,float *x) ;
float shpf(int n,float *x);
float dcj(int n,float *x,float *y);
float djc(int n,float *x,float *y) ;
float xsa(int n,float *x,float *y) ;
float he(int n,float *y);
float xsb(int n,float *x,float *y,float a) ;
void update(float *f,int n,float input);
void bubble_sorts(unsigned char  a[],int start,int end);
/*extern char g_speed_read_count;
extern char g_speed_control_count;
extern char g_speed_control_flag;

extern float g_speed_pid_this;
extern float g_speed_pid_last;

extern float g_speed_out;

extern float g_speed_p;
extern float g_speed_i;
extern float g_speed_d;

extern float speed_sum_max,speed_sum_min;//误差积分的临界
extern float speed_out_max,speed_out_min;//pid的pwm输出的最大最小边界

extern float g_speed_err;
extern float g_speed_err_d;
extern float g_speed_err_pre;

extern float g_speed_new;
extern float g_speed_old ;
extern float g_speed_set;

extern float g_speed_control_period;
extern float g_speed_err_sum;*/
/************************************************************/
//舵机
/*extern char guodu,b_width;
extern char yuzhi;
extern char stop_flag;*/
//extern char b_average;//整条黑线检测的平均值
/*extern char s_yuzhi;
extern int g_line_old;
extern int g_line_new;
extern char g_steer_control_flag;
extern char g_steer_control_count;
 
extern float g_steer_err_old;	
extern float g_steer_err_new;

extern float d_steer_err;///////
extern float steer_control_D;
extern float steer_control_P;

extern float steer_out_old;
extern float steer_out_new;

extern float steer_control_out_max;
extern float steer_control_out_min;

extern float steer_control_out;//计数

extern float g_steer_control_period; //周期  */