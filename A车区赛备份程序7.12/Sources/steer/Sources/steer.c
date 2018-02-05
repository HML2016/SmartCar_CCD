#include"all.h"
int steer_control_out_max=3370; //2550舵机最大值（（改））
int steer_control_out_min=2680;//1350;舵机最小值（（改））
float steer_out_last=3035;
float steer_control_D;//42  65;    
float steer_control_P;//29;//29.9 27.9;//33.9;
extern uint8_t RoadType;
extern int  Right_Or_Left;
extern float steer_out_this;//中间位置
extern float  Middle_Err;
extern float  Previous_Error[15];
float Delt_error;
float Delta_P;
float Delta_D;
float Fuzzy_Kp;//0.15
float Fuzzy_Kd;
float TURNP;
float TURND;
void steer_pd(void) //舵机PD算法
{    
static int Calculate_Length=0;       
      if(Calculate_Length<6) 
     {  
                 Calculate_Length++;
                 Delta_P=0;
                 Delt_error=0;
                 Delta_D=0;
      }
          else 
     {  
          Delt_error=-10*Slope_Calculate(0,Calculate_Length,Previous_Error);
          Delta_P=Fuzzy(Middle_Err,Delt_error)*Fuzzy_Kp;
          Delta_D=Fuzzy(Middle_Err,Delt_error)*Fuzzy_Kd;
       }        
      steer_control_P=(TURNP+Delta_P)*Middle_Err;
      steer_control_D=(TURND+Delta_D)*(Middle_Err-steer_out_last);
      steer_out_this=STEER_MID+steer_control_P+steer_control_D;  
      if(steer_out_this>=steer_control_out_max) //舵机超过最大范围
      {                                          
          steer_out_this=steer_control_out_max;
      }                                                                
      else if(steer_out_this<=steer_control_out_min) //舵机小于最小范围
      {
         steer_out_this=steer_control_out_min;
      } 
      steering(steer_out_this);//操控舵机占空比
      steer_out_last=Middle_Err;
}

