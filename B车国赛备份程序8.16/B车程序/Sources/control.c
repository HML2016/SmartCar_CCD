#include "all.h"
#define left 0
#define right 1
extern int  Right_Or_Left; 
extern int Left,Right;
extern uint8_t Pixel[128];
extern uint8_t RisingEdgeCnt,FallingEdgeCnt;
extern uint8_t RoadType;
extern float  Middle_Err;
/*************************************/
int Max_Peak; 
int leftfind,rightfind;    //左右边沿都找到了，但是不能直接赋值的那种 
int Turn_State;
int Turn_State_Cnt=0;
int CCD_Diff[128],RisingEdgeIndex,FallingEdgeIndex;
int Both_Line_find_cnt=0;
int Reference_Width=46; 
uint8_t RisingEdge[5],FallingEdge[5];
uint8_t Leftlast,Rightlast,Leftlastfind=0,Rightlastfind=0;
uint8_t curve_to_cross=0;
uint8_t RoadTypeConfirm; 
uint8_t Threshold; //阈值
float  Previous_Width[15];
float  Previous_Error[15];
extern int stopflag;         
/*********起跑线检测*************/
int start_check(unsigned char *A,int interval,int yuzhi)//interval  间隔  间距
{
	int i,tiaobian=0;
	int last_tiaobian=0;
	int this_tiaobian=0;

	for(i=45;i<100;i++)
	{
		if(A[i]-A[i+interval]>yuzhi||A[i]-A[i+interval]<-yuzhi)
		{
		    this_tiaobian=i;
		    if(this_tiaobian!=last_tiaobian+1) 
		    {
		          tiaobian++;     
		    }       
		    last_tiaobian=this_tiaobian;
		}   
	}
	 return tiaobian;
}
/*************************************/
/*************************************/
void search()
 {
  uint8_t i=0,L,R;
  int left_c,right_c;
  static float middle_last;
  Max_Peak=0;
  RisingEdgeCnt=0;//上升沿计数
  FallingEdgeCnt=0;//下降沿计数
   for(i=0;i<5;i++)//重置清零
  {
    RisingEdge[i]=0;
    FallingEdge[i]=0;  
  }
   for(i=3;i<128;i++) //求出最大的差分值
  {
    CCD_Diff[i]= Pixel[i]- Pixel[i-3];
    if(fabs(CCD_Diff[i])>Max_Peak) 
    Max_Peak=fabs(CCD_Diff[i]); 
  }
   for(i=4;i<127;i++)
  {
    if((CCD_Diff[i]>=CCD_Diff[i-1])&&(CCD_Diff[i]>CCD_Diff[i+1])&&(CCD_Diff[i]>got_yuzhi)) //寻找正的峰值，左边线
    {
      if(RisingEdgeCnt<5)    //一行图像最多有5个上跳沿
      { 
       RisingEdge[RisingEdgeCnt]=i;
       RisingEdgeCnt++;  
      }
    } 
    if((CCD_Diff[i]<CCD_Diff[i-1])&&(CCD_Diff[i]<=CCD_Diff[i+1])&&(CCD_Diff[i]<-got_yuzhi))  //寻找负的峰值，右边线
    {
      if(FallingEdgeCnt<5)    //一行图像最多有5个下跳沿
      {
       FallingEdge[FallingEdgeCnt]=i;
       FallingEdgeCnt++;
      }
    }
  }
  
 Prejudge();//预判函数 
  switch(RoadType)
  {
   case 0:
     Normal();
     if(stopflag==1)
     Is_Barraicade();
   break;
  case 1:
    CrossRoad();
   break;
  case 2:
    Barraicade();
   break;
  case 3:
     Hill();
   break;
   default:
    Normal(); 
   break;
  } 
   Leftlast=Left;              // 更新边界线
   Rightlast=Right;            //更新边界线
   left_c=Left-1;              //减去2修正差分误差
   right_c=Right;
   Is_Hill();
  if(Rightlastfind||Leftlastfind)
   {
     Middle_Err=64-(right_c+left_c)/2.0+1;  //除以整数2会损失精度
   //  Middle_Err=Middle_Err_Filter(Middle_Err);
     if(fabs(Middle_Err-middle_last)>2&&(curve_to_cross==0))
     {
       if(RoadType!=2)
       {
         if(fabs(Middle_Err-middle_last)>3)
         {
          if(Middle_Err>middle_last)
           Middle_Err=middle_last+1;
          else Middle_Err=middle_last-1;
         }
         else
         {
          if(Middle_Err>middle_last) 
          Middle_Err=middle_last+2;
          else Middle_Err=middle_last-2;
         }
       }
     }
     middle_last=Middle_Err;
   }
   Push_And_Pull(Previous_Width,15,(float)(Right-Left));
   Push_And_Pull(Previous_Error,15,Middle_Err);
  // Is_p();
  if(Rightlastfind&&Leftlastfind)
   {
     if(Both_Line_find_cnt<20)
     Both_Line_find_cnt++;
   }
   else Both_Line_find_cnt=0;
}
/*************************************/
/*************************************/

void  Prejudge(void)
{
   if(RisingEdgeCnt==0&&FallingEdgeCnt==0)//说明没有跳边沿，全白
   {
      RoadType=1;
      curve_to_cross=0;
   }
   else RoadType=0;
}
/**********************************************/
/**********************************************/
void  Normal(void) 
{
  uint8_t tempwidth;
  if(Leftlastfind==0&&Rightlastfind==0)    //上次没有边线
  {
    Find_Bothine();
    if(Leftlastfind==0&&Rightlastfind==0)
    {
       if(FindBoundary(left))
       {
        Left=RisingEdge[RisingEdgeIndex];
        Leftlastfind=1; 
       }
       else  Leftlastfind=0;
       if(FindBoundary(right))
       {
        Right=FallingEdge[FallingEdgeIndex];
        Rightlastfind=1;
       }
       else  Rightlastfind=0; 
       if(Leftlastfind&&Rightlastfind)
       {
        if(Right<Left)
        {
         Leftlastfind=0;
         Rightlastfind=0;
        }
       }
    }
  } 
  else 
  {  
    //////////////////////////////
   if(Leftlastfind)						//上次找到了左线
   {
    if(FindNearest(left,Leftlast))		//这次能找到左线
    {
      Left=RisingEdge[RisingEdgeIndex];
      Leftlastfind=1;			
      if(Rightlastfind==0)				//上次找到左线，这次找到左线，上次没找到右线
      {
       if(FallingEdgeCnt)					//上次找到左线，这次找到左线，上次没找到右线，这次找到右线
       {
         for(FallingEdgeIndex=0;FallingEdgeIndex<FallingEdgeCnt;FallingEdgeIndex++)
         { 
            rightfind=0;   //
            if(FallingEdge[FallingEdgeIndex]>Left)         //找到潜在的边线了
           {  
             tempwidth = FallingEdge[FallingEdgeIndex]-Left;
             if(fabs(tempwidth-Reference_Width)>=2)       //右线出现的太左了，慢慢靠近
             {
               if(tempwidth>Reference_Width)  
                Reference_Width=Reference_Width+1;   //潜在的赛道边线比参考宽度宽太多
               else                         
               {
                 Reference_Width=Reference_Width-1;   //潜在的赛道边线比参考宽度窄太多
               }
               Right=Left+Reference_Width;
               Rightlastfind = 0;
             }
             else 
             {
              Right=FallingEdge[FallingEdgeIndex];
              Rightlastfind=1;
             }
             rightfind=1;   //找到边线了，但是还不能立马赋值
             break;
           }
         } 
           if(Rightlastfind==0&&rightfind==0)   //虽然说有上升沿，但是该上升沿无效
          {

           Right=Left+Reference_Width;
          }
       }
       else                        //没有下跳沿                                  
       {

              Right=Left+Reference_Width;
       }
      }
    }
    else  //上次能找到左线，这次没能找到左线
    {
       Leftlastfind=0;   //丢线 
        /*           跳转到十字           */   
      if(FindBoundary(left))
     {
      if(RisingEdge[RisingEdgeIndex]<Leftlast)  //这次没能够找到与上次相邻的边界，但是有边界却比现有的边界宽
      {
        if(Rightlastfind==0&&((RoadType!=2)&&(RoadType!=3)))   //上次没有找到右边界，这次左边界又往外靠，那么就是遇到十字了
        {
         RoadType=1; 
        }
      }
      else if(RisingEdge[RisingEdgeIndex]<Leftlast+8)   //这次的边界比上次更靠近中央，但是他们的差值不大于8
      {
         Left=RisingEdge[RisingEdgeIndex];
         Leftlastfind=1;                 //未丢线		 
      }
     } 
      else                     //不能找到左边界
     {
       if(Rightlastfind==0&&(RoadType!=4))  
       {
        RoadType=1;
       }
     }
    }
   }
   ///////////////////////////////
   if(Rightlastfind)					//上次找到了右线
   {
    if(FindNearest(right,Rightlast))	//这次能找到右线
    {
      Right=FallingEdge[FallingEdgeIndex];
      Rightlastfind=1;
      if(Leftlastfind==0)				//上次找到了右线,这次能找到右线,上次找不到左线
      {
       if(RisingEdgeCnt>0)				//上次找到了右线,这次能找到右线,上次找不到左线,这次找到了左线
       {                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                
	      for(RisingEdgeIndex=RisingEdgeCnt-1;RisingEdgeIndex>=0;RisingEdgeIndex--)
         {
           leftfind=0;
           if(RisingEdge[RisingEdgeIndex]<Right)         //找到潜在的边线了
           {
             tempwidth = Right-RisingEdge[RisingEdgeIndex];
             if (fabs(tempwidth-Reference_Width)>=2)       //右线出现的太左了，慢慢靠近
             {
              if(tempwidth>Reference_Width) 
                Reference_Width=Reference_Width+1;   //潜在的赛道边线比参考宽度宽太多
              else                          
              {
                Reference_Width=Reference_Width-1;   //潜在的赛道边线比参考宽度窄太多
              }
              Left=Right-Reference_Width;
              Leftlastfind = 0;
             }
             else 
             {
               Left=RisingEdge[RisingEdgeIndex];
               Leftlastfind=1;
             }
            leftfind=1;
            break;
           }          
         }
            if(Leftlastfind==0&&leftfind==0)   //虽然说有上升沿，但是该上升沿无效
            {

               Left=Right-Reference_Width;
            }
       }
       else                               //没有上升沿哦亲
       { 

         Left=Right-Reference_Width;
       }
      }
    }
    else  //上次能找到，这次没能找到与上次相邻近的线
    {
      Rightlastfind=0;   //丢线
     /*           跳转到十字           */
     if(FindBoundary(right))   //能够找到右边界
     {
      if(FallingEdge[FallingEdgeIndex]>Rightlast)  //这次没能够找到与上次相邻的边界，但是有边界却比现有的边界宽
      {
        if(Leftlastfind==0&&((RoadType!=2)&&(RoadType!=3)))   //上次没有找到左边界，这次右边界又往外靠，那么就是遇到十字了
        {
         RoadType=1; 
        }
      }
      else if(FallingEdge[FallingEdgeIndex]>Rightlast-8)  //这次的边界比上次更靠近中央，但是他们的差值不大于8
      {
          Right=FallingEdge[FallingEdgeIndex];
          Rightlastfind=1;           //其实是未丢线
      }
     }
     else                     //不能找到右边界
     {
        if(Leftlastfind==0&&(RoadType!=4))   //上次没有找到左边界，这次右边界又往外靠，那么就是遇到十字了
       {
         RoadType=1;
       }
     }
    }
   }
   //都能找到边线，更新参考宽度
   if(Rightlastfind&&Leftlastfind)
   {
     Reference_Width=Right-Left; 
     if(Reference_Width<35)
     Reference_Width=35;
   }
  }
}
void CrossRoad(void)
{
   static uint8_t Normal_Cnt;
   static uint8_t Cross_Road_Cnt=0;
  
    //缓慢归零，车子回正  
  if(curve_to_cross==0)
  {  
    Rightlastfind=0;
    Leftlastfind=0; 
  }
  else
  {  
    if(Rightlastfind)
    {
        if(FindNearest(right,Rightlast))		//这次能找到左线
       {
        Right=FallingEdge[FallingEdgeIndex];
        Rightlastfind=1;
        Left=Right-Reference_Width;
       }
       else 
       {
         Rightlastfind=0;
         //curve_to_cross=0;
       }
    }
    else if(Leftlastfind)
    {
      if(FindNearest(left,Leftlast))		//这次能找到左线
     {
      Left=RisingEdge[RisingEdgeIndex];    
      Leftlastfind=1;
      Right=Reference_Width+Left;
     }
     else 
     {
       Leftlastfind=0;
      // curve_to_cross=0;
     }
    }
  }
  
    if(fabs(Middle_Err)>1)
    {
      if(curve_to_cross==0)
      {
       if(Middle_Err>0)
       Middle_Err=Middle_Err-1;     
       if(Middle_Err<0)
       Middle_Err=Middle_Err+1;
      }
    }
    else Middle_Err=Middle_Err;   
    if(curve_to_cross==0||(curve_to_cross==1)) 
    Find_Bothine();
    if(Rightlastfind&&Leftlastfind)//能够找到两边边界
    {
     if(Right-Left<65&&Cross_Road_Cnt>4)     //宽度有效
     {

        { 
          Normal_Cnt++;
          if(Normal_Cnt>=3)
          {
           RoadType=0;
           curve_to_cross=0;
          /*            跳回正常道路            */
          }
         
        }
     }
     else 
     {
      Rightlastfind=0;
      Leftlastfind=0;
      Normal_Cnt=0;
     }
    }
    else Normal_Cnt=0;
    if(RoadType!=1)
    {
      Cross_Road_Cnt=0;
    }
    else
    {
      if(Cross_Road_Cnt<10)
      Cross_Road_Cnt++; 
      if(Cross_Road_Cnt>3&&Cross_Road_Cnt<8&&curve_to_cross==0)/////////////////////////////////改
      {
        Leftlastfind=0;
        Rightlastfind=0;
      } 
    }
}
void Find_Bothine(void)
{
    uint8_t i=0,j=0,find=0;
    for(j=60;j>20;j--)
    {
       if((CCD_Diff[j]>=CCD_Diff[j-1])&&(CCD_Diff[j]>CCD_Diff[j+1])&&(CCD_Diff[j]>got_yuzhi)) //寻找正的峰值
      {
       break;
      }
    }
     for(i=70;i<110;i++)
     {
        if((CCD_Diff[i]<CCD_Diff[i-1])&&(CCD_Diff[i]<=CCD_Diff[i+1])&&(CCD_Diff[i]<-got_yuzhi))  //寻找负的峰值
        {
          break;
        }
     }
    if(j>25&&i<105)
    {
      find=1;
       Left=j;
       Right=i;
       Leftlastfind=1;
       Rightlastfind=1;  
    }
    else
    {    
      for(j=10;j<80;j++)         
      {
         if((CCD_Diff[j]>=CCD_Diff[j-1])&&(CCD_Diff[j]>CCD_Diff[j+1])&&(CCD_Diff[j]>got_yuzhi)) //寻找正的峰值
        {
          for(i=j+1;i<=120;i++) 
          {
            if((CCD_Diff[i]<CCD_Diff[i-1])&&(CCD_Diff[i]<=CCD_Diff[i+1])&&(CCD_Diff[i]<-got_yuzhi))  //寻找负的峰值
            {
              if(i-j>25)
              {
                 find=1;
                 Left=j;
                 Right=i;
                 Leftlastfind=1;
                 Rightlastfind=1; 
              }
              break;
            }
          }
        }
        if(find) break;
      } 
    }
    if(find==0)
    {
      Leftlastfind=0;
      Rightlastfind=0; 
    }
}

uint8_t FindBoundary(uint8_t mode)    //寻找最靠近中心的边界
{
  uint8_t find=0;
  if(mode==left)
  { 
   if(RisingEdgeCnt!=0)  
   { 
    for(RisingEdgeIndex=RisingEdgeCnt-1;RisingEdgeIndex>=0;RisingEdgeIndex--)
    {
     if(RisingEdge[RisingEdgeIndex]<75)
     {
       find=1;
       break;
     }
     if(RisingEdgeIndex==0)  break;
    }
   }
  }
   else
  {
    if(FallingEdgeCnt!=0)  
    {
     for(FallingEdgeIndex=0;FallingEdgeIndex<FallingEdgeCnt;FallingEdgeIndex++)
     {
      if(FallingEdge[FallingEdgeIndex]>53)
      {
       find=1;
       break;
      }
     }
    }
  }
  return find;
}

uint8_t FindNearest(uint8_t mode,uint8_t lastegde)
{
  uint8_t find=0;
  if(mode==left)
  {
     if(RisingEdgeCnt!=0)  
     {
      for(RisingEdgeIndex=0;RisingEdgeIndex<RisingEdgeCnt;RisingEdgeIndex++)
      {
       if(fabs(lastegde-RisingEdge[RisingEdgeIndex])<=3)//3
       {
         find=1;
         break;
       }
      }
     }
  }
  else
  {
    if(FallingEdgeCnt!=0)  
   {
     for(FallingEdgeIndex=0;FallingEdgeIndex<FallingEdgeCnt;FallingEdgeIndex++)
     {
       if(fabs(lastegde-FallingEdge[FallingEdgeIndex])<=3)//3
       {
         find=1;
         break;
       }
     }
   }
  }
  return find;
}
void Barraicade(void)
{
  int i; 
   if(Right_Or_Left==left) //障碍在左边
   {
     if(FindNearest(right,Rightlast))
     {
      Right=FallingEdge[FallingEdgeIndex];
      Rightlastfind=1;
     }
     else Rightlastfind=0;
     if(Leftlastfind&&Rightlastfind)
      Reference_Width=Right-Left;
     else
     {
      Left=Right-Reference_Width;
     }
   }
   else   //障碍在右边
   { 
     if(FindNearest(left,Leftlast))
     {
      Left=RisingEdge[RisingEdgeIndex];
      Leftlastfind=1;
     }
     else Leftlastfind=0;
     if(Leftlastfind&&Rightlastfind)
     Reference_Width=Right-Left;
     else
     {
       Right=Left+Reference_Width;
     }
   }
   Turn_State_Cnt++; 
}
/**********************************************************/
/**********************************************************/
void Is_Barraicade()
{
  int left_edge,right_edge,left_edge_find,right_edge_find;
  int i;
  float half_width=0;
  half_width=Previous_Width[0]/2;
  if(Leftlastfind&&Rightlastfind)
  {
    right_edge_find=0;
    left_edge_find=0;
    if(FallingEdgeCnt>0)
    {
      for(i=0;i<FallingEdgeCnt;i++)
      {
       if((FallingEdge[i]>Leftlast)&&(FallingEdge[i]+7<Rightlast)) 
       {
        left_edge=FallingEdge[i]; 
        left_edge_find=1;
        break;
       }
      }
    }  
    if(RisingEdgeCnt>0)  
   {
    for(i=RisingEdgeCnt-1;i>=0;i--)
    {
       if((RisingEdge[i]<Rightlast)&&(RisingEdge[i]-7>Leftlast)) 
       {
        right_edge=RisingEdge[i]; 
        right_edge_find=1;
        break;
       }
    }
   }
   
   if(right_edge_find&&left_edge_find)
   {
     if(((right_edge-left_edge)*1.0/Previous_Width[0])>0.2)  //大于宽度的1/5 本来是1/4
     {
       RoadType=2;
       Turn_State_Cnt=0;        
       Turn_State=0; 
       if(Rightlast-right_edge>half_width) {
        Right_Or_Left=left; //左边出现了障碍 
        } 
       if(left_edge-Leftlast>half_width) {
        Right_Or_Left=right;  //右边出现了障碍
       } 

     }
   }  
 }
}
/**********************************************************/
/**********************************************************/
void Push_And_Pull(float *buff,int len,float newdata)
{
 int i;
 for(i=len-1;i>0;i--)
 {
   *(buff+i)=*(buff+i-1);
 }
   *buff=newdata; 
}
/**********************************************************/
/**********************************************************/
void Hill()
{
  //////////用来在坡道上搜索线的变量/////////
  uint8_t i=0,j=0,threshold_t;
  static uint8_t lost_cnt;
  ////////////////////////////
  static uint8_t  Confirm_Cnt=0;  
   if(RoadTypeConfirm==0)  //还没确认
  {
    Normal(); //调用normal函数来搜线 
    if(Confirm_Cnt<5)      //用5个周期来确认
    {
      if(Leftlastfind==0||Rightlastfind==0||fabs(Middle_Err)>5)   //有一条边没找到说明这个其实是弯道
      {
       RoadType=0; 
       Confirm_Cnt=0;
      } 
     else Confirm_Cnt++;
    }
    else RoadTypeConfirm=1;     //赛道类型确认了，确实是坡道 
    lost_cnt=0;
  }
  else                                  //已经判断是坡道了
  { 
    Leftlastfind=0;
    Rightlastfind=0;
    threshold_t=Threshold;
    while((Leftlastfind==0)&&(threshold_t*1.5>Threshold))
    {
       for(i=Leftlast-6;i<=Leftlast+6;i++) ////////////////////////////////////////////////////////////////////////////////////////////???????????????????????????         
      {
        if((CCD_Diff[i]>=CCD_Diff[i-1])&&(CCD_Diff[i]>CCD_Diff[i+1])&&(CCD_Diff[i]>threshold_t))  //寻找负的峰值
        {  
          Left=i;
          Leftlastfind=1;
        }
      }
      threshold_t=threshold_t-2;
    } 
    threshold_t=Threshold;
    while((Rightlastfind==0)&&(threshold_t*1.5>Threshold))
    {
      for(j=Rightlast+6;j>=Rightlast-6;j--)
      {
        if((CCD_Diff[j]<CCD_Diff[j-1])&&(CCD_Diff[j]<=CCD_Diff[j+1])&&(CCD_Diff[j]<-threshold_t)) //寻找正的峰值
        {
           Right=j;
           Rightlastfind=1;
        }
      }
      threshold_t=threshold_t-2;
    }
    if((Leftlastfind==1)&&(Rightlastfind==0))/////////////////////////////////////??????????????????????????????????????????????????????????????????????????????
    {
       for(j=Left+10;(j<=Left+50)&&(j<120);j++)
      {
        if((CCD_Diff[j]<CCD_Diff[j-1])&&(CCD_Diff[j]<=CCD_Diff[j+1])&&(CCD_Diff[j]<-Threshold)) //寻找正的峰值
        {
           Right=j;
           Rightlastfind=1;
           break;
        }
      }
    }
      
    if((Leftlastfind==0)||(Rightlastfind==1)) 
    {
       for(i=Right-10;(i>=Right-50)&&(i>10);i--) ////////////////////////////////////////////////////////////////////////////////////////////???????????????????????????         
      {
        if((CCD_Diff[i]>=CCD_Diff[i-1])&&(CCD_Diff[i]>CCD_Diff[i+1])&&(CCD_Diff[i]>threshold_t))  //寻找负的峰值
        {  
          Left=i;
          Leftlastfind=1;
          break;
        }
      }
    }
    
    if(Leftlastfind==0||Rightlastfind==0)  ////////////////////////////////////////////////////////////////////////////////////////////???????????????????????????????????
    { 
      lost_cnt++;
      if(Right-Left>50||(lost_cnt>=3&&Right-Left>30)||(lost_cnt>=5&&Right-Left<20))
      {
        RoadType=0;
        Confirm_Cnt=0;
        RoadTypeConfirm=0;
      }
    }
    else lost_cnt=0;
  }    
}                 
/**********************************************************/
/**********************************************************/
void Is_Hill()
{
   static int Width_Increase_Cnt=0,Straight_Cnt; 
   float slope;
   int i;
   if((fabs(Middle_Err)<=3)&&(RoadType!=3)&&Rightlastfind&&Leftlastfind)    //偏差小于一定值 在直道，用于判定坡道。跑动距离大于0.5，再判断值，防止起跑误判
   {
       Straight_Cnt++;
       if(Straight_Cnt>14)                //计算连续15次的值                             //前15个状态一直是直道
       {
          Width_Increase_Cnt=0;
          slope=Slope_Calculate(0,8,Previous_Error);           
          for(i=0;i<10;i++)                        
         {
           Width_Increase_Cnt+=Previous_Width[i]-Previous_Width[i+1];
         }
          
         if((Width_Increase_Cnt>=5)&&(fabs(slope)<2))      //只要宽度增加的次数大于5且变化的角度大于-3度则认为找打了坡道  排除由于车模角度减小带来的干扰
         {
           RoadType=3;
           RoadTypeConfirm=0;
         } 
       }
   }
   else
   {
     Straight_Cnt=0;
   }     
}









