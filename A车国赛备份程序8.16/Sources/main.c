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
long int Stoptime=0;//停车计时

char stopflag=0;
/***********障碍变量*********************/
unsigned int Barraicade_num=0;
unsigned int Barraicade_flag1=1,Barraicade_flag2=0,Barraicade_flag3=0;
char Barraicadeflag=0;
long int  Barraicade_time_1=0, Barraicade_time_2=0,Barraicade_time_3=0;
/*********************************************/
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
    OLED_PrintValueI(64,0, Current_speed);
    OLED_PutString(0, 1, "R_speed:");
    OLED_PrintValueI(64,1, Current_speed1);
    OLED_PutString(0, 2, "Middle_Err:");
    OLED_PrintValueI(64,2, Middle_Err);
    OLED_PutString(0, 3, "RoadType:");
    OLED_PrintValueI(64,3, RoadType);
    //OLED_Fill(0x00);           // 清屏
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
     while(PORTE_PE0==1)
      {
      if(PTH==0xfe)
        {
        stopflag=0;
        OLED_PutString(0, 0, "Car Stop?");
        OLED_PutString(36, 1, "NO ");
        }
       else
        {
        stopflag=1;
        OLED_PutString(0, 0, "Car Stop?");
        OLED_PutString(36, 1, "YES");
        }
      
      }
     while(PORTE_PE2==1) //S1开始按键 
    {
        switch(PTH) 
        {
          case 0xfe:Stoptime=5000;break;
          case 0xfd:Stoptime=7000;break;
          case 0xfb:Stoptime=10000;break;
          case 0xf7:Stoptime=16000;break;
          case 0xef:Stoptime=20000;break;
          case 0xdf:Stoptime=25000;break;
          case 0xbf:Stoptime=30000;break;
          case 0x7f:Stoptime=32767;break;
          default:Stoptime=32767;break;
         }
       
       OLED_PutString(0, 0, "Stoptime:(ms)");   //显示字符
       OLED_PrintValueI(32,1, Stoptime);
      
     }
     while(PORTE_PE1==1)
     {  
       switch(PTH)
       {
          case 0xfe:setspeed=40;Fuzzy_Kp=0.25;Fuzzy_Kd=2;TURNP=2;TURND=25;Li=3.5;Ri=3.5;superspeed=70;break;
          case 0xfd:setspeed=50;Fuzzy_Kp=0.23;Fuzzy_Kd=2;TURNP=4;TURND=25;Li=4;Ri=3.7;superspeed=90;break;
          case 0xfb:setspeed=60;Fuzzy_Kp=0.23;Fuzzy_Kd=2;TURNP=4;TURND=25;Li=4;Ri=3.7;superspeed=95;break;
          case 0xf7:setspeed=70;Fuzzy_Kp=0.23;Fuzzy_Kd=2;TURNP=4;TURND=25;Li=4;Ri=3.7;superspeed=95;break;    
          case 0xef:setspeed=75;Fuzzy_Kp=0.22;Fuzzy_Kd=2;TURNP=2;TURND=25;Li=4;Ri=3.8;superspeed=95;break;  //5
          case 0xdf:setspeed=80;Fuzzy_Kp=0.23;Fuzzy_Kd=2;TURNP=2;TURND=20;Li=4;Ri=4;superspeed=100;break;   //6
          case 0xbf:setspeed=85;Fuzzy_Kp=0.23;Fuzzy_Kd=1;TURNP=2;TURND=25;Li=4.5;Ri=4.5;superspeed=115;break; //7
          case 0x7f:setspeed=90;Fuzzy_Kp=0.23;Fuzzy_Kd=1;TURNP=2;TURND=25;Li=5;Ri=5;superspeed=100;break;  //8
          default:setspeed=50;Fuzzy_Kp=0.23;Fuzzy_Kd=2;TURNP=4;TURND=25;Li=4.2;Ri=3.7;superspeed=90;break; 
        }   
    //OLED_Fill(0x00);           // 清屏
    OLED_PutString(0, 0, "SETSPEED:");   //显示字符                                            
    OLED_PrintValueI(64,0,setspeed);
    OLED_PutString(0, 1, "LI:     ");
    OLED_PrintValueI(64,1, Li);
    OLED_PutString(0, 2, "RI:");
    OLED_PrintValueI(64,2, Ri);
    OLED_PutString(0, 3, "SUPER");
    OLED_PrintValueI(64,3, superspeed);
     }
   /**************障碍时间设置*******************/
    while(PORTE_PE3==1)
    {
      switch(PTH)
        {
        case 0xfe:  Barraicade_num=1;break;
        case 0xfd:  Barraicade_num=2;break;
        case 0xfb:  Barraicade_num=3;break;
        default:    Barraicade_num=1;break;
        }
    OLED_Fill(0x00);           // 清屏 
    OLED_PutString(0, 0, "  Barraicade_num:");   //显示字符
    OLED_PrintValueI(32,1, Barraicade_num);    
    }
    //while(PORTE_PE3!=0);
    while (PORTE_PE0==1)
      {
        switch(PTH)
          {
          case 0xfe:Barraicade_time_1=1000;break;
          case 0xfd:Barraicade_time_1=2000;break;
          case 0xfb:Barraicade_time_1=3000;break;
          case 0xf7:Barraicade_time_1=5000;break;
          case 0xef:Barraicade_time_1=7000;break;
          case 0xdf:Barraicade_time_1=9000;break;
          case 0xbf:Barraicade_time_1=11000;break;
          case 0x7f:Barraicade_time_1=15000;break;
          default:Barraicade_time_1=20000;break;
          }
      
      OLED_PutString(0, 0, "Barraicade 1 time:");   //显示字符
      OLED_PrintValueI(32,1, Barraicade_time_1);
      OLED_Fill(0x00);           // 清屏        
      }
      //while(PORTE_PE3!=0); 
    if(Barraicade_num>1)
    {
     while(PORTE_PE2==1)
      {
      switch(PTH)
        {
          case 0xfe:Barraicade_time_2=3000;break;
          case 0xfd:Barraicade_time_2=5000;break;
          case 0xfb:Barraicade_time_2=7000;break;
          case 0xf7:Barraicade_time_2=9000;break;
          case 0xef:Barraicade_time_2=11000;break;
          case 0xdf:Barraicade_time_2=13000;break;
          case 0xbf:Barraicade_time_2=15000;break;
          case 0x7f:Barraicade_time_2=20000;break;
          default:Barraicade_time_2=10000;break;
        }
     
     OLED_PutString(0, 0, "Barraicade 2 time:");   //显示字符
     OLED_PrintValueI(32,1, Barraicade_time_2);
     OLED_Fill(0x00);           // 清屏     
      }
    }
    if(Barraicade_num==3)
      {
      while(PORTE_PE1==1)
        {
        switch(PTH)
          {
          case 0xfe:Barraicade_time_3=3000;break;
          case 0xfd:Barraicade_time_3=5000;break;
          case 0xfb:Barraicade_time_3=7000;break;
          case 0xf7:Barraicade_time_3=9000;break;
          case 0xef:Barraicade_time_3=11000;break;
          case 0xdf:Barraicade_time_3=13000;break;
          case 0xbf:Barraicade_time_3=15000;break;
          case 0x7f:Barraicade_time_3=17000;break;
          default:Barraicade_time_3=10000;break; 
          }
        //while(PORTE_PE3!=0); 
        OLED_PutString(0, 0, "Barraicade 3 time:");   //显示字符
        OLED_PrintValueI(32,1, Barraicade_time_3); 
        OLED_Fill(0x00);           // 清屏   
        }    
      }
   /***************************************************/
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
    //DDRJ=0xff;
    //PTJ=0xff;
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
		if(PORTE_PE3==0)
		   OLED_Init(); 

    }             
}
/***********************************************************/
/***************************中断部分************************/
#pragma CODE_SEG __NEAR_SEG NON_BANKED
interrupt VectorNumber_Vpit0 void PIT_ISR(void) 
{
    static unsigned int part=0,SendflagCCD30ms=0,Sendflag50ms=0,OLEDflag100ms=0;
    int M_PACNT;
    static long int tim=0; 
    static long int Barraicade_tim=0;
    PITTF_PTF0 = 1;
    PORTK_PK0=0;       
    part++;
    SendflagCCD30ms++; 
    Sendflag50ms++;
    OLEDflag100ms++;
    switch(part)
  {
    case 1: //Motor_forward(60,60);
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
  if(OLEDflag100ms>=80)
   {
    OLEDflag100ms=0;
    OLED_flag=1;
    LED1=~LED1;
   }
  
  PORTK_PK0=1;
  if(tim<65536)
    {
    Barraicade_tim++;
    tim++; 
    }
 /*************停车判断*************/       
  if(tim>=Stoptime&&stopflag==1) 
    {
     //PTJ=0x00;
     
     /*if(fabs(Middle_Err)<=4) 
     {
      t=start_check(Pixel,3,20);
      if(t>=4)
        {
         setspeed=0;
        }
     }*/
     if(PTJ_PTJ0==0||PTJ_PTJ1==0)
          setspeed=0;
    }
    
  /*****************障碍判断*********************/ 
  if((Barraicade_tim==Barraicade_time_1)&&(Barraicade_flag1==1))
    {
    //PTJ=0x00;
    Barraicade_flag1=0;
    Barraicade_flag2=1;
    Barraicadeflag=1;
    Barraicade_tim=0;
    }
  if((Barraicade_tim==Barraicade_time_2)&&(Barraicade_flag2==1)&&(Barraicadeflag==0)&&(Barraicade_num>1))
    {
    //PTJ=0x00;
    Barraicade_flag2=0;
    Barraicade_flag3=1;
    Barraicadeflag=1;
    Barraicade_tim=0;
    }
  if((Barraicade_tim==Barraicade_time_3)&&(Barraicade_flag3==1)&&(Barraicadeflag==0)&&(Barraicade_num==3))
    {
    //PTJ=0x00;
    Barraicade_flag3=0;
    Barraicadeflag=1;
    Barraicade_tim=0;
    }
  if(((RoadType==2)&&(Barraicadeflag==1)&&(Barraicade_tim>2000))||((Barraicade_tim>2000)&&(Barraicadeflag==1)))
    {
    //PTJ=0xff;
    Barraicadeflag=0;
    Barraicade_tim=0;
    }
  /*************************************************************************/
 }

#pragma CODE_SEG DEFAULT

 


