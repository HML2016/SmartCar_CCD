#include"all.h"
int steer_control_out_max=3370; //2550������ֵ�����ģ���
int steer_control_out_min=2680;//1350;�����Сֵ�����ģ���
float steer_out_last=3035;
float steer_control_D;//42  65;    
float steer_control_P;//29;//29.9 27.9;//33.9;
extern uint8_t RoadType;
extern int  Right_Or_Left;
extern float steer_out_this;//�м�λ��
extern float  Middle_Err;
extern float  Previous_Error[15];
float Delt_error;
float Delta_P;
float Delta_D;
float Fuzzy_Kp;//0.15
float Fuzzy_Kd;
float TURNP;
float TURND;
void steer_pd(void) //���PD�㷨
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
      if(steer_out_this>=steer_control_out_max) //����������Χ
      {                                          
          steer_out_this=steer_control_out_max;
      }                                                                
      else if(steer_out_this<=steer_control_out_min) //���С����С��Χ
      {
         steer_out_this=steer_control_out_min;
      } 
      steering(steer_out_this);//�ٿض��ռ�ձ�
      steer_out_last=Middle_Err;
}

