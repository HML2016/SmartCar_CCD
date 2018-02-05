#include "all.h"
#define BUS_FREQ     80000000		//总线频率，运用了PLL超频技术
#define PWM_PRCLK   0x11			//CH_A = CH_B = BUS_FREQ/2
#define CH_A_FREQ    40000000		//时钟A频率
#define CH_B_FREQ    40000000		//时钟B频率
#define CH_SA_FREQ   2000000		//时钟SA频率
#define CH_SB_FREQ   2000000		//时钟SB频率
#define CH_SA_DIV    (CH_A_FREQ / (CH_SA_FREQ * 2))	 //CH_SA_DIV = 10
#define CH_SB_DIV    (CH_B_FREQ / (CH_SB_FREQ * 2))	 //CH_SB_DIV = 10
#define STEER_FREQ   100			//舵机频率100 Hz
#define MOTOR_FREQ  10000			//DC电动机频率10k
//uint	M_PACNT;					//读取脉冲累加器的计数值
/***************总线频率的设定****************************/
void SetBusCLK_80M(void)//设置总线时钟频率是40M
{      
    CLKSEL_PLLSEL = 0;			//不使能锁相环时钟  CLKSEL是时钟选择寄存器   PLLSEL--PLL时钟选择为系统时钟
    PLLCTL_PLLON=1;				//锁相环电路允许 PPLON开启锁相环   PPLCTL为PLL控制寄存器
    
    /* SYNR = (VCOFRQ1 | VCOFRQ0 | SYNR5 | SYNR4 | //
                 SYNR3 | SYNR2 | SYNR1 | SYNR0 | //		)   */ 
  SYNR =0xc0 | 0x09;  // VCO_clock=2*osc_clock*(1+SYNR)/(1+REFDV)=80 MHz; 时钟合成器寄存器  SYNR寄存器来控制锁相环的倍频系数
                        // VCOFRQ[1:0]=1:1,代表VCO_clock在80~160 MHz之间    并确定VCOCLK的频率范围
                            
    /* REFDV = (REFFRQ1 | REFFRQ1 | REFDV5 | REFDV4 | //
                 REFDV3 | REFDV2 | REFDV1 | REFDV0 | //
                )   */                                              
  REFDV=0x80 | 0x01;// VCO_clock=2*osc_clock*(1+SYNR)/(1+REFDV)=80 MHz;
 					// REF_clock=osc_clock/(REFDV+1)=16/(1+1)=8 MHz
 					// REFFRQ[1:0]=1:0,因为参考时钟在6~12 MHz之间

    POSTDIV=0x00; 				//VCO_clock = PLL_clock   POST分频寄存器控制VCOCLK与PLLCLK之间的分频系数
    
    _asm(nop);					//短暂延时，等待时钟频率稳定
    _asm(nop);
    
    while(!(CRGFLG_LOCK==1))	//时钟频率已稳定，锁相环频率锁定  CRGFLG时钟系统标志寄存器 
    {
      ;
    }
    
    CLKSEL_PLLSEL =1; 			//使能锁相环时钟
}

//CH_SA_FREQ =2M
//CH_SB_FREQ =2M

//此处的直流电机分别接的是 1 ，3通道，舵机接的是5通道
/***********************************************************/
/***************定时器的的设定***************************/

void PIT_Init(void)// PIT定时器使能，用来触发单片机的外围设备，也可以用来产生周期中断
{
    PITCE_PCE0 = 1; //PIT通道使能寄存器  PCE0使能            
    PITMUX_PMUX0 = 0; //PIT微时基选择位 PMUX0使能          
    PITMTLD0 = 79;   //PIT微定时器加载寄存器 PITMTLD0或1使能         
    PITLD0 = 999;     //PIT加载寄存器（0~7）         
    PITINTE_PINTE0 = 1; //PIT中断使能寄存器        
    PITCFLMT_PITE = 1;  //PITE--PIT模块使能位       
}
/***********************************************************/

/******************AD初始化***********************************/
void AD_Init(void) 
{
    //ATD0DIEN=0x00; 全部使用模拟输入
    ATD0CTL0 = 0x0F;   /* Set wrap around AD循环采集到AN15 */
    ATD0CTL1 = 0x4F;   /* 12-bit data  确定数模转换的精度为12位，采样前放电*/
    ATD0CTL2 = 0x40;   /* Fast Flag Clear 快速清除标志位，禁止外部触发，禁止中断*/
    ATD0CTL3 = 0x88;   /* Right justified data,Conversion Sequence Length = 1 右对齐模式每个序列的转换长度为1*/
    ATD0CTL4 = 0x04;   /* Set sample time and prescaler是预分频器 采样时间的ATD时钟周期数4个 ATD时钟预分频器时钟频率为F=Fbus/2/5*/
}
/***********************************************************/
/*******************使能CCD****************************************/
void CCD_IO_Init(void) //
{
    TSL1401_CLK_DDR = 1;//TSL1401_CLK_DDR     DDRA_DDRA0   CCD1输出
    TSL1401_SI_DDR  = 1;//TSL1401_SI_DDR      DDRA_DDRA1
    TSL1401_CLK = 0;//TSL1401_CLK         PORTA_PA0
    TSL1401_SI  = 0;//TSL1401_SI          PORTA_PA1
}
/***********************************************************/
 /***************脉冲累加器的设定***************************/
void PAC_Init(void)	            
{
     PACTL = 0x40;     				//脉冲累加器A允许
     PACNT = 0x00;     				//脉冲累加器A计数初值
}
void ECTInit(void) 
{    
    TIOS_IOS0 = 0; // channel 0 as output compare
    TIE_C0I = 1;  // 使能 channel 0 中断
    
    TCTL4_EDG0A = 1;
    TCTL4_EDG0B = 0;  // 检测上升沿
    
    TSCR1_TEN  = 1;  //timer enable    
}
/***********************************************************/
/***************脉冲初始设定***************************/
void PWM_Init(void)
{
   
    PWMPRCLK = 0x11;           //A时钟和B时钟的分频系数为2,频率为40MHz
    PWMSCLA = 0x0A;	
    PWMSCLB = 0x0A;				//SA SB时钟频率为2MHZ
    
    //PWMCTL_CON45 = 1;					// 16位级联PWM45
    //PWMCTL_CON67 = 1;					// 16位级联PWM67
    PWMCTL_CON01 = 1;					// 16位级联PWM01
    //PWMCLK_PCLK4 = 1;					// PWM4使用SA
    //PWMCLK_PCLK6 = 1;					// PWM6使用SB
    //PWMCLK_PCLK1 = 1;					// PWM1使用SA
    PWMCLK=0xf2;
    
    PWMPER01 = (CH_SA_FREQ / STEER_FREQ);		// 写PWM01的周期寄存器
    PWMPER4 = (CH_SB_FREQ / MOTOR_FREQ);		// 写PWM45的周期寄存器  200*1/2M
    PWMPER5 = (CH_SB_FREQ / MOTOR_FREQ);
    PWMPER6 = (CH_SB_FREQ / MOTOR_FREQ);		// 写PWM67的周期寄存器  200
    PWMPER7 = (CH_SB_FREQ / MOTOR_FREQ);
    PWMPOL_PPOL1 = 1;							// 极性为正
    PWMPOL_PPOL4 = 1;							// 极性为正
    PWMPOL_PPOL5 = 1;
    PWMPOL_PPOL6 = 1; 
    PWMPOL_PPOL7 = 1;
    PWMCAE = 0x00;								// 左对齐
    PWME_PWME1 = 1;								// 使能PWM45 
} 
/***********************************************************/

void LED_Init(void) {
    LED1_DDR = 1;
    LED2_DDR = 1;
    LED1 = 1;
    LED2 = 1;
}













