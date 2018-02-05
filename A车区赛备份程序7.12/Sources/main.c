#include"all.h"
uint8_t Pixel[128];
uint8_t Pixel1[128];
uint8_t TimerFlag20ms=0;
uint8_t RisingEdgeCnt,FallingEdgeCnt;
uint8_t RoadType;
int Left,Right;
int t;
int Current_speed=0;
int Current_speed1=0;
int Right_Or_Left;
float speed;
float setspeed;
float  Middle_Err; 
float steer_out_this=3030;
float pwmtemp_l=0,pwmtemp_r=0; 
float pwmtempl=0,pwmtempr=0;
float superspeed;
float hillspeed;
extern int leftfind,rightfind;
extern float steer_control_P;
extern float Fuzzy_Kp;//0.15
extern float Fuzzy_Kd;
extern float TURNP;
extern float TURND;
extern float Ri;
extern float Li;
extern uint8_t Leftlastfind,Rightlastfind;
char TimerCnt20ms=0;
char OLED_flag=0;
char SendCCD_flag=0;
char SendPara_flag=0;
int Stoptime=0;//停车计时
int stopflag=0;
/************************************************************/
int Speed_Flag=0;
/*                在液晶上显示信息                          */
/************************************************************/
void play_data(void) 
{   
    unsigned int L_speed,R_speed,S;
    L_speed=pwmtemp_l;
    R_speed=pwmtemp_r;
    S=Middle_Err;
    OLED_PutString(0, 0, "L_speed:");   //显示字符
    OLED_PrintValueI(64,0, L_speed);
    OLED_PutString(0, 1, "R_speed:");
    OLED_PrintValueI(64,1, R_speed);
    OLED_PutString(0, 2, "Middle_Err:");
    OLED_PrintValueI(64,2, Middle_Err);
    OLED_PutString(0, 3, "Speed:");
    OLED_PrintValueI(64,3, speed);
}

/**************中断优先级******************/  
void Interrupt_Priority_Set(void){
    INT_CFADDR=0x70;
    INT_CFDATA5=0x05;
    INT_CFADDR=0xC0;
    INT_CFDATA6=0x06;
    INT_CFADDR=0xD0;
    INT_CFDATA3=0x07;
}
/******************************************/
void main(void) 
{ 
     DDRK_DDRK0=1; 
     DDRB=0X00;
     DDRE=0XF0; 
     DDRH=0X00;// 拨码开关  DDRH为H口数据方向寄存器
     OLED_Init();
     while(PORTE_PE0==1) //S1开始按键 
    {
        switch(PTH) {
          case 0xfe:Stoptime=5000;break;
          case 0xfd:Stoptime=19000;break;
          case 0xfb:Stoptime=19000;break;
          case 0xf7:Stoptime=19000;break;
          case 0xef:Stoptime=9000;break;
          case 0xdf:Stoptime=10000;break;
          case 0xbf:Stoptime=11000;break;
          case 0x7f:Stoptime=20000;break;
          default:Stoptime=8000;break;
         }
       
       OLED_PutString(32, 0, "Stoptime:");   //显示字符
       OLED_PrintValueI(32,1, Stoptime); 
       }
      while(PORTE_PE2==1){  
        switch(PTH){
          case 0xfe:setspeed=60;Fuzzy_Kp=0.25;Fuzzy_Kd=2;TURNP=2;TURND=25;Li=3.5;Ri=3.5;superspeed=70;break;
          case 0xfd:setspeed=75;Fuzzy_Kp=0.23;Fuzzy_Kd=2;TURNP=4;TURND=25;Li=4;Ri=3.7;superspeed=90;break;
          case 0xfb:setspeed=75;Fuzzy_Kp=0.23;Fuzzy_Kd=2;TURNP=4;TURND=25;Li=4;Ri=3.7;superspeed=95;break;
          case 0xf7:setspeed=80;Fuzzy_Kp=0.23;Fuzzy_Kd=2;TURNP=4;TURND=25;Li=4.2;Ri=3.7;superspeed=95;break;    
          case 0xef:setspeed=100;Fuzzy_Kp=0.22;Fuzzy_Kd=2;TURNP=2;TURND=25;Li=4;Ri=3.8;superspeed=95;break;  //5
          case 0xdf:setspeed=110;Fuzzy_Kp=0.23;Fuzzy_Kd=2;TURNP=2;TURND=20;Li=4;Ri=4;superspeed=100;break;   //6
          case 0xbf:setspeed=120;Fuzzy_Kp=0.23;Fuzzy_Kd=1;TURNP=2;TURND=25;Li=4.5;Ri=4.5;superspeed=115;break; //7
          case 0x7f:setspeed=130;Fuzzy_Kp=0.23;Fuzzy_Kd=1;TURNP=2;TURND=25;Li=5;Ri=5;superspeed=100;break;  //8
          default:setspeed=75;Fuzzy_Kp=0.25;Fuzzy_Kd=2;TURNP=2;TURND=25;Li=4;Ri=4;superspeed=90;break; 
        }   
    OLED_PutString(0, 0, "SETSPEED:");   //显示字符                                            
    OLED_PrintValueI(64,0,setspeed);
    OLED_PutString(0, 1, "LI:");
    OLED_PrintValueI(64,1, Li);
    OLED_PutString(0, 2, "RI:");
    OLED_PrintValueI(64,2, Ri);
    OLED_PutString(0, 3, "SUPER");
    OLED_PrintValueI(64,3, superspeed);
     }
    DisableInterrupts;
    SetBusCLK_80M();   //设置总线时钟频率是40M
    SCI0_Init();  //设置波特率并使能SCI收发   
    PIT_Init();  //设置PIT定时器使能，用来触发单片机的外围设备，也可以用来产生周期中断
    AD_Init();//AD初始化：确定了数模转换的精度，设定了采样时间为4个时钟周期 设定了预分频的频率
    CCD_IO_Init();//设定通道A0和1口为CCD信号的输出
    PWM_Init();
    PAC_Init(); //脉冲累加初始化
    LED_Init();
    DDRJ=0xFC;//光电对管管脚J0，J1设置数据方向为输入
    hillspeed=setspeed-20;
    delay();
   steering(STEER_MID);
   EnableInterrupts;     
  while(1)
    {
     if(OLED_flag)
		{ 
			play_data();
			OLED_flag=0;
			Send_CCD1();   
		}
    }             
}
/***********************************************************/
/***************************中断部分************************/
#pragma CODE_SEG __NEAR_SEG NON_BANKED
interrupt VectorNumber_Vpit0 void PIT_ISR(void) 
{
    static unsigned int part=0,SendflagCCD30ms=0,Sendflag50ms=0,OLEDflag100ms=0;
    int M_PACNT;
    static int tim=0; 
    PITTF_PTF0 = 1;
    PORTK_PK0=0;       
    part++;
    SendflagCCD30ms++; 
    Sendflag50ms++;
    OLEDflag100ms++;
    switch(part)
  {
    case 1:
            car_speed();
            ImageCapture();//采集128个像素点 0.5ms
          
           	break;
	       
    case 2:
            CCD_Normalization();//归一化 2ms                       
            break;
    case 3: 
            search();  //寻线 1.5ms
            steer_pd(); //舵机控制 30us
            M_PACNT=PACNT;
            PACNT=0;
            Current_speed=(int)(M_PACNT)+20;
            Current_speed1=(int)PORTB*2+30;
            part = 0;   //循环计数归零
            break;         
    default:
           break;
  }           
  if(OLEDflag100ms>=80) {
    OLEDflag100ms=0;
    OLED_flag=1;
    LED1=~LED1;
    }                                                     
    PORTK_PK0=1;
    if(tim<=20002)
    tim++;  
    if(tim>=Stoptime) {
            stopflag=1;
             if(fabs(Middle_Err)<=4) {
              t=start_check(Pixel,3,20);
               if(t>=5)
               {
                setspeed=0;
                
               }
             }
            }    
 }
/*#pragma CODE_SEG __NEAR_SEG NON_BANKED
interrupt VectorNumber_Vpit0 void PIT_ISR(void) 
{
    static unsigned int part=0,OLEDflag100ms=0,c_cnt=0;
    int M_PACNT;
    static int tim=0,Current_speed_l,Current_speed1_r; 
    PITTF_PTF0 = 1;         
    part++;
    OLEDflag100ms++;
    switch(part)
  {
    case 1: car_speed();
            PORTK_PK0=0;
            PACTL=0x40;break;
    case 2: 
            ImageCapture();//采集128个像素点 0.5ms
           	break;
    case 3: M_PACNT=PACNT;
            PACNT=0; 
            PACTL = 0x00;//关闭累加器
            Current_speed_l=Current_speed_l+(int)(M_PACNT);
            Current_speed1_r=Current_speed1_r+(int)PORTB;
            c_cnt++;
            PORTK_PK0=1; 
            CCD_Normalization();//归一化 2ms                       
            break;
    case 4: 
            search();  //寻线 1.5ms
            steer_pd(); //舵机控制 30us
            part =0;   //循环计数归零
            break;         
    default:
           break;
  }           
  if(OLEDflag100ms>=80) {
    OLEDflag100ms=0;
    OLED_flag=1;
    LED1=~LED1;
    }                                                     
    if(tim<=12002)
    tim++; 
    if(c_cnt>=1)
    {
     Current_speed=(Current_speed_l/2)*3.9+4.7;
     Current_speed1=(Current_speed1_r/2)*5.1-12.6;
     Current_speed_l=0;
     Current_speed1_r=0;
     c_cnt=0;
    } 
     if(tim>=Stoptime) {//0
               if(fabs(Middle_Err)<=5) {
                t=start_check(Pixel,3,20);
                 if(t>=5)
                setspeed=0; 
               }
              }        
    if(PTJ_PTJ0==1&&PTJ_PTJ1==1&&tim>=300) 
             {
            //delayms(0.25);
             if(PTJ_PTJ0==1&&PTJ_PTJ1==1)     
               setspeed=0;
              }   
              
 }     */           
/*************************************************************/
/*                   输入捕捉中断函数                        */
/*************************************************************/
/* VectorNumber_Vsci1 void SCI1_ISR(void)
{
  char sci_data;static i=0;
  char data_tem;
  int j=0;
  EnableInterrupts;
			Send_Variable();
			Send_CCD1();
			SendCCD_flag=0;
			Send_Para();
			SendPara_flag=0;
			LED2=~LED2;*/
  /*if(SCI0SR1_RDRF) 
  {
      sci_data = SCI0DRL;			
      if(sci_data=='S') 
      {
            Motor_brake();
      } 
      else  if(sci_data=='G')
      {
           Motor_forward(15,15);
      }
      else if(sci_data!='\n')
      {
          str_rec[i]=sci_data;
          i++;
      } 
      else
      {
          i=0; 
          if(str_rec[i]=='s'||str_rec[i]=='g') 
          {
            data_tem=str_rec[i];
            for(j=1;j<strlen(str_rec);j++) 
            {
                str_rec[j-1]=str_rec[j];
            }
            j=0;                     
            if(data_tem=='s') 
            {
                steer_control_P=atof(str_rec);
                Sci0_send_strings("the  steer_control_P is");
                send_pc(steer_control_P); 
            }
            else if(data_tem=='g')
            {
  //              speed=atof(str_rec);
                Motor_forward(speed,speed);
                Sci0_send_strings("the speed is"); 
                send_pc(speed); 
            }              
          }
       } 
  } 
} 
          case 0xfe:setspeed=65;Fuzzy_Kp=0.20;Fuzzy_Kd=0.35;TURNP=2;TURND=25;Li=0;Ri=0;break;
          case 0xfd:setspeed=70;Fuzzy_Kp=0.25;Fuzzy_Kd=0.35;TURNP=2;TURND=25;Li=3.7;Ri=3.65;break;
          case 0xfb:setspeed=75;Fuzzy_Kp=0.25;Fuzzy_Kd=1;TURNP=2;TURND=25;Li=3.7;Ri=3.65;break;
          case 0xf7:setspeed=85;Fuzzy_Kp=0.23;Fuzzy_Kd=1;TURNP=2;TURND=25;Li=3.6;Ri=3.5;break;    
          case 0xef:setspeed=90;Fuzzy_Kp=0.23;Fuzzy_Kd=1;TURNP=2;TURND=25;Li=5;Ri=5;break;  //5
          case 0xdf:setspeed=110;Fuzzy_Kp=0.23;Fuzzy_Kd=2;TURNP=2;TURND=25;Li=4;Ri=4;break;   //6
          case 0xbf:setspeed=100;Fuzzy_Kp=0.25;Fuzzy_Kd=0.35;TURNP=1;TURND=25;Li=3.6;Ri=3.6;break; //7
          case 0x7f:setspeed=105;Fuzzy_Kp=0.3;Fuzzy_Kd=0.35;TURNP=3;TURND=25;Li=3.1;Ri=3.2;break;  //8
          default:setspeed=60;Fuzzy_Kp=0.25;Fuzzy_Kd=0.35;TURNP=2;TURND=25;Li=3.7;Ri=3.65;break;*/
#pragma CODE_SEG DEFAULT

 


