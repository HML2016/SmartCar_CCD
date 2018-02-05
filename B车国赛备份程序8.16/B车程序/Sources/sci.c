#include"all.h"
#define BYTE0(Temp)       (*((char *)(&Temp) + 3))    
#define BYTE1(Temp)       (*((char *)(&Temp) + 2))
#define BYTE2(Temp)       (*((char *)(&Temp) + 1))
#define BYTE3(Temp)       (*(char *)(&Temp)) 
extern unsigned int Max_Value;
extern int Left,Right,leftfind,rightfind;
extern int pwmtempl,pwmtempr,pwmtemp_l,pwmtemp_r,Current_speed,steer_control_P,steer_control_D,PID_P,PID_D,set_speed,steer_out_this;
extern float Middle_Err;
extern uint8_t RisingEdgeCnt,FallingEdgeCnt;
unsigned char CCD_Draw_Buff[150];
void SCI0_Init(void)//���ò����ʲ�ʹ��SCI�շ�
{
    SCI1BD =80000000/16/115200;  //���ò�����Ϊ115200 bps  SCIBDΪSCI�����ʼĴ���    SCI������=SCIģ��ʱ��/16/SBR
    SCI1CR1 = 0x00;      	//LOOPS=0,SCISWAI=0,RSRC=0,M=0,               SBR ΪSCI������λ
	                    		//WAKE=0,ILT=0,PE=0,PT=0 	
    SCI1CR2 = 0x0C;      	//TIE=0,TCIE=0,RIE=0,ILIE=0,
	                    		//TE=1,RE=1,RWU=0,SBK=0   TE�Ƿ�����ʹ��λ     RE�ǽ�����ʹ��λ
}                       // TEʹ��SCI������������TxD���ű�SCI����    REʹ��SCI�Ľ�����

void putc(unsigned char dat) 
{

      while(!SCI1SR1_TDRE)     // �ȴ����ͻ�������
      {
            ;
      }                 
       
      SCI1DRL = dat;	//SCI���ݼĴ���	                    
}
void sendbegin() {
  putc(0x55);
  putc(0xAA);
  putc(0xAE);
  }

void chuli() 
{
  unsigned int i,j;
  float ratio=127.0/Max_Value;
  for(i=0;i<128;i++)
    if(Pixel[i]>Max_Value)
        Max_Value=Pixel[i];
    for(i=0; i<128; i++)
  {
    CCD_Draw_Buff[i]=Pixel[i]*ratio;                            
  }
  CCD_Draw_Buff[130]=BYTE0(Max_Value);
  CCD_Draw_Buff[131]=BYTE1(Max_Value); 
  CCD_Draw_Buff[128]=(unsigned char)Left;
  CCD_Draw_Buff[129]=(unsigned char)Right; 
  CCD_Draw_Buff[134]=(unsigned char)leftfind;
  CCD_Draw_Buff[135]=(unsigned char)rightfind;
} 

void Send_CCD1(void) //���͵�һ��CCD������
{
  unsigned int i,num=136;
  chuli();
  sendbegin();
  putc(0x55); //ͨ�����ڷ���һ���ֽڵ����ݣ���ͬ
  putc(0xaa);
  putc(0xaf);
  putc(num); 
  for(i=0;i<num;i++)              //CCD
  {
   putc(CCD_Draw_Buff[i]); 
  }
  putc(0x0f);  //֡β�����뷢��
}


float reserve=0;
float *Variable[20]={&Current_speed,&Middle_Err,&pwmtempl,&pwmtempr,&pwmtemp_l,&pwmtemp_r,&Left,
                     &Right,&steer_out_this,&RisingEdgeCnt,&FallingEdgeCnt,&reserve,&reserve,&reserve,&reserve,&reserve,&reserve,&reserve,&reserve};

void Send_Variable()     //����ʵʱ����
{                                                                   
  unsigned int i=0,ch=0;
  float temp=0;
  unsigned int Variable_num=16;
  sendbegin();
  putc(0x55);
  putc(0xaa);
  putc(0xad);
  putc(Variable_num);/////////////
 for(i=0;i<Variable_num;i++)
  {
    temp=*Variable[i];
    ch=BYTE0(temp);
     putc(ch);
    ch=BYTE1(temp);
     putc(ch);
    ch=BYTE2(temp);
     putc(ch);
    ch=BYTE3(temp);
     putc(ch);
  }
    putc(0x0d);
} 
float *Parameter[16]={&steer_control_P,&steer_control_D,&PID_P,&PID_D,&reserve,&reserve,&reserve,&reserve,
                         &reserve,&reserve,&reserve,&reserve,&reserve,&reserve,&reserve};
//���Ϳ��Ʋ���
void Send_Para()
{
  unsigned int i=0,ch=0;
  float temp=0;
  unsigned int Parameter_num=14;
  sendbegin();
  putc(0x55);
  putc(0xaa);
  putc(0xab);
  putc(Parameter_num);
  for(i=0;i<Parameter_num;i++)
  { 
     temp=*Parameter[i];
    ch=BYTE0(temp);
    putc(ch);
    ch=BYTE1(temp);
    putc(ch);
    ch=BYTE2(temp);
    putc(ch);
    ch=BYTE3(temp);
    putc(ch);
  }
    putc(0X0b);//֡β
}
/*
void Send_CCD2(void) //���͵ڶ���CCD������
{
  unsigned int i,num=136;
  chuli();
  putc(0x55); //ͨ�����ڷ���һ���ֽڵ����ݣ���ͬ
  putc(0xaa);
  putc(0xba);
  putc(num); 
  for(i=0;i<num;i++)              //CCD
  {
   putc(CCD_Draw_Buff2[i]); //CCD_Draw_Buff2 ����洢��������һ��CCD����Ϣ
  }
  putc(0x0a);  //֡β�����뷢��
}
*/


















 