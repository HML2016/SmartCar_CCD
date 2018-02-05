#include"all.h"
float PID_P =6;//100//80  155    120 /**************p0.5,i0.75,d0.4time21s*///P 2.5
float PID_I =2;//100
float PID_D =4;//0.4
float error_r=0;      
float last_error_r=0; 
float pre_error_r=0;
float error_l=0;      
float last_error_l=0; 
float pre_error_l=0;                                                                
extern float speed;
extern uint8_t RoadType;
extern float superspeed;
extern float hillspeed;
extern float steer_out_this;
extern float pwmtemp_l,pwmtemp_r; 
extern float pwmtempl,pwmtempr;
extern float setspeed; 
extern float  Middle_Err;
extern int Current_speed;
extern int Current_speed1;
float Ri;
float Li;
uint8_t superflag=0;
uint8_t stopcnt=0;
void car_speed(void)     //增量式PID正常处理部分
{                               
    static int speed_cnt=0,flag_cnt=0;
    int count=0;
    int sha=0;
    float Li1,Ri1;
    if(RoadType!=3&&RoadType!=2&&RoadType!=1) {
        if(fabs(Middle_Err)<5) {
         speed_cnt++; 
          if(speed_cnt>=400)
           {
             speed=superspeed;
             superflag=1;
             speed_cnt=400;
  }
             else {
                    count=0;
                    speed=setspeed;
                    speed_cnt=0;
                   }
            
           }
           else if(RoadType==2) {
            speed=setspeed;
            speed_cnt=0;
           }
           else if(RoadType==3){
             speed=hillspeed;                                       
             speed_cnt=0;
          }
          else if(RoadType==1){
             speed=setspeed;
             speed_cnt=0;
          }
          if(Middle_Err>=6)
         {
          if(superflag==1)
           {
              if(flag_cnt<=500)
              {
                Li1=4.5;
                Ri1=4.5;
                flag_cnt++;
                if(flag_cnt>=500) 
                      superflag=0;
              }  
            }
            else 
             {
                Li1=Li;
                Ri1=Ri;
                flag_cnt=0;
             } 
          }
          else 
          {
            Li1=Li;
            Ri1=Ri;
            flag_cnt=0;
          }
        
         if(Middle_Err>0)
          {
           pwmtemp_l=speed;
           pwmtemp_r=speed*(1+7/Middle_Err); 
          }
         else if(Middle_Err<0)
          {
          pwmtemp_l=speed*(1-7/Middle_Err);
          pwmtemp_r=speed;
          }
         else if(Middle_Err=0)
          {
          pwmtemp_l=pwmtemp_r=speed;
          }   
         //pwmtemp_l=speed*(1-Li1*(steer_out_this-STEER_MID)/STEER_MID);//3.5/////80 0.16//2.9   -fabs(Middle_Err)*Middle_Err*0.25;
         //pwmtemp_r=speed*(1+Ri1*(steer_out_this-STEER_MID)/STEER_MID);//8////3.9    +fabs(Middle_Err)*Middle_Err*0.2;
         if(setspeed!=0)
         { 
         error_l=pwmtemp_l-Current_speed;
         error_r=pwmtemp_r-Current_speed1;
         pwmtempl=pwmtempl+PID_P*(error_l-last_error_l)+PID_D*(error_l+pre_error_l-2*last_error_l)+PID_I*error_l;
         pwmtempr=pwmtempr+PID_P*(error_r-last_error_r)+PID_D*(error_r+pre_error_r-2*last_error_r)+PID_I*error_r;
          if(pwmtempl<0) 
              {            
                pwmtempl=0;
              }                                                                
              else if(pwmtempl>200) 
             {
               pwmtempl = 200;
             }
          if(pwmtempr<0) 
              {            
                pwmtempr=0;
              }                                                                
              else if(pwmtempr>200) 
             {
               pwmtempr=200;
             }
          Motor_forward(pwmtempr,pwmtempl);//
          pre_error_l=last_error_l;       
          last_error_l=error_l; 
          pre_error_r=last_error_r;       
          last_error_r=error_r;
         } else  
         {
         if(stopcnt<=20)
         { 
         Motor_back(40);
         stopcnt++;
         }
         else 
         Motor_back(10);
         }
}

}
          /*********************************/
         /*float Li1,Ri1;
         static int sum_cnt=0,super_cnt1=0,super_cnt2=0;
         static int Flag_superspeed=0;
         static int Sum_Err;
         if(RoadType!=3&&RoadType!=2)
         {
             Sum_Err+=fabs(Middle_Err);
             sum_cnt++;
             if(sum_cnt>=10&&(Sum_Err/sum_cnt)<=5)//判定直道 
             {
               Flag_superspeed=1;
               Sum_Err=sum_cnt=0;
             }
             else if(sum_cnt>=10&&(Sum_Err/sum_cnt)>5)//判定为弯道
             {
              speed=setspeed;
              Sum_Err=sum_cnt=0;
             }
             else if(sum_cnt<10)//未到判定条件保持原输出
              speed=setspeed;
            
         } 
         else if(RoadType==2) 
         {
            speed=setspeed;
            //speed_cnt=0;
         }
         else if(RoadType==3)
         {
            speed=hillspeed;
            //speed_cnt=0;
         }
         
         if(Flag_superspeed==1)//直道加速处理 
         {
          super_cnt1++;
          super_cnt2++;
          if(super_cnt2>=800)//进入弯道减速处理
          {
            super_cnt1-=21;
            if(super_cnt1>=0)
            {
              //减速未完成
              speed=0;//setspeed-30;
              //Motor_back(superspeed);
              //Motor_forward(0,0);
              LED2=~LED2;
              
            }
            else//减速完成
            {
              speed=setspeed;
              Flag_superspeed=super_cnt1=super_cnt2=0;
            }
          }
          else//未进入弯道
          {
            speed=superspeed+20;
            Li1=6;
            Ri1=6;
          }
         }
         else //其他情况处理
         {
           Li1=Li;
           Ri1=Ri;
         }*/ 
         /*****************************/ 





