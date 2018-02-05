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
int leftfind,rightfind;    //���ұ��ض��ҵ��ˣ����ǲ���ֱ�Ӹ�ֵ������ 
int Turn_State;
int Turn_State_Cnt=0;
int CCD_Diff[128],RisingEdgeIndex,FallingEdgeIndex;
int Both_Line_find_cnt=0;
int Reference_Width=46; 
uint8_t RisingEdge[5],FallingEdge[5];
uint8_t Leftlast,Rightlast,Leftlastfind=0,Rightlastfind=0;
uint8_t curve_to_cross=0;
uint8_t RoadTypeConfirm; 
uint8_t Threshold; //��ֵ
float  Previous_Width[15];
float  Previous_Error[15];
extern int stopflag;         
/*********�����߼��*************/
int start_check(unsigned char *A,int interval,int yuzhi)//interval  ���  ���
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
  RisingEdgeCnt=0;//�����ؼ���
  FallingEdgeCnt=0;//�½��ؼ���
   for(i=0;i<5;i++)//��������
  {
    RisingEdge[i]=0;
    FallingEdge[i]=0;  
  }
   for(i=3;i<128;i++) //������Ĳ��ֵ
  {
    CCD_Diff[i]= Pixel[i]- Pixel[i-3];
    if(fabs(CCD_Diff[i])>Max_Peak) 
    Max_Peak=fabs(CCD_Diff[i]); 
  }
   for(i=4;i<127;i++)
  {
    if((CCD_Diff[i]>=CCD_Diff[i-1])&&(CCD_Diff[i]>CCD_Diff[i+1])&&(CCD_Diff[i]>got_yuzhi)) //Ѱ�����ķ�ֵ�������
    {
      if(RisingEdgeCnt<5)    //һ��ͼ�������5��������
      { 
       RisingEdge[RisingEdgeCnt]=i;
       RisingEdgeCnt++;  
      }
    } 
    if((CCD_Diff[i]<CCD_Diff[i-1])&&(CCD_Diff[i]<=CCD_Diff[i+1])&&(CCD_Diff[i]<-got_yuzhi))  //Ѱ�Ҹ��ķ�ֵ���ұ���
    {
      if(FallingEdgeCnt<5)    //һ��ͼ�������5��������
      {
       FallingEdge[FallingEdgeCnt]=i;
       FallingEdgeCnt++;
      }
    }
  }
  
 Prejudge();//Ԥ�к��� 
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
   Leftlast=Left;              // ���±߽���
   Rightlast=Right;            //���±߽���
   left_c=Left-1;              //��ȥ2����������
   right_c=Right;
   Is_Hill();
  if(Rightlastfind||Leftlastfind)
   {
     Middle_Err=64-(right_c+left_c)/2.0+1;  //��������2����ʧ����
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
   if(RisingEdgeCnt==0&&FallingEdgeCnt==0)//˵��û�������أ�ȫ��
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
  if(Leftlastfind==0&&Rightlastfind==0)    //�ϴ�û�б���
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
   if(Leftlastfind)						//�ϴ��ҵ�������
   {
    if(FindNearest(left,Leftlast))		//������ҵ�����
    {
      Left=RisingEdge[RisingEdgeIndex];
      Leftlastfind=1;			
      if(Rightlastfind==0)				//�ϴ��ҵ����ߣ�����ҵ����ߣ��ϴ�û�ҵ�����
      {
       if(FallingEdgeCnt)					//�ϴ��ҵ����ߣ�����ҵ����ߣ��ϴ�û�ҵ����ߣ�����ҵ�����
       {
         for(FallingEdgeIndex=0;FallingEdgeIndex<FallingEdgeCnt;FallingEdgeIndex++)
         { 
            rightfind=0;   //
            if(FallingEdge[FallingEdgeIndex]>Left)         //�ҵ�Ǳ�ڵı�����
           {  
             tempwidth = FallingEdge[FallingEdgeIndex]-Left;
             if(fabs(tempwidth-Reference_Width)>=2)       //���߳��ֵ�̫���ˣ���������
             {
               if(tempwidth>Reference_Width)  
                Reference_Width=Reference_Width+1;   //Ǳ�ڵ��������߱Ȳο���ȿ�̫��
               else                         
               {
                 Reference_Width=Reference_Width-1;   //Ǳ�ڵ��������߱Ȳο����խ̫��
               }
               Right=Left+Reference_Width;
               Rightlastfind = 0;
             }
             else 
             {
              Right=FallingEdge[FallingEdgeIndex];
              Rightlastfind=1;
             }
             rightfind=1;   //�ҵ������ˣ����ǻ���������ֵ
             break;
           }
         } 
           if(Rightlastfind==0&&rightfind==0)   //��Ȼ˵�������أ����Ǹ���������Ч
          {

           Right=Left+Reference_Width;
          }
       }
       else                        //û��������                                  
       {

              Right=Left+Reference_Width;
       }
      }
    }
    else  //�ϴ����ҵ����ߣ����û���ҵ�����
    {
       Leftlastfind=0;   //���� 
        /*           ��ת��ʮ��           */   
      if(FindBoundary(left))
     {
      if(RisingEdge[RisingEdgeIndex]<Leftlast)  //���û�ܹ��ҵ����ϴ����ڵı߽磬�����б߽�ȴ�����еı߽��
      {
        if(Rightlastfind==0&&((RoadType!=2)&&(RoadType!=3)))   //�ϴ�û���ҵ��ұ߽磬�����߽������⿿����ô��������ʮ����
        {
         RoadType=1; 
        }
      }
      else if(RisingEdge[RisingEdgeIndex]<Leftlast+8)   //��εı߽���ϴθ��������룬�������ǵĲ�ֵ������8
      {
         Left=RisingEdge[RisingEdgeIndex];
         Leftlastfind=1;                 //δ����		 
      }
     } 
      else                     //�����ҵ���߽�
     {
       if(Rightlastfind==0&&(RoadType!=4))  
       {
        RoadType=1;
       }
     }
    }
   }
   ///////////////////////////////
   if(Rightlastfind)					//�ϴ��ҵ�������
   {
    if(FindNearest(right,Rightlast))	//������ҵ�����
    {
      Right=FallingEdge[FallingEdgeIndex];
      Rightlastfind=1;
      if(Leftlastfind==0)				//�ϴ��ҵ�������,������ҵ�����,�ϴ��Ҳ�������
      {
       if(RisingEdgeCnt>0)				//�ϴ��ҵ�������,������ҵ�����,�ϴ��Ҳ�������,����ҵ�������
       {                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                
	      for(RisingEdgeIndex=RisingEdgeCnt-1;RisingEdgeIndex>=0;RisingEdgeIndex--)
         {
           leftfind=0;
           if(RisingEdge[RisingEdgeIndex]<Right)         //�ҵ�Ǳ�ڵı�����
           {
             tempwidth = Right-RisingEdge[RisingEdgeIndex];
             if (fabs(tempwidth-Reference_Width)>=2)       //���߳��ֵ�̫���ˣ���������
             {
              if(tempwidth>Reference_Width) 
                Reference_Width=Reference_Width+1;   //Ǳ�ڵ��������߱Ȳο���ȿ�̫��
              else                          
              {
                Reference_Width=Reference_Width-1;   //Ǳ�ڵ��������߱Ȳο����խ̫��
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
            if(Leftlastfind==0&&leftfind==0)   //��Ȼ˵�������أ����Ǹ���������Ч
            {

               Left=Right-Reference_Width;
            }
       }
       else                               //û��������Ŷ��
       { 

         Left=Right-Reference_Width;
       }
      }
    }
    else  //�ϴ����ҵ������û���ҵ����ϴ����ڽ�����
    {
      Rightlastfind=0;   //����
     /*           ��ת��ʮ��           */
     if(FindBoundary(right))   //�ܹ��ҵ��ұ߽�
     {
      if(FallingEdge[FallingEdgeIndex]>Rightlast)  //���û�ܹ��ҵ����ϴ����ڵı߽磬�����б߽�ȴ�����еı߽��
      {
        if(Leftlastfind==0&&((RoadType!=2)&&(RoadType!=3)))   //�ϴ�û���ҵ���߽磬����ұ߽������⿿����ô��������ʮ����
        {
         RoadType=1; 
        }
      }
      else if(FallingEdge[FallingEdgeIndex]>Rightlast-8)  //��εı߽���ϴθ��������룬�������ǵĲ�ֵ������8
      {
          Right=FallingEdge[FallingEdgeIndex];
          Rightlastfind=1;           //��ʵ��δ����
      }
     }
     else                     //�����ҵ��ұ߽�
     {
        if(Leftlastfind==0&&(RoadType!=4))   //�ϴ�û���ҵ���߽磬����ұ߽������⿿����ô��������ʮ����
       {
         RoadType=1;
       }
     }
    }
   }
   //�����ҵ����ߣ����²ο����
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
  
    //�������㣬���ӻ���  
  if(curve_to_cross==0)
  {  
    Rightlastfind=0;
    Leftlastfind=0; 
  }
  else
  {  
    if(Rightlastfind)
    {
        if(FindNearest(right,Rightlast))		//������ҵ�����
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
      if(FindNearest(left,Leftlast))		//������ҵ�����
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
    if(Rightlastfind&&Leftlastfind)//�ܹ��ҵ����߽߱�
    {
     if(Right-Left<65&&Cross_Road_Cnt>4)     //�����Ч
     {

        { 
          Normal_Cnt++;
          if(Normal_Cnt>=3)
          {
           RoadType=0;
           curve_to_cross=0;
          /*            ����������·            */
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
      if(Cross_Road_Cnt>3&&Cross_Road_Cnt<8&&curve_to_cross==0)/////////////////////////////////��
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
       if((CCD_Diff[j]>=CCD_Diff[j-1])&&(CCD_Diff[j]>CCD_Diff[j+1])&&(CCD_Diff[j]>got_yuzhi)) //Ѱ�����ķ�ֵ
      {
       break;
      }
    }
     for(i=70;i<110;i++)
     {
        if((CCD_Diff[i]<CCD_Diff[i-1])&&(CCD_Diff[i]<=CCD_Diff[i+1])&&(CCD_Diff[i]<-got_yuzhi))  //Ѱ�Ҹ��ķ�ֵ
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
         if((CCD_Diff[j]>=CCD_Diff[j-1])&&(CCD_Diff[j]>CCD_Diff[j+1])&&(CCD_Diff[j]>got_yuzhi)) //Ѱ�����ķ�ֵ
        {
          for(i=j+1;i<=120;i++) 
          {
            if((CCD_Diff[i]<CCD_Diff[i-1])&&(CCD_Diff[i]<=CCD_Diff[i+1])&&(CCD_Diff[i]<-got_yuzhi))  //Ѱ�Ҹ��ķ�ֵ
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

uint8_t FindBoundary(uint8_t mode)    //Ѱ��������ĵı߽�
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
   if(Right_Or_Left==left) //�ϰ������
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
   else   //�ϰ����ұ�
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
     if(((right_edge-left_edge)*1.0/Previous_Width[0])>0.2)  //���ڿ�ȵ�1/5 ������1/4
     {
       RoadType=2;
       Turn_State_Cnt=0;        
       Turn_State=0; 
       if(Rightlast-right_edge>half_width) {
        Right_Or_Left=left; //��߳������ϰ� 
        } 
       if(left_edge-Leftlast>half_width) {
        Right_Or_Left=right;  //�ұ߳������ϰ�
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
  //////////�������µ��������ߵı���/////////
  uint8_t i=0,j=0,threshold_t;
  static uint8_t lost_cnt;
  ////////////////////////////
  static uint8_t  Confirm_Cnt=0;  
   if(RoadTypeConfirm==0)  //��ûȷ��
  {
    Normal(); //����normal���������� 
    if(Confirm_Cnt<5)      //��5��������ȷ��
    {
      if(Leftlastfind==0||Rightlastfind==0||fabs(Middle_Err)>5)   //��һ����û�ҵ�˵�������ʵ�����
      {
       RoadType=0; 
       Confirm_Cnt=0;
      } 
     else Confirm_Cnt++;
    }
    else RoadTypeConfirm=1;     //��������ȷ���ˣ�ȷʵ���µ� 
    lost_cnt=0;
  }
  else                                  //�Ѿ��ж����µ���
  { 
    Leftlastfind=0;
    Rightlastfind=0;
    threshold_t=Threshold;
    while((Leftlastfind==0)&&(threshold_t*1.5>Threshold))
    {
       for(i=Leftlast-6;i<=Leftlast+6;i++) ////////////////////////////////////////////////////////////////////////////////////////////???????????????????????????         
      {
        if((CCD_Diff[i]>=CCD_Diff[i-1])&&(CCD_Diff[i]>CCD_Diff[i+1])&&(CCD_Diff[i]>threshold_t))  //Ѱ�Ҹ��ķ�ֵ
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
        if((CCD_Diff[j]<CCD_Diff[j-1])&&(CCD_Diff[j]<=CCD_Diff[j+1])&&(CCD_Diff[j]<-threshold_t)) //Ѱ�����ķ�ֵ
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
        if((CCD_Diff[j]<CCD_Diff[j-1])&&(CCD_Diff[j]<=CCD_Diff[j+1])&&(CCD_Diff[j]<-Threshold)) //Ѱ�����ķ�ֵ
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
        if((CCD_Diff[i]>=CCD_Diff[i-1])&&(CCD_Diff[i]>CCD_Diff[i+1])&&(CCD_Diff[i]>threshold_t))  //Ѱ�Ҹ��ķ�ֵ
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
   if((fabs(Middle_Err)<=3)&&(RoadType!=3)&&Rightlastfind&&Leftlastfind)    //ƫ��С��һ��ֵ ��ֱ���������ж��µ����ܶ��������0.5�����ж�ֵ����ֹ��������
   {
       Straight_Cnt++;
       if(Straight_Cnt>14)                //��������15�ε�ֵ                             //ǰ15��״̬һֱ��ֱ��
       {
          Width_Increase_Cnt=0;
          slope=Slope_Calculate(0,8,Previous_Error);           
          for(i=0;i<10;i++)                        
         {
           Width_Increase_Cnt+=Previous_Width[i]-Previous_Width[i+1];
         }
          
         if((Width_Increase_Cnt>=5)&&(fabs(slope)<2))      //ֻҪ������ӵĴ�������5�ұ仯�ĽǶȴ���-3������Ϊ�Ҵ����µ�  �ų����ڳ�ģ�Ƕȼ�С�����ĸ���
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









