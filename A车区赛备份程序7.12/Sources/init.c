#include "all.h"
#define BUS_FREQ     80000000		//����Ƶ�ʣ�������PLL��Ƶ����
#define PWM_PRCLK   0x11			//CH_A = CH_B = BUS_FREQ/2
#define CH_A_FREQ    40000000		//ʱ��AƵ��
#define CH_B_FREQ    40000000		//ʱ��BƵ��
#define CH_SA_FREQ   2000000		//ʱ��SAƵ��
#define CH_SB_FREQ   2000000		//ʱ��SBƵ��
#define CH_SA_DIV    (CH_A_FREQ / (CH_SA_FREQ * 2))	 //CH_SA_DIV = 10
#define CH_SB_DIV    (CH_B_FREQ / (CH_SB_FREQ * 2))	 //CH_SB_DIV = 10
#define STEER_FREQ   100			//���Ƶ��100 Hz
#define MOTOR_FREQ  10000			//DC�綯��Ƶ��10k
//uint	M_PACNT;					//��ȡ�����ۼ����ļ���ֵ
/***************����Ƶ�ʵ��趨****************************/
void SetBusCLK_80M(void)//��������ʱ��Ƶ����40M
{      
    CLKSEL_PLLSEL = 0;			//��ʹ�����໷ʱ��  CLKSEL��ʱ��ѡ��Ĵ���   PLLSEL--PLLʱ��ѡ��Ϊϵͳʱ��
    PLLCTL_PLLON=1;				//���໷��·���� PPLON�������໷   PPLCTLΪPLL���ƼĴ���
    
    /* SYNR = (VCOFRQ1 | VCOFRQ0 | SYNR5 | SYNR4 | //
                 SYNR3 | SYNR2 | SYNR1 | SYNR0 | //		)   */ 
  SYNR =0xc0 | 0x09;  // VCO_clock=2*osc_clock*(1+SYNR)/(1+REFDV)=80 MHz; ʱ�Ӻϳ����Ĵ���  SYNR�Ĵ������������໷�ı�Ƶϵ��
                        // VCOFRQ[1:0]=1:1,����VCO_clock��80~160 MHz֮��    ��ȷ��VCOCLK��Ƶ�ʷ�Χ
                            
    /* REFDV = (REFFRQ1 | REFFRQ1 | REFDV5 | REFDV4 | //
                 REFDV3 | REFDV2 | REFDV1 | REFDV0 | //
                )   */                                              
  REFDV=0x80 | 0x01;// VCO_clock=2*osc_clock*(1+SYNR)/(1+REFDV)=80 MHz;
 					// REF_clock=osc_clock/(REFDV+1)=16/(1+1)=8 MHz
 					// REFFRQ[1:0]=1:0,��Ϊ�ο�ʱ����6~12 MHz֮��

    POSTDIV=0x00; 				//VCO_clock = PLL_clock   POST��Ƶ�Ĵ�������VCOCLK��PLLCLK֮��ķ�Ƶϵ��
    
    _asm(nop);					//������ʱ���ȴ�ʱ��Ƶ���ȶ�
    _asm(nop);
    
    while(!(CRGFLG_LOCK==1))	//ʱ��Ƶ�����ȶ������໷Ƶ������  CRGFLGʱ��ϵͳ��־�Ĵ��� 
    {
      ;
    }
    
    CLKSEL_PLLSEL =1; 			//ʹ�����໷ʱ��
}

//CH_SA_FREQ =2M
//CH_SB_FREQ =2M

//�˴���ֱ������ֱ�ӵ��� 1 ��3ͨ��������ӵ���5ͨ��
/***********************************************************/
/***************��ʱ���ĵ��趨***************************/

void PIT_Init(void)// PIT��ʱ��ʹ�ܣ�����������Ƭ������Χ�豸��Ҳ�����������������ж�
{
    PITCE_PCE0 = 1; //PITͨ��ʹ�ܼĴ���  PCE0ʹ��            
    PITMUX_PMUX0 = 0; //PIT΢ʱ��ѡ��λ PMUX0ʹ��          
    PITMTLD0 = 79;   //PIT΢��ʱ�����ؼĴ��� PITMTLD0��1ʹ��         
    PITLD0 = 999;     //PIT���ؼĴ�����0~7��         
    PITINTE_PINTE0 = 1; //PIT�ж�ʹ�ܼĴ���        
    PITCFLMT_PITE = 1;  //PITE--PITģ��ʹ��λ       
}
/***********************************************************/

/******************AD��ʼ��***********************************/
void AD_Init(void) 
{
    //ATD0DIEN=0x00; ȫ��ʹ��ģ������
    ATD0CTL0 = 0x0F;   /* Set wrap around ADѭ���ɼ���AN15 */
    ATD0CTL1 = 0x4F;   /* 12-bit data  ȷ����ģת���ľ���Ϊ12λ������ǰ�ŵ�*/
    ATD0CTL2 = 0x40;   /* Fast Flag Clear ���������־λ����ֹ�ⲿ��������ֹ�ж�*/
    ATD0CTL3 = 0x88;   /* Right justified data,Conversion Sequence Length = 1 �Ҷ���ģʽÿ�����е�ת������Ϊ1*/
    ATD0CTL4 = 0x04;   /* Set sample time and prescaler��Ԥ��Ƶ�� ����ʱ���ATDʱ��������4�� ATDʱ��Ԥ��Ƶ��ʱ��Ƶ��ΪF=Fbus/2/5*/
}
/***********************************************************/
/*******************ʹ��CCD****************************************/
void CCD_IO_Init(void) //
{
    TSL1401_CLK_DDR = 1;//TSL1401_CLK_DDR     DDRA_DDRA0   CCD1���
    TSL1401_SI_DDR  = 1;//TSL1401_SI_DDR      DDRA_DDRA1
    TSL1401_CLK = 0;//TSL1401_CLK         PORTA_PA0
    TSL1401_SI  = 0;//TSL1401_SI          PORTA_PA1
}
/***********************************************************/
 /***************�����ۼ������趨***************************/
void PAC_Init(void)	            
{
     PACTL = 0x40;     				//�����ۼ���A����
     PACNT = 0x00;     				//�����ۼ���A������ֵ
}
void ECTInit(void) 
{    
    TIOS_IOS0 = 0; // channel 0 as output compare
    TIE_C0I = 1;  // ʹ�� channel 0 �ж�
    
    TCTL4_EDG0A = 1;
    TCTL4_EDG0B = 0;  // ���������
    
    TSCR1_TEN  = 1;  //timer enable    
}
/***********************************************************/
/***************�����ʼ�趨***************************/
void PWM_Init(void)
{
   
    PWMPRCLK = 0x11;           //Aʱ�Ӻ�Bʱ�ӵķ�Ƶϵ��Ϊ2,Ƶ��Ϊ40MHz
    PWMSCLA = 0x0A;	
    PWMSCLB = 0x0A;				//SA SBʱ��Ƶ��Ϊ2MHZ
    
    //PWMCTL_CON45 = 1;					// 16λ����PWM45
    //PWMCTL_CON67 = 1;					// 16λ����PWM67
    PWMCTL_CON01 = 1;					// 16λ����PWM01
    //PWMCLK_PCLK4 = 1;					// PWM4ʹ��SA
    //PWMCLK_PCLK6 = 1;					// PWM6ʹ��SB
    //PWMCLK_PCLK1 = 1;					// PWM1ʹ��SA
    PWMCLK=0xf2;
    
    PWMPER01 = (CH_SA_FREQ / STEER_FREQ);		// дPWM01�����ڼĴ���
    PWMPER4 = (CH_SB_FREQ / MOTOR_FREQ);		// дPWM45�����ڼĴ���  200*1/2M
    PWMPER5 = (CH_SB_FREQ / MOTOR_FREQ);
    PWMPER6 = (CH_SB_FREQ / MOTOR_FREQ);		// дPWM67�����ڼĴ���  200
    PWMPER7 = (CH_SB_FREQ / MOTOR_FREQ);
    PWMPOL_PPOL1 = 1;							// ����Ϊ��
    PWMPOL_PPOL4 = 1;							// ����Ϊ��
    PWMPOL_PPOL5 = 1;
    PWMPOL_PPOL6 = 1; 
    PWMPOL_PPOL7 = 1;
    PWMCAE = 0x00;								// �����
    PWME_PWME1 = 1;								// ʹ��PWM45 
} 
/***********************************************************/

void LED_Init(void) {
    LED1_DDR = 1;
    LED2_DDR = 1;
    LED1 = 1;
    LED2 = 1;
}













