/*================ COPYRIGHT 2010 (주)기원전자 기술연구소  =========================

        화  일  명 : iexec.c
	프로그래명 : LS SPL 컨트롤로.
	작 성 자   : 박 강 호
	도    구   :CORETEX-M3(STM32F103V16)

*****************************************************************************************************/
#define __IEXEC_H__

#include "stm32f10x_lib.h"
#include "main.h"

#include "settings.h"
#include "balance.h"

extern bool CANTXERR_FG;
extern u16  DMA_ADC_Value[];
extern void CAN_Ext_Tx_nByte(u32 ExtId,u8 *pBuff,u8 nByte);
extern void CAN_TRASMITTER_PROC(void);    // CAN 송신 관리 프로그램.
extern u16  AD_READ(u8 Channel);

extern void Delay_1ms(u16 Dly);

void          PRG_RUN_PROC(void);

#define     VAC_LAMP_ON_LOW        60   // 500mSec..  램프 점멸 주기 SLOW
#define     VAC_LAMP_ON_FAST       25   // 20mSec     램프 점멸 주기 SLOW
#define     VAC_LAMP_CYCLE        100   // 1sec       램프 점멸 사이클

#define     VAC_EEPROM_ADDR_1       0   // 주소

u16  EEPROM_DATA;

ushort Data1;

static u8  T_mSec = 0;
static u8  T_10mSec = 0;
static u8  T_100mSec = 0;
static u8  T_PRG_RUN = 0;

u8    FP_SETTING_LEVER_CHK = 1;         // 셋팅 모드 CHECK SET..
void  Variable_init(void);          // 변수 초기화 루틴..
 
//
//===== 변수 초기화 실행.. ==================================================
void  WATCH_DOG()
{
  static bool Watch_ON = On;
  if ( Watch_ON ) { Watch_ON = Off; PFO_WATCH_DOG = Off;}
  else            { Watch_ON = On;  PFO_WATCH_DOG = On; }
  return;
}
/*****************************************************************************************************/
/* Delay Function										     */												
/*****************************************************************************************************/
void Delay(u32 buf){
   u32 i;
  for(i=0; i< buf; i++);
}

//===== 타이머 실행.. ==================================================
void  TIMER_PROC()
{
  static u8  T_CAN_mSec = 0;

  //
  // 프로그램 전체가 1mSec간격으로 실행..
  //
    T_mSec++;
    T_PRG_RUN++;
    T_CAN_mSec++;
    FP_mSec = On;           // mSec Timer Flag  On

    if ( T_PRG_RUN >= VAC_PRG_RUN_TIME) {       // 2mSec 간격..
       FP_PRG_RUN = On;
       T_PRG_RUN = 0;
    }

   if ( T_CAN_mSec >= 2 )  { T_CAN_mSec=0; FP_CAN_Timer_2mSec = On; }

   if ( T_mSec >= 10 ) {
    T_mSec = 0;
    FP_10mSec = On;       // 10mSec Timer Flag on..


    T_10mSec++;

//  T_ROLLING_CAL++;
    T_ROLLING_LAMP++;  
    T_ROLLING_CHAGE++;
  
    if ( ROLLING_CHANGE < T_ROLLING_CHAGE )   
    {                            // 100mSec샘플링..
        T_ROLLING_CHAGE = 0;    // ROLLING SENSOR SAMPLING ...
        AD_ROLLING_SAMPLING= AD_ROLLING;
    }
    
    
    if( T_10mSec >= 10) {  // 100mSec Timer Flag on..
      FP_100mSec = On;
      T_10mSec = 0;
      T_100mSec++;

      
      if( T_100mSec >= 10) {  // Sec Timer Flag on..
        FP_Sec = On;
        T_100mSec = 0;
        
        FP_POWER_ON =  Off;		// 전원 투입후 1초 안에는 동작 하지 않음..
        FP_BAL_SETTING = Off;  //  전원 투입후 1초 안에는 셋팅 검출...ON

      }
    }
  }

}


void    CAN_Recive_PROC()
{
  
  static u8     T_Error =0;
  static s16   Temp;
    
     if ( T_Error >= 20 )   FP_ROLLING_H_ERR = On;
     else {  
         if ( FP_100mSec) T_Error++;
         FP_ROLLING_H_ERR =Off;
         
     }
  
      if ( ROLL_Recive_Flag == On) {
        T_Error =0;
         ROLLING_ANGLE   =  Roll_Recive_Data_1;
         ROLLING_ANGLE |= (Roll_Recive_Data_2 << 8 );
         ROLLING_ANGLE = ROLLING_ANGLE - 9000;
         
         if ( ROLLING_ANGLE >= 1270 ) ROLLING_ANGLE = 1270;     // 각도 -12.7~+12.7을 0~255로 변환..
         if ( ROLLING_ANGLE <= -1270 ) ROLLING_ANGLE = -1270;
         
         Temp= (ROLLING_ANGLE+ 1270)/10;
         
                 if (Temp <= 0 )        Temp =0;
         else if ( Temp >= 255 )    Temp = 255;
         
         AD_ROLLING = Temp;
         ADC_ROLLING = Temp;

    //
    // 수평 센서 보정 루틴..
    //
   if ( VA_SET_CENTER <= EE_BAL_SET_DATA )               VA_BAL_CALIBRATION =(s16 )ADC_ROLLING - ((s16 )EE_BAL_SET_DATA-(s16 )VA_SET_CENTER);
   else                                                                      VA_BAL_CALIBRATION =(s16 )ADC_ROLLING + ((s16 )VA_SET_CENTER-(s16 )EE_BAL_SET_DATA);
        
   if (VA_BAL_CALIBRATION <=0 )              AD_ROLLING = 0;
   else if (VA_BAL_CALIBRATION >=255 )    AD_ROLLING = 255;
   else                                                  AD_ROLLING = (u8)VA_BAL_CALIBRATION;
         
         PITCH_ANGLE   = Roll_Recive_Data_3;
         PITCH_ANGLE |= (Roll_Recive_Data_4<<8);
         PITCH_ANGLE = PITCH_ANGLE - 9000;
         if ( PITCH_ANGLE >= 1270 ) PITCH_ANGLE = 1270;     // 각도 -12.7~+12.7을 0~255로 변환..
         if ( PITCH_ANGLE <= -1270 ) PITCH_ANGLE = -1270;
         
         Temp= (PITCH_ANGLE+ 1270)/10;
                 if (Temp <= 0 )        Temp =0;
         else if ( Temp >= 255 )    Temp = 255;
         PITCH_ANGLE = Temp;
         
         YAW_ANGLE   = Roll_Recive_Data_5;
         YAW_ANGLE |= (Roll_Recive_Data_6<<8);
         YAW_ANGLE = (YAW_ANGLE/360) +127;      // 표시 범위를 127 +- 100 ( 27~137 )으로 제한..
         ROLL_Recive_Flag     = Off;  
      }
      
      if ( GYRO_Recive_Flag == On) {
         GYRO_X   = Gyro_Recive_Data_1;
         GYRO_X |= (Gyro_Recive_Data_2<<8);
         GYRO_X = GYRO_X - 20000;
         GYRO_X = (GYRO_X/50)+127;          //  127 +- 100 ( 27~137 )으로 변경 
         GYRO_Recive_Flag     = Off;             // DATA READ OK CHECK
       }
     
     if ( FP_ROLLING_H_ERR ) {  // 에러 발생시 처리..
       YAW_ANGLE =0;
       PITCH_ANGLE = 127;       // 수평으로 인식..
       AD_ROLLING = 127;        // 수평으로 인식..
       GYRO_X =0;
       
     }

  
}

//===== 디지탈 입력 ==================================================

void	Digital_InputProc(void)
{
 u16      Gport_A,  Gport_C;
 
static u8 SW_SEL=0;
static u8 SW_SENSTIVE=0;

static u8 SW_UP=0;
static u8 SW_DOWN=0;
static u8 SW_EX_UP=0;
static u8 SW_EX_DOWN=0;
 
static u8 SW_UPLIMIT=0;

//
// Before flag save..
//
FIO_SEL                 = FI_SEL;             // 과거 상태 저장.
FIO_SENSTIVE        = FI_SENSTIVE; 
FIO_BAL_UP            = FI_BAL_UP;
FIO_BAL_DOWN       = FI_BAL_DOWN;
FIO_EX_BAL_UP       = FI_EX_BAL_UP;
FIO_EX_BAL_DOWN  = FI_EX_BAL_DOWN;
FIO_UPLIMIT            = FI_UPLIMIT;


//
// VARIABLE SHIFT ..
// 
SW_SEL <<= 1;  
SW_SENSTIVE <<= 1;  
SW_UP	<<= 1;  
SW_DOWN	<<= 1;  
SW_EX_UP  <<= 1;  
SW_EX_DOWN<<= 1;  
SW_UPLIMIT<<= 1;  

  //
 // PORT READ..
 //
 Gport_A = GPIOA->IDR;
 Gport_C = GPIOC->IDR;

  if ( (Gport_A & BIT_0) != 0 )  SW_EX_DOWN++;
    
  if ( (Gport_C & BIT_1) == 0 )         SW_SENSTIVE++;
  if ( (Gport_C & BIT_2)  != 0 )         SW_UPLIMIT++;
  if ( (Gport_C & BIT_3 ) != 0 )         SW_EX_UP++;
  if ( (Gport_C & BIT_13)  == 0 )       SW_UP++;
  if ( (Gport_C & BIT_14) == 0 )        SW_DOWN++;
  if ( (Gport_C & BIT_15) == 0 )        SW_SEL++;
        
 
  if ( SW_SEL == 0XFF)	           FI_SEL          =  On;
  else                                          FI_SEL          = Off;
  if ( SW_SENSTIVE == 0XFF)	   FI_SENSTIVE = On;
  else                                          FI_SENSTIVE=  Off;
  if ( SW_UP   == 0XFF)	           FI_BAL_UP    =  On;
  else                                          FI_BAL_UP    = Off;
  if ( SW_DOWN == 0XFF)	           FI_BAL_DOWN = On;
  else                                          FI_BAL_DOWN= Off;
  if ( SW_EX_UP   == 0XFF)	           FI_EX_BAL_UP = On;
  else                                          FI_EX_BAL_UP= Off;
  
  if ( SW_EX_DOWN == 0XFF)	   FI_EX_BAL_DOWN = On;
 else                                           FI_EX_BAL_DOWN= Off;
  if ( SW_UPLIMIT == 0XFF)	           FI_UPLIMIT    =Off;  // 리미트 스위치 상태 반전 2021.04.01
 else                                           FI_UPLIMIT= On;
 
 
//
// 감도 제어 모드 선택 루틴..
//
            
  if ( FIO_SENSTIVE == Off )     {
            if ( FI_SENSTIVE )    MODE_SENSTIVE++;
  }

  if ( MODE_SENSTIVE >= 3 ) MODE_SENSTIVE =0;   // 수동모드와 자도오드 2가지만 되도록 수정.

//
// 자동 제어 모드 선택 루틴..
//
  MODE_AUTO_OLD = MODE_AUTO;
            
  if ( FIO_SEL ==Off )     {
            if ( FI_SEL )    MODE_AUTO++;
  }

  if ( MODE_AUTO >= 2 ) MODE_AUTO =0;


  if ( (MODE_SETTING_2 == 0) && (MODE_SETTING_1==0) && (MODE_SETTING_1==0) )  {     // 셋팅 모드 확인 루틴..

     switch ( MODE_SENSTIVE )    {
          case 0:    FOL_SENSTIVE  = On; break;
          case 1:    FOL_MIDDLE    = On; break;
          case 2:    FOL_SLOW      = On; break;
          default :  MODE_SENSTIVE = 0;  break;
     }  

    FM_MANUAL = FM_SLOPE = FM_FLAT = Off;
    switch ( MODE_AUTO )    {
          case 0:   FM_MANUAL = On;  FOL_MANUAL  = On;          break;
          case 1:   FM_SLOPE  = On;    FOL_FLAT  = On;               break;
          case 2:   FM_FLAT   = On;     FOL_SLOPE     = On;          break;
          default : MODE_AUTO  = 0;    FOL_MANUAL  = On;          break;
    }  
 }
 
 
}

#define   VAC_AD_CONVER                 1023
#define   VAC_AD_BIT_VOLTAGE            (float)(3.3/(float)4095)    // =3.3[V]/4095( 14BIT)
#define   VAC_AD_DEFAULT_RATE           (5/2)/VAC_AD_BIT_VOLTAGE    // 5V를 최대전압으로 해서 1023으로 환산 상수.

#define   VAC_AD_SENSOR_POWER_LOW       (4/2)/VAC_AD_BIT_VOLTAGE    // 4.0V이하일 때 AD값.
#define   VAC_AD_SENSOR_POWER_HIGH      (6/2)/VAC_AD_BIT_VOLTAGE    // 6.0V이상일 때 AD값.

u16 ADC_POWER;

//===== 아날로그 입력 ================================================

void	ANALOG_INPUT(void)
{
  static u8 ii;
  static u16 AD_TOTAL = 0;
 
  static u16 ADX_ROLLING =0;
  static u16 ADX_STROCK =0;
  static u16 ADX_BALANCE_SET =0;
  static u16 ADX_BATTERY =0;

  float   Rate;

  if ( VAC_AD_SENSOR_POWER_LOW >= ADC_POWER )      { 
    SENSOR_PWR_STATUR  = 1;  
    Rate = VAC_AD_DEFAULT_RATE; 
  }  // 센서 전압 4.5V이하.
  else if ( VAC_AD_SENSOR_POWER_HIGH <= ADC_POWER ){
    SENSOR_PWR_STATUR  = 2;  
    Rate = VAC_AD_DEFAULT_RATE; 
  } // 센서 전압 5.5V이상.
  else                                             {
    SENSOR_PWR_STATUR  = 0;  
    Rate = (float)ADC_POWER/(float)VAC_AD_CONVER; 
  }

//  AD_DATA_0  = AD_READ ( ADC_Channel_0);  //
  ADX_ROLLING            = AD_READ ( ADC_Channel_1);   // 수평 센서 
  ADX_STROCK            = AD_READ ( ADC_Channel_2);   // 
  ADX_BALANCE_SET   = AD_READ ( ADC_Channel_3);   //  
  ADC_VALVE_CURRENT            = AD_READ ( ADC_Channel_4);  //  
  ADX_BATTERY           = AD_READ ( ADC_Channel_5);  // 
  ADC_SENSOR           = AD_READ ( ADC_Channel_14);  // 
 
//
// AD 값을 온도로 환산 루틴..
//
  /*
  Volt = (float)ADC_TEMP*3.3/(float)4095;
  Temp = (((1.43-Volt)*1000)/4.3)+25;
  if ( Temp >= 95)  Temp = 95;                 //  온도 95도 까지 측정..
  if ( Temp <= -1)  { Temp *= -1; Temp+=100; }  // -온도 일때 +100
  Temperature = Temp;
*/

//
// 센서 전원 전압과 비율료 센서값을 계산함..
//
    ADX_ROLLING =       (u16)((float)ADX_ROLLING / Rate);         //  
    ADX_STROCK =       (u16)((float)ADX_STROCK / Rate);         //   
    ADX_BALANCE_SET =        (u16)((float)ADX_BALANCE_SET / Rate);  // 
    
    ADC_BATTERY =   ADX_BATTERY;
 
    if ( ADX_ROLLING >= VAC_AD_CONVER)                  ADX_ROLLING         = VAC_AD_CONVER;   //
    if ( ADX_STROCK >= VAC_AD_CONVER)                   ADX_STROCK         = VAC_AD_CONVER;   // 
    if ( ADX_BALANCE_SET >= VAC_AD_CONVER)          ADX_BALANCE_SET          = VAC_AD_CONVER;   // 

    
//  8비트 계산 ..
// 
//  ADC_ROLLING = ADX_ROLLING >> 2;
  ADC_STROCK = ADX_STROCK >> 2;
  AD_BALANCE_SET = ADX_BALANCE_SET >> 2;

    
    
 //
 // AD 5V 전원 전압 처리..
 //
  for (ii = 3 , AD_TOTAL = 0 ; ii >= 1 ; ii--)
  {
   	ADM_POWER[ii] = ADM_POWER[ii-1];
	AD_TOTAL     += ADM_POWER[ii];
  }
  ADM_POWER[0]    = ADC_SENSOR;
  AD_TOTAL       += ADM_POWER[0];
  AD_TOTAL       >>= 2;               // 1/4
  ADC_POWER = AD_TOTAL;

/*  
// 롤링 센서 이동평균..
  
  ADC_ROLLING = 1024 - ADC_ROLLING;
  for (ii = 3 , AD_TOTAL = 0x00 ; ii >= 1 ; ii--)
  {
   	ADM_ROLLING[ii] = ADM_ROLLING[ii-1];
	AD_TOTAL += ADM_ROLLING[ii];
  }
  ADM_ROLLING[0]  = ADX_ROLLING>>2;
  AD_TOTAL += ADM_ROLLING[0];
  AD_TOTAL >>= 2;               // 1/4
  if( AD_TOTAL > 255 )    AD_TOTAL = 255;
  AD_ROLLING = (u8)AD_TOTAL;

*/  


// ADC_STROCK = 255 - ADC_STROCK;	// 수평실린더 값 반전
   AD_STROCK = ADC_STROCK;
   //
   // 스트록 센서 보정..
   //        
        if ( (u8)VA_STROCK_CENTER <= EE_STROCK_SET_DATA )                 VA_BAL_CALIBRATION =(s16 )ADC_STROCK - ((s16 )EE_STROCK_SET_DATA-(s16 )VA_STROCK_CENTER);
        else                                                                                          VA_BAL_CALIBRATION =(s16 )ADC_STROCK + ((s16 )VA_STROCK_CENTER-(s16 )EE_STROCK_SET_DATA); 

        if (VA_BAL_CALIBRATION <=0 )             AD_STROCK = 0;
        else if (VA_BAL_CALIBRATION >=255 )  AD_STROCK = 255;
        else                                                AD_STROCK = (unsigned char)VA_BAL_CALIBRATION;
 
 
}

//===== 출력 변수 초기화 =============================================
void OUTDATA_CLEAR( bool ONOFF)
{
   FOL_MANUAL = FOL_FLAT= FOL_SLOPE = Off;
   FOL_SENSTIVE = FOL_MIDDLE= FOL_SLOW = Off;
   FOS_BAL_UP = FOS_BAL_DOWN =  Off;
  
}



//static TIM_OCInitTypeDef        TIM_OCInitStructure;
//
//  출력 제어 루틴..
//
void	OUT_PUT(void)
{

  static u8     OLD_POWER = 0;
  static u16    OUT_PORTA = 0;
  static u16    OUT_PORTB = 0;
  static u16    OUT_PORTD = 0;
//  static u16    OUT_PORTC =0;
  static u16    OUT_PORTC =0xFFFF;
 //
 // 제어 루틴..
 //
  /*
 if ( FP_POWER_ON ) {
    OLD_POWER= 1;
    OUTDATA_CLEAR(On);
  } 
  else if ( OLD_POWER== 1) {
    OUTDATA_CLEAR(Off);
    OLD_POWER = 0;
  }
 
 //
//	리미트제어
//
 if ( (FI_BAL_UP ==Off) && (FI_BAL_DOWN == Off) )    // 수동 조작시에는 리미트 제어 하지 않음.
 {
   if ( FOS_BAL_UP )
   {
       if ( ADC_STROCK > VA_STROCK_LIMIT_H)      FOS_BAL_UP = Off;
   }
   else if ( FOS_BAL_DOWN )
   {
       if ( ADC_STROCK < VA_STROCK_LIMIT_L )     FOS_BAL_DOWN = Off;
   }
  }

*/	
  OUT_PORTA = GPIOA->ODR;
  OUT_PORTB = GPIOB->ODR;
  OUT_PORTC = GPIOC->ODR;
  OUT_PORTD = GPIOD->ODR;
 
  OUT_PORTB |= BIT_5;   // EEPROM 항상 HIGH
  
  //
  OUT_PORTC  |=     BIT_11; 
  GPIOC->ODR  = OUT_PORTC;
  
  OUT_PORTC  &=  ~BIT_11; 
  GPIOC->ODR  = OUT_PORTC;
    
  OUT_PORTC  |=     BIT_12; 
  GPIOC->ODR  = OUT_PORTC;
  
  OUT_PORTC  &=  ~BIT_12; 
  GPIOC->ODR  = OUT_PORTC;
  
  OUT_PORTD  |=     BIT_2; 
  GPIOD->ODR  = OUT_PORTD;
  
  OUT_PORTD  &=  ~BIT_2; 
  GPIOD->ODR  = OUT_PORTD;

   
  //
  // PORT B OUTPUT 
  //

  if ( FOL_SENSTIVE )  OUT_PORTB  |=     BIT_12;
  else                         OUT_PORTB  &=  ~BIT_12; 
  if ( FOL_MIDDLE )      OUT_PORTB  |=     BIT_13;
  else                         OUT_PORTB  &=  ~BIT_13; 
  if ( FOL_SLOW )         OUT_PORTB  |=     BIT_14;
  else                         OUT_PORTB  &=  ~BIT_14; 

  if ( FOL_MANUAL )    OUT_PORTB  |=     BIT_15;
  else                         OUT_PORTB  &=  ~BIT_15; 

  if ( FOL_FLAT )          OUT_PORTC  |=     BIT_6; 
  else                         OUT_PORTC  &=  ~BIT_6; 
  if ( FOL_SLOPE )       OUT_PORTC  |=     BIT_7; 
  else                         OUT_PORTC  &=  ~BIT_7; 
  
  //
  // PORT D OUTPUT 
  //
  

  if ( FOS_BAL_UP )         OUT_PORTA |= BIT_6;
  else                            OUT_PORTA  &=  ~BIT_6; 
  if ( FOS_BAL_DOWN )    OUT_PORTA |= BIT_7;
  else                            OUT_PORTA  &=  ~BIT_7; 
/*
  if ( FOS_BAL_UP )         OUT_PORTA |= BIT_7;
  else                            OUT_PORTA  &=  ~BIT_7; 
  if ( FOS_BAL_DOWN )    OUT_PORTA |= BIT_6;
  else                            OUT_PORTA  &=  ~BIT_6; 
*/
  GPIOA->ODR =  OUT_PORTA;
  GPIOB->ODR = OUT_PORTB;
  GPIOC->ODR = OUT_PORTC;
   
  
/*
  if ( SET_CONTRAST_LED != SET_CONTRAST_LED_B ) {
    
    if ( SET_CONTRAST_LED > 1000 ) SET_CONTRAST_LED =1000;
    
    SET_CONTRAST_LED_B = SET_CONTRAST_LED;
    TIM_OCInitStructure.TIM_OCMode =      TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;

    TIM_OCInitStructure.TIM_Pulse = SET_CONTRAST_LED;   // LED 밝기 
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OC1Init(TIM3, &TIM_OCInitStructure);
    TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);

    TIM_ARRPreloadConfig(TIM3, ENABLE);
  }
    
    
  if ( SET_CONTRAST_BACK != SET_CONTRAST_BACK_B ) {

    if ( SET_CONTRAST_BACK > 1000 ) SET_CONTRAST_BACK =1000;

    SET_CONTRAST_BACK_B = SET_CONTRAST_BACK;
    
    TIM_OCInitStructure.TIM_OCMode =      TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;

    TIM_OCInitStructure.TIM_Pulse =  1000 - SET_CONTRAST_BACK;   // LED 백라이트 밝기 
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OC2Init(TIM3, &TIM_OCInitStructure);
    TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);

    TIM_ARRPreloadConfig(TIM3, ENABLE);
    TIM_Cmd(TIM3, ENABLE); //TIM4 enable counter  
  } */
//
// 포트 출력.
//
//   GPIOA->ODR  = OUT_PORTA;
//   GPIOC->ODR  = OUT_PORTC;

    
}


  u8 iexecctrl=2;
 
void  Variable_init(void) // 변수 초기화 루틴..
{
  u8 ii;
  
  FP_POWER_ON = On;
  FP_PRG_RUN = Off;
  FP_mSec    = Off;       //   mSec Timer Flag off
  FP_10mSec  = Off;       // 10mSec Timer Flag on..
  FP_100mSec = Off;       // 100mSec Timer Flag on..
  FP_Sec     = Off;       //     Sec Timer Flag on..

  FP_POWER_ON = On;
 
  
  //T_INIT = 0;
  T_mSec = T_10mSec = 0;
  T_100mSec = 0;
  T_PRG_RUN = 0;
  T_POWER =0;
  
  FP_CAN_ERROR = Off;
   

//
// EEPROM 처리 루틴..
//
  FP_BAL_SETTING =  On;          // 수평제어 셋팅 모드 CHECK SET

  FP_BAL_PALL_RUN = Off;
  MODE_SENSTIVE = 1;            // 수평 감도 표준모드..
  FP_ROLLING_H_ERR= Off;             // 저전압 동작 OFF FLAG CLEAR
  FP_ROLLING_L_ERR= Off;             // 저전압 동작 OFF FLAG CLEAR
  FP_STROCK_H_ERR= Off;             // 저전압 동작 OFF FLAG CLEAR
  FP_STROCK_L_ERR= Off;             // 저전압 동작 OFF FLAG CLEAR

/*  
  EE_BAL_SET_DATA      =  EEPROM_READ(VAE_ADD_ROLLING);
  for(ii = 0; ii < 100; ii++){          // Delay
    WATCH_DOG();
    Delay_1ms(1);
  }
  EE_STROCK_SET_DATA = EEPROM_READ(VAE_ADD_STROCK);

  for(ii = 0; ii < 100; ii++){          // Delay
    WATCH_DOG();
    Delay_1ms(1);
  }
  
  EEPROM_OPTION_MANUAL = EEPROM_READ(VAE_ADD_OPTION);

  for(ii = 0; ii < 100; ii++){          // Delay
    WATCH_DOG();
    Delay_1ms(1);
  }
  
  if ( (EE_STROCK_SET_DATA > (u8)VA_STROCK_SETTING_MAX) || (EE_STROCK_SET_DATA < (u8)VA_STROCK_SETTING_MIN) )
  {
     EE_STROCK_SET_DATA = (u8)VA_STROCK_CENTER;//  EEPROM에 저장이 되어 있지 않으면 원래값으로 지정..
     WATCH_DOG();
     EEPROM_WRITE(VAE_ADD_STROCK,EE_STROCK_SET_DATA);  // EEPROM 저장
     for(ii = 0; ii < 100; ii++){          // Delay
        WATCH_DOG();
        Delay_1ms(1);
      }
  }

  if ( (EE_BAL_SET_DATA > (u8)VA_ROLLING_SETTING_MAX) || (EE_BAL_SET_DATA < (u8)VA_ROLLING_SETTING_MIN) )
  {
     EE_BAL_SET_DATA    = VA_SET_CENTER;   //  EEPROM에 저장이 되어 있지 않으면 원래값으로 지정..
     WATCH_DOG();
     EEPROM_WRITE(VAE_ADD_ROLLING,EE_BAL_SET_DATA);     // EEPROM 저장
     for(ii = 0; ii < 100; ii++){          // Delay
        WATCH_DOG();
        Delay_1ms(1);
      }
  }

 if ( (EEPROM_OPTION_MANUAL > VA_OPTION_MANUAL_ON) || (EEPROM_OPTION_MANUAL < VA_OPTION_MANUAL_OFF) )      // 수동 모드 보정 여부 판정..
 {
     EEPROM_OPTION_MANUAL = VA_OPTION_MANUAL_OFF;       // 보정 OFF..
     WATCH_DOG();
     EEPROM_WRITE(VAE_ADD_OPTION, EEPROM_OPTION_MANUAL);  // EEPROM 저장
     for(ii = 0; ii < 100; ii++){          // Delay
      WATCH_DOG();
      Delay_1ms(1);
    }
 }
*/
  
//
// 입/출력 초기화..
//
   
  OUTDATA_CLEAR(Off);
  OUT_PUT();
  OUT_PUT();
  WATCH_DOG();             // Watch Dog Run...

  for ( ii=0; ii<=16; ii++)
  {
     Digital_InputProc();  // 입력 처리 루틴..
     ANALOG_INPUT();
  } 
 BALANCE_INIT();
 EEPROM_Init (0);   // EEPROM READ...
 AUTO_MANUAL(1);        // 수평제어 수동 센터값으로 보정 출력..

}

//
// 실행 루틴..
//
void iexec_control_proc(void) {
  switch(iexecctrl) {
   case 0:	
       if(tm0fg){
        tm0fg = 0;
        }
        iexecctrl++;
         break;					

   case 1:
     if(tm1fg){
        tm1fg = 0;
        timer_setup(1,100);        // 10mSec 간격으로  CAN DATA 발송..2011.02.17. 컴퓨터 반응속도가 느려서 10mSec->30mSec수정..
        //
        // CAN DATA발송 A
//        CAN_TRASMITTER_PROC();
       }
      iexecctrl++;
      break;

    case 2:
      if(tm2fg) {

        tm2fg = 0;
        timer_setup(2,1);        // 10mSec 간격으로  프로그램 실행..
        TIMER_PROC();
        WATCH_DOG();             // Watch Dog Run...
//      IWDG->KR  = 0xAAAA;                           // reload the watchdog
         IWDG_ReloadCounter();    // SOFT WARE WATCH DOG CLEAR...

        if ( FP_PRG_RUN == On)  {

          OUTDATA_CLEAR(Off);
          ANALOG_INPUT();
          Digital_InputProc();  // 입력 처리 루틴..
          CAN_Recive_PROC();


         BAL_SETTING_1();                 // 수평/변위 센서 보정 루틴
      BAL_SETTING_2();                 // 옵션/초기값 설정 루틴
         BAL_CHECK();                     // 센서/스위치 체크 
         AUTO_BALANCE();
  
   
          ERROR_CHECK();

          OUT_PUT();        // 출력 처리 루틴..
          
          Setting_Proc();               // 셋팅 관련 처리 루틴..
          
          FP_PRG_RUN = Off;
          FP_mSec    = Off;       //   mSec Timer Flag off
          FP_10mSec  = Off;       // 10mSec Timer Flag on..
          FP_100mSec = Off;       // 100mSec Timer Flag on..
          FP_Sec     = Off;       //     Sec Timer Flag on..
        }
      }
      iexecctrl = 0;
      break;

    default: 	
           iexecctrl =0;	
           timer_setup(0,5);
           cnt=0;
           break;
  }
}
/************************************** COPYRIGHT 2011 (주)기원전자 기술연구소*************************************/
