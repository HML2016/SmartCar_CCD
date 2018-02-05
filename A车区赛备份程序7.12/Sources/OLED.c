#include "derivative.h" 
#include "OLED.h"
#include "6x8.h"
#include "8x16.h"
#include "GB1616.h"
/*************************************************************/
/*                    向OLED屏写入数据                       */
/*************************************************************/
void OLED_WrDat(unsigned char Data)
{
    unsigned char j = 8;
    DC   = 1;
    CLK  = 0;  
    while (j--)
    {
        if (Data & 0x80)
            MOSI = 1;
        else
            MOSI = 0;
        CLK = 1; 
        asm("nop"); 
        CLK = 0;    
        Data <<= 1;    
    }
}
/*************************************************************/
/*                    向OLED屏写入命令                       */
/*************************************************************/
void OLED_WrCmd(unsigned char Data)
{
    unsigned char i = 8;
    DC   = 0;
    CLK  = 0;
    while (i--)
    {
        if (Data & 0x80)
        {
            MOSI = 1;
        }
        else
        {
            MOSI = 0;
        }
        CLK = 1;
        asm("nop");           
        CLK = 0;    
        Data <<= 1;   
    }  
}


/*************************************************************/
/*                          清屏命令                         */
/*************************************************************/
void OLED_Fill(unsigned char ucData)
{
    unsigned char ucPage,ucColumn;
    
    for(ucPage = 0; ucPage < 8; ucPage++)
    {
        OLED_WrCmd(0xb0 + ucPage);  //0xb0+0~7表示页0~7
        OLED_WrCmd(0x00);           //0x00+0~16表示将128列分成16组其地址在某组中的第几列
        OLED_WrCmd(0x10);           //0x10+0~16表示将128列分成16组其地址所在第几组
        for(ucColumn = 0; ucColumn < 128; ucColumn++)
        {
            OLED_WrDat(ucData);
        }
    }
} 


/*************************************************************/
/*             设定显示坐标，X,Y分别为横坐标和纵坐标         */
/*************************************************************/
void OLED_SetPos(unsigned char X, unsigned char Y)
{ 
    OLED_WrCmd(0xb0 + Y);
    OLED_WrCmd(((X & 0xf0) >> 4) | 0x10);
    OLED_WrCmd((X & 0x0f) | 0x00); 
} 

//以下初始化的函数是由OLED厂家提供的初始化过程
//使用时可以不必理会这些函数的具体意义
//如果做一些高级应用，请参照OLED屏的手册进行分析
void SetAddressingMode(unsigned char ucData)
{
    OLED_WrCmd(0x20);        // Set Memory Addressing Mode
    OLED_WrCmd(ucData);      // Default => 0x02
                            // 0x00 => Horizontal Addressing Mode
                            // 0x01 => Vertical Addressing Mode
                            // 0x02 => Page Addressing Mode
}

void SetColumnAddress(unsigned char a, unsigned char b)
{
    OLED_WrCmd(0x21);        // Set Column Address
    OLED_WrCmd(a);           // Default => 0x00 (Column Start Address)
    OLED_WrCmd(b);           // Default => 0x7F (Column End Address)
}

void SetStartLine(unsigned char ucData)
{
    OLED_WrCmd(0x40|ucData); // Set Display Start Line
                            // Default => 0x40 (0x00)
}

void SetContrastControl(unsigned char ucData)
{
    OLED_WrCmd(0x81);        // Set Contrast Control
    OLED_WrCmd(ucData);      // Default => 0x7F
}

void SetChargePump(unsigned char ucData)
{
    OLED_WrCmd(0x8D);        // Set Charge Pump
    OLED_WrCmd(0x10|ucData); // Default => 0x10
                            // 0x10 (0x00) => Disable Charge Pump
                            // 0x14 (0x04) => Enable Charge Pump
}

void SetSegmentRemap(unsigned char ucData)
{
    OLED_WrCmd(0xA0|ucData); // Set Segment Re-Map
                            // Default => 0xA0
                            // 0xA0 (0x00) => Column Address 0 Mapped to SEG0
                            // 0xA1 (0x01) => Column Address 0 Mapped to SEG127
}

void SetEntireDisplay(unsigned char ucData)
{
    OLED_WrCmd(0xA4|ucData); // Set Entire Display On / Off
                            // Default => 0xA4
                            // 0xA4 (0x00) => Normal Display
                            // 0xA5 (0x01) => Entire Display On
}

void SetInverseDisplay(unsigned char ucData)
{
    OLED_WrCmd(0xA6|ucData); // Set Inverse Display On/Off
                            // Default => 0xA6
                            // 0xA6 (0x00) => Normal Display
                            // 0xA7 (0x01) => Inverse Display On
}

void SetMultiplexRatio(unsigned char ucData)
{
    OLED_WrCmd(0xA8);        // Set Multiplex Ratio
    OLED_WrCmd(ucData);      // Default => 0x3F (1/64 Duty)
}

void SetDisplayOnOff(unsigned char ucData)
{
    OLED_WrCmd(0xAE|ucData); // Set Display On/Off
                            // Default => 0xAE
                            // 0xAE (0x00) => Display Off
                            // 0xAF (0x01) => Display On
}

void SetStartPage(unsigned char ucData)
{
    OLED_WrCmd(0xB0|ucData); // Set Page Start Address for Page Addressing Mode
                            // Default => 0xB0 (0x00)
}

void SetCommonRemap(unsigned char ucData)
{
    OLED_WrCmd(0xC0|ucData); // Set COM Output Scan Direction
                            // Default => 0xC0
                            // 0xC0 (0x00) => Scan from COM0 to 63
                            // 0xC8 (0x08) => Scan from COM63 to 0
}

void SetDisplayOffset(unsigned char ucData)
{
    OLED_WrCmd(0xD3);        // Set Display Offset
    OLED_WrCmd(ucData);      // Default => 0x00
}

void SetDisplayClock(unsigned char ucData)
{
    OLED_WrCmd(0xD5);        // Set Display Clock Divide Ratio / Oscillator Frequency
    OLED_WrCmd(ucData);      // Default => 0x80
                            // D[3:0] => Display Clock Divider
                            // D[7:4] => Oscillator Frequency
}

void SetPrechargePeriod(unsigned char ucData)
{
    OLED_WrCmd(0xD9);        // Set Pre-Charge Period
    OLED_WrCmd(ucData);      // Default => 0x22 (2 Display Clocks [Phase 2] / 2 Display Clocks [Phase 1])
                            // D[3:0] => Phase 1 Period in 1~15 Display Clocks
                            // D[7:4] => Phase 2 Period in 1~15 Display Clocks
}

void SetCommonConfig(unsigned char ucData)
{
    OLED_WrCmd(0xDA);        // Set COM Pins Hardware Configuration
    OLED_WrCmd(0x02|ucData); // Default => 0x12 (0x10)
                            // Alternative COM Pin Configuration
                            // Disable COM Left/Right Re-Map
}

void SetVCOMH(unsigned char ucData)
{
    OLED_WrCmd(0xDB);        // Set VCOMH Deselect Level
    OLED_WrCmd(ucData);      // Default => 0x20 (0.77*VCC)
}

void SetNop(void)
{
    OLED_WrCmd(0xE3);        // Command for No Operation
}

/*************************************************************/
/*                      初始化OLED屏                         */
/*************************************************************/
void OLED_Init(void)        
{
    unsigned char i;
    OLED_dir=0XFF;
    CLK=1;      
    RST=0;
    
    for(i=0;i<100;i++)
    {
        asm("nop");         //等待屏复位  
    }
    
    RST=1;

    SetDisplayOnOff(0x00);     // Display Off (0x00/0x01)
    SetDisplayClock(0x80);     // Set Clock as 100 Frames/Sec
    SetMultiplexRatio(0x3F);   // 1/64 Duty (0x0F~0x3F)
    SetDisplayOffset(0x00);    // Shift Mapping RAM Counter (0x00~0x3F)
    SetStartLine(0x00);        // Set Mapping RAM Display Start Line (0x00~0x3F)
    SetChargePump(0x04);       // Enable Embedded DC/DC Converter (0x00/0x04)
    SetAddressingMode(0x02);   // Set Page Addressing Mode (0x00/0x01/0x02)
    SetSegmentRemap(0x01);     // Set SEG/Column Mapping     0x00左右反置 0x01正常
    SetCommonRemap(0x08);      // Set COM/Row Scan Direction 0x00上下反置 0x08正常
    SetCommonConfig(0x10);     // Set Sequential Configuration (0x00/0x10)
    SetContrastControl(0xCF);  // Set SEG Output Current
    SetPrechargePeriod(0xF1);  // Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
    SetVCOMH(0x40);            // Set VCOM Deselect Level
    SetEntireDisplay(0x00);    // Disable Entire Display On (0x00/0x01)
    SetInverseDisplay(0x00);   // Disable Inverse Display On (0x00/0x01)  
    SetDisplayOnOff(0x01);     // Display On (0x00/0x01)
    OLED_Fill(0x00);           // 清屏
    OLED_SetPos(0,0);
} 
 
/*************************************************************/
/*                     显示6X8的字符                         */
//函数名：void OLED_6x8Char(unsigned char X1, unsigned char Y1, unsigned char Data1)
//功能描述：显示一个6x8标准ASCII字符
//参数：x为显示的横坐标0~122，y为页范围0～7，ch要显示的字符
//返回：无
/*************************************************************/
void OLED_6x8Char(unsigned int X1, unsigned int Y1, unsigned char Data1)
{
    unsigned char xx, temp;     
       
    temp = Data1-32;
    if(X1 > 122)
    {
        X1 = 0;
        Y1++;
    }
    
    OLED_SetPos(X1, Y1);
    
    for(xx = 0; xx < 6; xx++)
    {     
        OLED_WrDat(F6x8[temp][xx]);  
    }
}

/*************************************************************/
/*                     显示6X8的字符串                       */
//函数名：void OLED_6x8Str(unsigned char X1, unsigned char Y1, unsigned char *ucStr)
//功能描述：写入一组6x8标准ASCII字符串
//参数：x为显示的横坐标0~122，y为页范围0～7，要显示的字符串
//返回：无
/*************************************************************/
void OLED_6x8Str(unsigned int X1, unsigned int Y1, unsigned char *ucStr)
{
    unsigned char yy, temp1; 

    while(*ucStr)
    {
        temp1 = *ucStr - 32;
        if(X1 > 122)
        {
            X1 = 0;
            Y1++;
        }
        
        OLED_SetPos(X1, Y1);
        
        for(yy = 0; yy < 6; yy++)
        {     
            OLED_WrDat(F6x8[temp1][yy]);  
        }
        X1 += 6;
        ucStr++;
    }
        
}



/*************************************************************/
/*                     显示8X16的字符串                      */
//函数名：LED_P8x16Str(byte x,byte y,byte *p)
//功能描述：写入一组8x16标准ASCII字符串
//参数：x为显示的横坐标0~120，y为页范围0～3，要显示的字符串
//返回：无
/*************************************************************/
void OLED_8x16Char(unsigned int X1, unsigned int Y1, unsigned char Data1)
{
    unsigned char yy, temp1;

    Y1 <<= 1;
    
    temp1 = Data1 - 32;
    if(X1 > 120)
    {
        X1 = 0;
        Y1 += 2;
    }
    OLED_SetPos(X1, Y1);   
    
    for(yy = 0; yy < 8; yy++) 
    {
        OLED_WrDat(F8X16[(temp1 << 4) + yy]);
    }
    
    OLED_SetPos(X1, Y1 + 1);   
    
    for(yy = 0; yy < 8; yy++) 
    {
        OLED_WrDat(F8X16[(temp1 << 4) + yy + 8]);
    }
    X1 += 8;

}

/*************************************************************/
/*                     显示16X16的字符                       */
/*************************************************************/
void OLED_16x16Char(unsigned int X1,unsigned int Y1,unsigned char c[2])
{
    unsigned char xx, k;

    Y1 <<= 1;
    if(X1 > 112)
    {
        X1 = 0;
        Y1 += 2;
    }
    
    for (k=0;k<64;k++) //64标示自建汉字库中的个数，循环查询内码
    { 
        if ((codeGB_16[k].Index[0]==c[0])&&(codeGB_16[k].Index[1]==c[1]))
        { 

            OLED_SetPos(X1, Y1);
            for(xx = 0; xx < 16; xx++)               
                OLED_WrDat(codeGB_16[k].Msk[xx]);   
            
            OLED_SetPos(X1,Y1 + 1); 
            for(xx = 0;xx <16;xx++)          
                OLED_WrDat(codeGB_16[k].Msk[16+xx]);
            break;
        }  
    }	

}

/*************************************************************/
/*                         显示字符串                        */
/*************************************************************/
void OLED_PutString(unsigned int X1,unsigned int Y1,unsigned char *s) 
{
    unsigned char l=0;
    while(*s) 
    {
        if( *s < 0x80) 
        {
            OLED_8x16Char(X1,Y1,*s);
            s++;X1+=8;
            if(X1 > 120)
            {
                X1 = 0;
                Y1+=1;
            }

        }
        else
        {
            OLED_16x16Char(X1,Y1,(unsigned char*)s);
            s+=2;X1+=16;
            if(X1 > 112)
            {
                X1 = 0;
                Y1+=1;
            }
        }
    }
}
//==============================================================
//函数名：  void LED_PrintValue(uchar x, uchar y, char data);
//功能描述：将一个char型数转换成3位数进行显示
//参数：x的范围为0～122，y为页的范围0～7，data为需要转化显示的数值
//返回：无
//==============================================================
void OLED_PrintValueC(unsigned char x, unsigned char y,  char data)
{
	unsigned char shuzhu[4];
	if(data < 0)
	{
		OLED_8x16Char(x,y,'-');
		data = - data;
	}
	else
	{
		OLED_8x16Char(x,y,'+');
	}
	shuzhu[0]= data/100+48;
	shuzhu[1]= (data%100)/10+48;
	shuzhu[2]= data%10+48;
	shuzhu[3]=0;
  OLED_PutString(x+8,y,shuzhu);
}

//==============================================================
//函数名：  void LED_PrintValueI(uchar x, uchar y, int data);
//功能描述：将一个int型数转换成5位数进行显示
//参数：x的范围为0～120，y为页的范围0～7，data为需要转化显示的数值
//返回：无
//==============================================================
void OLED_PrintValueI(unsigned int x, unsigned int y, int data)
{
	unsigned char shuzhu[6];
	if(data < 0)
	{
		OLED_8x16Char(x,y,'-');
		data = - data;
	}
	else
	{
		OLED_8x16Char(x,y,'+');
	}

	shuzhu[0]= data/10000+48;
	shuzhu[1]= (data%10000)/1000+48;
  shuzhu[2]= (data%1000)/100+48;
	shuzhu[3]= (data%100)/10+48;
	shuzhu[4]= data%10+48;
	shuzhu[5]=0;
	OLED_PutString(x+8,y,shuzhu);
}

 /*void OLED_PrintValueFP(u8 x, u8 y, uint data, u8 num)
 {
 unsigned int m,i,j,k;
 	OLED_6x8Char(x, y, '.');
	m= data/1000;
	i = (data%1000)/100;
	j = (data%100)/10;
	k = data%10;
	switch(num)
	{
		case 1:  	OLED_6x8Char(x+6,y,k+48);
				break;
		case 2:  	OLED_6x8Char(x+6,y,j+48);
				OLED_6x8Char(x+12,y,k+48);
				break;
		case 3:	OLED_6x8Char(x+6,y,i+48);
				OLED_6x8Char(x+12,y,j+48);
				OLED_6x8Char(x+18,y,k+48);
				break;
		case 4: 	OLED_6x8Char(x+6,y,m+48);
				OLED_6x8Char(x+12,y,i+48);
				OLED_6x8Char(x+18,y,j+48);
				OLED_6x8Char(x+24,y,k+48);
				break;
	}
 }
  */
//==============================================================
//函数名：   void LED_PrintValueF(uchar x, uchar y, float data, uchar num);
//功能描述：将一个float型数转换成整数部分5位带小数和符号的数据并进行显示
//参数：x的范围为0～62，y为页的范围0～7，data为需要转化显示的数值整数部分最多位5位  num表示保留的小数位0~4
//返回：无
//==============================================================
/*void OLED_PrintValueF(u8 x, u8 y, float data, u8 num)
 {
 	unsigned int l,m,i,j,k;  //万千百十个
 	u8 databiti = 6; //整数位数
 	uint tempdataui = 0;
  	int tempdataii = (int)data; //整数部分
 	long int tempdatalp = (long int)((data - (int)data)*10000); //取小数位后4位

 	//整数部分显示
 	if(data < 0.0000001)  LED_P6x8Char(x, y,'-');
 	else LED_P6x8Char(x, y,'+');
	if(tempdataii < 0)tempdataii = - tempdataii;  //去掉整数部分负号
	tempdataui = tempdataii;
 	l  = tempdataui/10000;
	m= (tempdataui%10000)/1000;
	i = (tempdataui%1000)/100;
	j = (tempdataui%100)/10;
	k = tempdataui%10;
 	if (l != 0)  //五位
 	{
 		OLED_6x8Char(x+6,y,l+48);
 		OLED_6x8Char(x+12,y,m+48);
		OLED_6x8Char(x+18,y,i+48);
		OLED_6x8Char(x+24,y,j+48);
		OLED_6x8Char(x+30,y,k+48);
 	}
 	else if(m != 0) //四位
 	{
 		databiti = 5;
 		OLED_6x8Char(x+6,y,m+48);
 		OLED_6x8Char(x+12,y,i+48);
		OLED_6x8Char(x+18,y,j+48);
		OLED_6x8Char(x+24,y,k+48);
 	}
  	else if(i != 0) //三位
  	{
  		databiti = 4;
  	OLED_6x8Char(x+6,y,i+48);
 		OLED_6x8Char(x+12,y,j+48);
		OLED_6x8Char(x+18,y,k+48);
  	}
  	else if(j != 0) //两位
  	{
    		databiti = 3;
  	OLED_6x8Char(x+6,y,j+48);
 		OLED_6x8Char(x+12,y,k+48);
  	}
	else
	{
		databiti = 2;
		OLED_6x8Char(x+6,y,k+48);
	}
 	if(tempdatalp < 0)tempdatalp = - tempdatalp;	//去掉小数部分负号
	switch(num)
	{
		case 0: break;
		case 1:  OLED_PrintValueFP(x + databiti * 6, y, (unsigned int)(tempdatalp / 1000),num);break;
		case 2:  OLED_PrintValueFP(x + databiti * 6, y, (unsigned int)(tempdatalp / 100),num);break;
		case 3:  OLED_PrintValueFP(x + databiti * 6, y, (unsigned int)(tempdatalp / 10),num);break;
		case 4:  OLED_PrintValueFP(x + databiti * 6, y, (unsigned int)(tempdatalp),num);break;
	}
 }*/
 
 