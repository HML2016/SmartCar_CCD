#include"all.h"
/*****************��ʱ����**********************/
void delay(void)
{
   unsigned int i,j;
   for(i=0;i<65500;i++)
   {
     for(j=0;j<400;j++)
     {
     }
   }
 }
/************ð���㷨˳���Ǵ�С����************/
void bubble_sorts(unsigned char  a[],int start,int end)
{
	int i,j;
	unsigned char temp;
	for(j=start;j<end-1;j++)
	for(i=start;i<end-1-j;i++)
	if(a[i]>a[i+1])
	{
		temp=a[i];
		a[i]=a[i+1];
		a[i+1]=temp;
	}
}
/*****************��������*********************/
void m_strcpy(unsigned char *des,unsigned char *src,int start,int end)
{
	int i;
	for(i=0;i<end-start;i++)
	{
		des[i]=src[start+i];
	}
}
/*****************����CCD����3*****************/
void mid_val_3(unsigned char *A)
{
	int i;
	unsigned char A_temp[128];//����һ��128��С������
	unsigned char A_temp2[3];//����һ��3��С������
	memcpy(A_temp,A,128);//����������A���鿽����A_temp��
	for(i=0;i+3<128;i++)//  
	{
		m_strcpy(A_temp2,A_temp,i,i+3);
		bubble_sorts(A_temp2,0,3);//�˴���ð������ ˳������С����
		A[3/2+i]=A_temp2[1];
	}
}
/*****************����CCD����5*****************/
void mid_val_5(unsigned char *A) 
{
	int i;
	unsigned char A_temp[128];
	unsigned char A_temp2[5];
	memcpy(A_temp,A,128);
	for(i=0;i+5<128;i++)
	{
		m_strcpy(A_temp2,A_temp,i,i+5);
		bubble_sorts(A_temp2,0,5);
		A[5/2+i]=A_temp2[1];
	}
}
/********************�˲�**********************/
void juzhi_lvbo(unsigned char *A)
{
  	int i,j;
  	int ave=0;
  	unsigned char A_temp[128];
  	unsigned char A_temp2[4];
  	memcpy(A_temp,A,128);
  	for(i=0;i+4<128;i++)
  	{
  		m_strcpy(A_temp2,A_temp,i,i+4);
  		for(j=0;j<4;j++)
  		{
  			ave+=A_temp2[j];
  		}	
  		A[4/2+i]=ave/4;
  		ave=0;
  	}
}	
/******************��̬��ֵ*********************/
/******************��ѡ��ֵ***********************/
int select_yuzhi() 
{
    unsigned char stat=0;
    stat=PORTE&0x01;
    if(stat==1)//������ȥ 
    {
        return std_yuzhi;
    } 
    else if(stat==0)//���뿪����ߵ�һ��ȥ����������
    {
         return std_yuzhi+6;
    }   
}
/******************��С���˷����б��***********************/
float Slope_Calculate(uint8_t begin,uint8_t end,float *p)  
{
  float xsum=0,ysum=0,xysum=0,x2sum=0;
   uint8_t i=0;
   float result=0;
   static float resultlast;
   p=p+begin;
   for(i=begin;i<end;i++)
   {
	   xsum+=i;
	   ysum+=*p;
	   xysum+=i*(*p);
	   x2sum+=i*i;
	   p=p+1;
   }
  if((end-begin)*x2sum-xsum*xsum) //�жϳ����Ƿ�Ϊ�� 
  {
    result=((end-begin)*xysum-xsum*ysum)/((end-begin)*x2sum-xsum*xsum);
    resultlast=result;
  }
  else
  {
   result=resultlast;
  }
  return result;
}



