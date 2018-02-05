#include"all.h"
/*****************延时函数**********************/
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
/************冒泡算法顺序是从小到大************/
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
/*****************拷贝函数*********************/
void m_strcpy(unsigned char *des,unsigned char *src,int start,int end)
{
	int i;
	for(i=0;i<end-start;i++)
	{
		des[i]=src[start+i];
	}
}
/*****************处理CCD数据3*****************/
void mid_val_3(unsigned char *A)
{
	int i;
	unsigned char A_temp[128];//创建一个128大小的数组
	unsigned char A_temp2[3];//创建一个3大小的数组
	memcpy(A_temp,A,128);//拷贝函数将A数组拷贝到A_temp中
	for(i=0;i+3<128;i++)//  
	{
		m_strcpy(A_temp2,A_temp,i,i+3);
		bubble_sorts(A_temp2,0,3);//此处是冒泡排序 顺序是由小到大
		A[3/2+i]=A_temp2[1];
	}
}
/*****************处理CCD数据5*****************/
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
/********************滤波**********************/
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
/******************动态阈值*********************/
/******************挑选阈值***********************/
int select_yuzhi() 
{
    unsigned char stat=0;
    stat=PORTE&0x01;
    if(stat==1)//不按下去 
    {
        return std_yuzhi;
    } 
    else if(stat==0)//拨码开关左边第一上去，按键不按
    {
         return std_yuzhi+6;
    }   
}
/******************最小二乘法拟合斜率***********************/
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
  if((end-begin)*x2sum-xsum*xsum) //判断除数是否为零 
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



