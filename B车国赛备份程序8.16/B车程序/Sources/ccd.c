#include "all.h"
#define CCD_Offset 60//40          //����ѹ
uint16_t Max_Value;
extern uint8_t Pixel[128];
extern uint8_t Pixel1[128];
extern uint8_t CCD_Draw_Buff[150];
void ImageCapture() {
    uint8_t i;
    uint16_t temp_int;
    TSL1401_SI = 1;         /* SI  = 1 PORTA��һͨ����*/
    SamplingDelay();//CPU��ʱ200ns
    TSL1401_CLK = 1;        /* CLK = 1 PTA0��IOֵΪ1*/
    SamplingDelay();//CPU��ʱ200ns
    TSL1401_SI = 0;         /* SI  = 0 */
    SamplingDelay();//CPU��ʱ200ns
    //Delay 20us for sample the first pixel
    for(i = 0; i < 20; i++) {
        Cpu_Delay1us();
    }
    //Sampling Pixel 1
    temp_int = AD_Measure12(0);
    Pixel[0] = (byte)(temp_int>>4);//typedef unsigned char byte��temp_int>>4��ʾ��ʧ���ȣ�����4λ��Ҫ�� ��byte����ǿ������ת�� *imageData = �ǰ��ұߵ�ֵ��ֵ����ߵ�ָ����ָ��ı��� ++��ʾ��ֵ��ɺ�imageData����1
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
    }//��ģ���ź�ת��Ϊ�ɼ����鲿��
    SamplingDelay();
    SamplingDelay();
    TSL1401_CLK = 1;        /* CLK = 1 */
    SamplingDelay();
    SamplingDelay();
    TSL1401_CLK = 0;        /* CLK = 0 */   
}
uint16_t AD_Measure12(uint8_t Channel) 
{
    ATD0CTL5_Cx = Channel;//ATD���ƼĴ���5����ת�����е����ͺ�ģ������Ĳ���ͨ��
    while(ATD0STAT0_SCF == 0);//ATD״̬�Ĵ���0 ������ת����ɱ�־λ��ת�����м�����SCF-ת�����н�����־
    return ATD0DR0;
}

void CCD_Normalization()  // ��һ������
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
   






















