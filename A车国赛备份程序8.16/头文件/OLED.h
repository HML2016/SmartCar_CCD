#include <hidef.h> 

#define CLK           PTM_PTM3       //接D0
#define MOSI          PTM_PTM2       //接D1
#define RST           PTM_PTM1       //接RST
#define DC            PTM_PTM0       //接DC
#define OLED_dir      DDRM

#define LED_IMAGE_WHITE       1
#define LED_IMAGE_BLACK       0

#define LED_MAX_ROW_NUM      64
#define LED_MAX_COLUMN_NUM  128

void OLED_WrDat(unsigned char Data);
void OLED_WrCmd(unsigned char Data);
void OLED_SetPos(unsigned char X, unsigned char Y);
void OLED_Fill(unsigned char ucData);
void SetAddressingMode(unsigned char ucData);
void SetColumnAddress(unsigned char a, unsigned char b);
void SetStartLine(unsigned char ucData);
void SetContrastControl(unsigned char ucData);
void SetChargePump(unsigned char ucData);
void SetSegmentRemap(unsigned char ucData);
void SetEntireDisplay(unsigned char ucData);
void SetInverseDisplay(unsigned char ucData);
void SetMultiplexRatio(unsigned char ucData);
void SetDisplayOnOff(unsigned char ucData);
void SetStartPage(unsigned char ucData);
void SetCommonRemap(unsigned char ucData);
void SetDisplayOffset(unsigned char ucData);
void SetDisplayClock(unsigned char ucData);
void SetPrechargePeriod(unsigned char ucData);
void SetCommonConfig(unsigned char ucData);
void SetVCOMH(unsigned char ucData);
void SetNop(void);
void OLED_Init(void);

void OLED_6x8Char(unsigned char X1, unsigned char Y1, unsigned char Data1);
void OLED_6x8Str(unsigned char X1, unsigned char Y1, unsigned char ucStr[]);
void OLED_8x16Char(unsigned char X1, unsigned char Y1, unsigned char Data1);
void OLED_16x16Char(unsigned char X1,unsigned char Y1,unsigned char c[2]);
void OLED_PutString(unsigned char X1,unsigned char Y1,unsigned char *s); 
void OLED_image(void);