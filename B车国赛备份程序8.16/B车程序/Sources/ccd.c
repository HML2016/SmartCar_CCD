#include "all.h"
#define CCD_Offset 60//40          //暗电压
uint16_t Max_Value;
extern uint8_t Pixel[128];
extern uint8_t Pixel1[128];
extern uint8_t CCD_Draw_Buff[150];
void ImageCapture() {
    uint8_t i;
    uint16_t temp_int;
    TSL1401_SI = 1;         /* SI  = 1 PORTA口一通道打开*/
    SamplingDelay();//CPU延时200ns
    TSL1401_CLK = 1;        /* CLK = 1 PTA0的IO值为1*/
    SamplingDelay();//CPU延时200ns
    TSL1401_SI = 0;         /* SI  = 0 */
    SamplingDelay();//CPU延时200ns
    //Delay 20us for sample the first pixel
    for(i = 0; i < 20; i++) {
        Cpu_Delay1us();
    }
    //Sampling Pixel 1
    temp_int = AD_Measure12(0);
    Pixel[0] = (byte)(temp_int>>4);//typedef unsigned char byte；temp_int>>4表示丢失精度，即低4位不要了 （byte）是强制类型转换 *imageData = 是把右边的值赋值给左边的指针所指向的变量 ++表示赋值完成后imageData自增1
  //  Pixel1[0]= Pixel[0];  
    TSL1401_CLK = 0;        /* CLK = 0 */
    Max_Value=Pixel[0];
    for(i=1; i<128; i++) {
        SamplingDelay();
        SamplingDelay();
        TSL1401_CLK = 1;    /* CLK = 1 */
        SamplingDelay();
        SamplingDelay();
        //Sampling Pixel 2~128
        temp_int= AD_Measure12(0);
        Pixel[i]=(byte)(temp_int>>4);
     //   Pixel1[i]=Pixel[i];
        if(Max_Value< Pixel[i]) 
         Max_Value= Pixel[i];
        TSL1401_CLK = 0;    /* CLK = 0 */
    }//将模拟信号转换为采集数组部分
    SamplingDelay();
    SamplingDelay();
    TSL1401_CLK = 1;        /* CLK = 1 */
    SamplingDelay();
    SamplingDelay();
    TSL1401_CLK = 0;        /* CLK = 0 */   
}
uint16_t AD_Measure12(uint8_t Channel) 
{
    ATD0CTL5_Cx = Channel;//ATD控制寄存器5决定转换序列的类型和模拟输入的采样通道
    while(ATD0STAT0_SCF == 0);//ATD状态寄存器0 包含了转换完成标志位和转换序列计数器SCF-转换序列结束标志
    return ATD0DR0;
}

void CCD_Normalization()  // 归一化处理
{
  uint8_t i=0;
  float ratio;
  if(Max_Value>CCD_Offset)
  ratio=127.0/(Max_Value-CCD_Offset);
  else  ratio=0;
  for(i=0; i<128; i++)
  {
    if(Pixel[i]>CCD_Offset)
    {
    Pixel[i]=(int)((Pixel[i]-CCD_Offset)*ratio);
    }
    else Pixel[i]=0;
    CCD_Draw_Buff[i]=Pixel[i];                      
  }
}
   






















