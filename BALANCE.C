/*================ COPYRIGHT 2020 (주)기원전자 기술연구소  =========================

        화  일  명 : BALANCE.C
	프로그래명 : 수평제어 
	작 성 자   : 박 강 호
	도    구   :CORETEX-M3(STM32F103V16)
                    EWARM Compiler 5.4Vre

    2015.04.14. 컴파일러 chp set 변경 STM32F10XX8로 

*****************************************************************************************************/

//#define CAN_GLOBALS
//#include "INCLUDES.H"
//#include "stm32f10x_lib.h"
//#include "CAN_APP.H"


#include "main.h"

#define  __BALANCE_H__
#include "balance.h"
#include "settings.h"

extern void Delay_1ms(u16 Dly);
//#include <stdio.h>
//#include <string.h>
//#include <stdlib.h>
//#include "iexec.h"

void    BALANCE_INIT(void)
{
  u8 ii;
  
  FP_BAL_SETTING = On;          // 수평제어 셋팅 모드 CHECK SET
 
  FP_BAL_PALL_RUN = Off;
  MODE_SENSTIVE = On;            // 수평 감도 표준모드..
  FP_ROLLING_H_ERR= Off;             // 저전압 동작 OFF FLAG CLEAR
  FP_ROLLING_L_ERR= Off;             // 저전압 동작 OFF FLAG CLEAR
  FP_STROCK_H_ERR= Off;             // 저전압 동작 OFF FLAG CLEAR
  FP_STROCK_L_ERR= Off;             // 저전압 동작 OFF FLAG CLEAR
  
/*  
  Flash_FULL_Auto( VAC_AUTO_Save, VAC_ID_ADD_ROLLING,0);
  for(ii = 0; ii < 100; ii++){          // Delay
          WATCH_DOG();
          Delay_1ms(1);
  }
  Flash_FULL_Auto( VAC_AUTO_Save, VAC_ID_ADD_STROCK,0);

//  EE_STROCK_SET_DATA = (u8 )VA_STROCK_CENTER;//  EEPROM에 저장이 되어 있지 않으면 원래값으로 지정..
//  EE_BAL_SET_DATA    = VA_SET_CENTER;   //  EEPROM에 저장이 되어 있지 않으면 원래값으로 지정..
*/
  EEPROM_OPTION_MANUAL = VA_OPTION_MANUAL_OFF;       // 보정 OFF..
  
}
//
// 자동 수동 제어..
//
void    AUTO_MANUAL(u8 Init)
{
 static u8  AD_STROCK_BEFORE = 0;
// static u8  LEAK_ON = 0;
 u8 Chk;
 
 if ( Init== 1) { 
   AD_STROCK_BEFORE = AD_STROCK ;   // 초기 센터값..
//   LEAK_ON =0;
   return;
 }

/* if ( LEAD_ON == 0 )            // 수동 조작 후 부터 LEAD 보상 하도록 수정..
 {
    AD_STROCK_BEFORE = AD_STROCK ;   // 초기 센터값..
    return; 
 }*/
 
  Chk = 0;
  if (FI_BAL_UP)  {
        Chk = 1;
        FOS_BAL_UP    = On;
        FOS_BAL_DOWN  = Off;
   }
  else if (FI_BAL_DOWN)  {
        FOS_BAL_UP    = Off;
        FOS_BAL_DOWN  = On;
        Chk = 1;
  }
  else if (FI_EX_BAL_UP)   {
        FOS_BAL_UP    = On;
        FOS_BAL_DOWN  = Off;
        Chk = 1;
   }
  else if (FI_EX_BAL_DOWN)   {
        FOS_BAL_UP    = Off;
        FOS_BAL_DOWN    = On;
        Chk = 1;
   }

 
  if ( Chk == 1 ) {             // 수동 조작시 종료..
    FP_BAL_PALL_RUN = Off; 
    AD_STROCK_BEFORE = AD_STROCK;
    return;
  }

  if ( EEPROM_OPTION_MANUAL == VA_OPTION_MANUAL_OFF )   return; // 보정 루틴 없으면 종료..
  if ( (FP_STROCK_H_ERR==1) || (FP_STROCK_L_ERR==1) )   return; // 스트록 센서 에러면 종료..
  //
  // 보정 루틴..( 누유로 인해서 빠지면... ) 
  //
  
//  if( AD_STROCK_BEFORE+2 < AD_STROCK )        FOS_BAL_DOWN = 1;
    if ( AD_STROCK_BEFORE-2 > AD_STROCK)   FOS_BAL_UP = On; // 상승 모상은 하지 안도록 수정..
}

//
// 수평 경사각 및 실린더 센서
//
 void    BAL_SETTING_1()
{
 static u8  T_SETTING = 0;
 static u8  T_LAMP = 0;
 static u8  T_FLASH = 0;
 
 //static u8  SETTING_SENSOR = 0;
 u8 Chk;
 u8 ii;
 
 if (  (MODE_SETTING_1 == 0) && (MODE_SETTING_2 == 0) ) {
  if (  (FI_SENSTIVE==0) && (FI_SEL==1) )  {           // 수평 선택 off & 감도 ON 5초 이상..이면 셋팅 모드로 진입..
    if ( FP_100mSec ) T_SETTING++;
    if ( T_SETTING >= 50 ) { 
     MODE_SETTING_1  = VAC_SETTING_MODE;
     MODE_SETTING_1  |= VAC_SETTING_SENSOR_MODE;
     T_SETTING = 0;
    }
  }
  else T_SETTING = 0;
  return;
 } 

 if( (MODE_SETTING_2 != 0)  || (MODE_CHECK!= 0 )  ) {
   MODE_SETTING_1 = 0;   
   return;
 }

 if ( ((MODE_SETTING_1 & VAC_SETTING_NG)==0) && ((MODE_SETTING_1 & VAC_SETTING_OK)==0) ) {
  
   Chk = 0;
   if ( (ADC_STROCK  < (u8 )VA_STROCK_SETTING_MIN)  ||  (ADC_STROCK  > (u8 )VA_STROCK_SETTING_MAX) )    Chk = 1;    // 셋팅 범위에 벗어났으면 ..1.4 ~ 3.4
   if ( (ADC_ROLLING < (u8 )VA_ROLLING_SETTING_MIN) ||  (ADC_ROLLING > (u8 )VA_ROLLING_SETTING_MAX) )   Chk = 1;    // 셋팅 범위에 벗어났으면 ..2.0 ~ 3.0
      
   if (  Chk ==1 )  MODE_SETTING_1 |= VAC_SETTING_NG;
   else  {
        EE_STROCK_SET_DATA = ADC_STROCK;    //  EEPROM에 저장이 되어 있지 않으면 원래값으로 지정..
        EE_BAL_SET_DATA    = ADC_ROLLING;   //  EEPROM에 저장이 되어 있지 않으면 원래값으로 지정..        
        
        WATCH_DOG();
        Flash_FULL_Auto( VAC_VALUE_Save, VAC_ID_ADD_ROLLING,EE_BAL_SET_DATA);
        for(ii = 0; ii < 100; ii++){          // Delay
          WATCH_DOG();
          Delay_1ms(1);
        }
        
        Flash_FULL_Auto( VAC_VALUE_Save, VAC_ID_ADD_STROCK,EE_STROCK_SET_DATA);
        for(ii = 0; ii < 100; ii++){          // Delay
          WATCH_DOG();
          Delay_1ms(1);
        }
        MODE_SETTING_1 |= VAC_SETTING_OK; // 설정 OK
   }
   T_LAMP = 0;
 }  
 
 //
 // 셋팅 완료 되었으면..
 //
 if( MODE_SETTING_1 & VAC_SETTING_OK )
 {
   if ( FP_100mSec )   {
      T_LAMP++;             //  3초 동안 모든 램프 점등 ..
      T_FLASH++;
   }
   if ( T_LAMP >= 30 )    MODE_SETTING_1 = 0;     //  셋팅 모드 해제..
   if ( T_FLASH >= 8 )    T_FLASH = 0;     //  셋팅 모드 해제..

   FOL_SENSTIVE = Off;
   FOL_MIDDLE = Off;
   FOL_SLOW = Off;       // 
   if ( T_FLASH <= 4 )   {
    FOL_MANUAL = On;
    FOL_SLOPE = On;
    FOL_FLAT = On;
   }
   else  {
    FOL_MANUAL = Off;
    FOL_SLOPE = Off;
    FOL_FLAT = Off;
   }
   return;
 } 

 //
 // 셋팅 NG 되었으면..
 //
 if( MODE_SETTING_1 & VAC_SETTING_NG )
 {
   if ( FP_100mSec )      T_LAMP++;             //  3초 동안 모든 램프 소등 ..
   if ( T_LAMP >= 30 )    MODE_SETTING_1 = 0;     //  셋팅 모드 해제..

   FOL_MANUAL = Off;
   FOL_SLOPE = Off;
   FOL_FLAT = Off;
   FOL_SENSTIVE = Off;
   FOL_MIDDLE = Off;
   FOL_SLOW = Off;
   return;
 } 


} 

//
// 수평 경사각 및 실린더 센서
//
 void    BAL_SETTING_2()
{

 static u8  T_SETTING = 0;
 static u8  T_LAMP = 0;
 static u8  T_FLASH = 0;
 static u8  T_SETTING_OVER = 0;
 static u8  Chk;
 
 u8 ii;
 //static u8  SETTING_SENSOR = 0;
 
 if ( (MODE_SETTING_1 == 0) && (MODE_SETTING_2 == 0) ) {
   if (  (FI_SEL==1) && (FI_SENSTIVE==1) )  {    // 수평자동 & 감도 동시 ON 5초 이상..이면 셋팅 모드로 진입..
     if ( FP_100mSec ) T_SETTING++;
     if ( T_SETTING >= VAC_SETTING_TIME ) { 
      T_SETTING_OVER = 0;                         // 셋팅 over 타임 clear...
      MODE_SETTING_2  = VAC_SETTING_MODE;
      MODE_SETTING_2  |= VAC_SETTING_OPTION_MODE;
     }
   }
   else T_SETTING = 0;
   return;
 }  

 if( (MODE_SETTING_1 != 0)  || (MODE_CHECK!= 0 )  ) {
   MODE_SETTING_2 = 0;   
   return;
 }
 if( MODE_SETTING_2 == 0 )   return;
 
 
// 
// 셋팅 센서 선택..
//
T_SETTING = 0;

if ( FP_Sec )                T_SETTING_OVER++;

if ( T_SETTING_OVER >= 30 )  MODE_SETTING_2 = 0;          // 30초 이상 셋팅 하지 않으면 해제..

if (  (FIO_SEL==0 ) && (FI_SEL ==1) )    {
   T_SETTING_OVER = 0;
   if ( MODE_SETTING_2 & VAC_SETTING_OPTION_MODE ) {
     MODE_SETTING_2 &= 0xF1;
     MODE_SETTING_2 |= VAC_SETTING_DEFAULT_MODE;     // 옵션 설정모드.. 
   } 
   else  {
     MODE_SETTING_2 &= 0xF1;
     MODE_SETTING_2 |= VAC_SETTING_OPTION_MODE;     // default 설정모드.. 
   } 
}

//
// 설정 버튼 눌렸을 경우..
//
 if ( (FIO_SENSTIVE==0) && (FI_SENSTIVE==1) ) {
    T_SETTING_OVER = 0;
   //
   // 센서 셋팅..
   //
/*    if ( MODE_SETTING_2 & VAC_SETTING_SENSOR_MODE ) {
      Chk = 0;
      if ( (ADC_STROCK  < (u8 )VA_STROCK_SETTING_MIN)  ||  (ADC_STROCK  > (u8 )VA_STROCK_SETTING_MAX) )    Chk = 1;    // 셋팅 범위에 벗어났으면 ..1.4 ~ 3.4
      if ( (ADC_ROLLING < (u8 )VA_ROLLING_SETTING_MIN) ||  (ADC_ROLLING > (u8 )VA_ROLLING_SETTING_MAX) )   Chk = 1;    // 셋팅 범위에 벗어났으면 ..2.0 ~ 3.0
      
      if (  Chk ==1 )  MODE_SETTING_2 |= VAC_SETTING_NG;
      else  {
        EE_STROCK_SET_DATA = ADC_STROCK;    //  EEPROM에 저장이 되어 있지 않으면 원래값으로 지정..
        EE_BAL_SET_DATA    = ADC_ROLLING;   //  EEPROM에 저장이 되어 있지 않으면 원래값으로 지정..        
        WATCH_DOG();
        EEPROM_WRITE(VAE_ADD_ROLLING,EE_BAL_SET_DATA);     // EEPROM 저장
        WATCH_DOG();
        EEPROM_WRITE(VAE_ADD_STROCK, EE_STROCK_SET_DATA);  // EEPROM 저장
        WATCH_DOG();
        MODE_SETTING_2 |= VAC_SETTING_OK; // 설정 OK
      }
      T_LAMP = 0;
   }  */
   //
   // 옵션 셋팅..
   //    
   if (MODE_SETTING_2 & VAC_SETTING_OPTION_MODE ) {   
   
     if ( EEPROM_OPTION_MANUAL==VA_OPTION_MANUAL_OFF )   EEPROM_OPTION_MANUAL = VA_OPTION_MANUAL_ON;
     else                                                                           EEPROM_OPTION_MANUAL = VA_OPTION_MANUAL_OFF;
     WATCH_DOG();
      Flash_FULL_Auto( VAC_VALUE_Save, VAC_ID_ADD_OPTION,EEPROM_OPTION_MANUAL);
//   EEPROM_WRITE(VAE_ADD_OPTION, EEPROM_OPTION_MANUAL);  // EEPROM 저장      옵션 저장..
     for(ii = 0; ii < 100; ii++){          // Delay
        WATCH_DOG();
        Delay_1ms(1);
      }
     MODE_SETTING_2 |= VAC_SETTING_OK; // 설정 OK
     T_LAMP = 0;
//     AUTO_MANUAL(1);     // 옵션 셋팅하면 현재값을 기준값으로 저장..
   }
   //
   // DEFAULT 셋팅..
   //
   else {
        EE_STROCK_SET_DATA = (u8)(VA_STROCK_CENTER);         //  EEPROM에 DEFAULT 127
        EE_BAL_SET_DATA    = VA_SET_CENTER;            //  EEPROM에 DEFAULT 2.4V
        EEPROM_OPTION_MANUAL = VA_OPTION_MANUAL_OFF;    //  EEPROM DEFAULT OFF
        
        WATCH_DOG();
        Flash_FULL_Auto( VAC_DEFAULT_Save, VAC_ID_ADD_ROLLING,EE_BAL_SET_DATA);
        for(ii = 0; ii < 100; ii++){  WATCH_DOG();     Delay_1ms(1);     }      // DELAY 

        Flash_FULL_Auto( VAC_DEFAULT_Save, VAC_ID_ADD_STROCK,EE_STROCK_SET_DATA);
        for(ii = 0; ii < 100; ii++){  WATCH_DOG();     Delay_1ms(1);     }      // DELAY 

        //     EEPROM_WRITE(VAE_ADD_OPTION, EEPROM_OPTION_MANUAL);  // EEPROM 저장      옵션 저장..
        Flash_FULL_Auto( VAC_DEFAULT_Save, VAC_ID_ADD_MODE_SENSTIVE,0);
        for(ii = 0; ii < 100; ii++){  WATCH_DOG();     Delay_1ms(1);     }
        Flash_FULL_Auto( VAC_DEFAULT_Save, VAC_ID_ADD_MODE_STANDARD,0);
        for(ii = 0; ii < 100; ii++){  WATCH_DOG();     Delay_1ms(1);     }
        Flash_FULL_Auto( VAC_DEFAULT_Save, VAC_ID_ADD_MODE_FAST,0);
        for(ii = 0; ii < 100; ii++){  WATCH_DOG();     Delay_1ms(1);     }

        Flash_FULL_Auto( VAC_DEFAULT_Save, VAC_ID_ADD_MODE_SENSTIVE_TIME,0);
        for(ii = 0; ii < 100; ii++){  WATCH_DOG();     Delay_1ms(1);     }
        Flash_FULL_Auto( VAC_DEFAULT_Save, VAC_ID_ADD_MODE_STANDARD_TIME,0);
        for(ii = 0; ii < 100; ii++){  WATCH_DOG();     Delay_1ms(1);     }
       Flash_FULL_Auto( VAC_DEFAULT_Save, VAC_ID_ADD_MODE_SLOW_TIME,0);
        
//        Flash_FULL_Auto( VAC_DEFAULT_Save, VAC_ID_ADD_OPTION,EEPROM_OPTION_MANUAL);
        
        
/*        
        for(ii = 0; ii < 100; ii++){          // Delay
          
          
          WATCH_DOG();
          Delay_1ms(1);
        }*/
        MODE_SETTING_2 |= VAC_SETTING_OK; // 설정 OK
        T_LAMP = 0;
   }
  }


 //
 // 셋팅 완료 되었으면..
 //
 if( MODE_SETTING_2 & VAC_SETTING_OK )
 {
   if ( FP_100mSec )  {  
     T_LAMP++;             //  3초 동안 모든 램프 점등 ..
     T_FLASH++;
   }
   if ( T_LAMP >= 30 )    MODE_SETTING_2 = 0;     //  셋팅 모드 해제..
   if ( T_FLASH >= 8 )    T_FLASH = 0;     //  셋팅 모드 해제..

   if ( T_FLASH <=4 ) {
    FOL_MANUAL = On;
    FOL_SLOPE = On;
    FOL_FLAT = On;
   
    FOL_MIDDLE = On;
    FOL_SLOW = On;
   }
   else {
    FOL_MANUAL = Off;
    FOL_SLOPE = Off;
    FOL_FLAT = Off;
   
    FOL_MIDDLE = Off;
    FOL_SLOW = Off;
   }
   
   if ( EEPROM_OPTION_MANUAL == VA_OPTION_MANUAL_ON ) FOL_SENSTIVE = On;       // 
   else                                               FOL_SENSTIVE = Off;
   AUTO_MANUAL(1);
   
   return;
 } 
/*
 //
 // 셋팅 NG 되었으면..
 //
 if( MODE_SETTING_2 & VAC_SETTING_NG )
 {
   if ( FP_100mSec )      T_LAMP++;             //  3초 동안 모든 램프 소등 ..
   if ( T_LAMP >= 30 )    MODE_SETTING_2 = 0;     //  셋팅 모드 해제..

   FOL_MANUAL = 0;
   FOL_SLOPE = 0;
   FOL_FLAT = 0;
   FOL_SENSTIVE = 0;
   FOL_MIDDLE = 0;
   if ( EEPROM_OPTION_MANUAL == VA_OPTION_MANUAL_ON ) FOL_MANUAL = 1;       // 
   else                                               FOL_MANUAL = 0;
   
   return;
 } 
*/
 if ( FP_100mSec )      T_LAMP++;       // 램프 점멸 0.4초 ON, 0.4초 OFF
 if ( T_LAMP >= 8 )     T_LAMP = 0;     

 if ( T_LAMP <= 4 )     {       // ON타임..
 
//       if ( MODE_SETTING_2 & VAC_SETTING_SENSOR_MODE )  FOL_MANUAL = 1;
         if ( MODE_SETTING_2 & VAC_SETTING_OPTION_MODE )  FOL_MANUAL= On;
    else if ( MODE_SETTING_2 & VAC_SETTING_DEFAULT_MODE ) FOL_FLAT = On;
    FOL_SENSTIVE = On;
    FOL_MIDDLE = On;
    FOL_SLOW= On;
 } 
 else {
    FOL_MANUAL = Off;
    FOL_SLOPE =Off;
    FOL_FLAT = Off;
    
    FOL_MIDDLE = Off;
    FOL_SLOW= Off;
 }

 if ( EEPROM_OPTION_MANUAL == VA_OPTION_MANUAL_ON ) FOL_SENSTIVE = On;       // 
 else                                               FOL_SENSTIVE = Off;
}

//
// 체크 루틴..
//
 void    BAL_CHECK()
{
 static u8  T_SETTING = 0;
 //static u8  T_LAMP = 0;
 static u8  T_FLASH = 0;
 
 //static u8  SETTING_SENSOR = 0;

if (  (MODE_SETTING_1 == 0) && (MODE_SETTING_2 == 0) && (MODE_CHECK==0) ) {
  if (  (FI_SENSTIVE==1) && (FI_SEL==0) )  {           // 수평 선택 off & 감도 ON 5초 이상..이면 셋팅 모드로 진입..
    if ( FP_100mSec ) T_SETTING++;
    if ( T_SETTING >= 50 ) { 
     MODE_CHECK  = VAC_SETTING_MODE;
     T_SETTING = 0;
    }
  }
  else T_SETTING = 0;
  return;
 } 

 if( (MODE_SETTING_1 != 0) || (MODE_SETTING_2 != 0 ) ) {
  MODE_CHECK = 0;
  return;
 }

 if( MODE_CHECK     == 0 )   return;

 if ( FI_SEL ) MODE_CHECK = 0;
     
 if ( (ADC_STROCK >= (u8 )VA_STROCK_CENTER-10)  && (ADC_STROCK <= (u8 )VA_STROCK_CENTER+10) )    FOL_MANUAL = On;       // 변위센서 셋팅범위 벗어남..
 else                                                                                              FOL_MANUAL = Off; 
 if ( (ADC_ROLLING >= (u8 )VA_SET_CENTER-10)    && (ADC_ROLLING <= (u8 )VA_SET_CENTER+10) )        FOL_FLAT = On;       // 변위센서 셋팅범위 벗어남..
 else                                                                                              FOL_FLAT = Off; 
 if ( (AD_BALANCE_SET >= (u8 )VA_SET_CENTER-VA_EFFECTIVE_RANGE) && (AD_BALANCE_SET <= (u8 )VA_SET_CENTER+VA_EFFECTIVE_RANGE) )     FOL_SLOPE = On;       // 변위센서 셋팅범위 벗어남..
 else                                                                                              FOL_SLOPE = Off; 
 
 if ( FI_BAL_UP )        { FOL_SENSTIVE = On; FOL_MIDDLE = On; FOL_SLOW = Off; } 
 else if ( FI_BAL_DOWN ) { FOL_SENSTIVE = Off; FOL_MIDDLE = On; FOL_SLOW = On; } 
 else                    { FOL_SENSTIVE = Off; FOL_MIDDLE = Off; FOL_SLOW = Off; }        

 if ( FP_100mSec )     T_FLASH++;
 if ( T_FLASH >= 8 )    T_FLASH = 0;     //  셋팅 모드 해제..

 if ( T_FLASH <= 4 )   {
    FOL_MANUAL = On;
    FOL_SLOPE = On;
    FOL_FLAT = On;
 }
//
// 셋팅 모드 일 경우..
//
FM_MANUAL = FM_SLOPE = FM_FLAT = Off;
MODE_SENSTIVE = 1;        // 셋팅중 표준모드..
MODE_AUTO = 0;            // 수동 모드..
return;
}


//
//
//
void	ERROR_CHECK()
{
  static u8   T_LAMP = 0;
  //static u8   T_BATTERY = 0;
  static u8   T_STROCK_ERR = 0;
//  static u8   T_ROLLING_ERR =0;
  
  u8            BUF1,BUF2;

  
  /*
  if ( ADC_BATTERY <= VAC_BATTERY_10V ) {
   
    if ( FP_BATTERY_ERR == 0 ) {
      if ( FP_100mSec )           T_BATTERY++;
      if ( T_BATTERY >= 30 )      FP_BATTERY_ERR = On;   // 3.0초 이상 10v이하면출력 제어 금지..
    }
  }
  else {
   FP_BATTERY_ERR = Off;
   T_BATTERY = 0;
  }*/
  
  //
  // 스트록 센서 에러 전압 검출.
  //
 if ( ADC_STROCK > VA_STROCK_ERR_H ) { 	// 높은 전압 체크

   if  ( FP_10mSec )     T_STROCK_ERR++;  
   if ( T_STROCK_ERR >= VA_STROCK_ERR_TIME )	{
		FP_STROCK_H_ERR = On;
		T_STROCK_ERR = 0;
	}
 } 
 else  if ( ADC_STROCK < VA_STROCK_ERR_L ) 	{ // 낮은 전압 체크
   if  ( FP_10mSec )     T_STROCK_ERR++;  
   if ( T_STROCK_ERR >= VA_STROCK_ERR_TIME )	{
		FP_STROCK_L_ERR = On;
		T_STROCK_ERR = 0;
	}
 }
 else   {

 	FP_STROCK_L_ERR = Off;
 	FP_STROCK_H_ERR = Off;
 	T_STROCK_ERR = 0; 
 }

 /*
 
  //
  // 롤링 센서 에러 전압 검출.
  //	
 if ( ADC_ROLLING > VA_STROCK_ERR_H ) { 	// 높은 전압 체크

   if  ( FP_10mSec )     T_ROLLING_ERR++;  
   if ( T_ROLLING_ERR >= VA_STROCK_ERR_TIME )	{
		FP_ROLLING_H_ERR = On;
		T_ROLLING_ERR = 0;
	}
 } 
 else  if ( ADC_ROLLING < VA_STROCK_ERR_L ) 	{ // 낮은 전압 체크
   if  ( FP_10mSec )     T_ROLLING_ERR++;  
   if ( T_ROLLING_ERR >= VA_STROCK_ERR_TIME )	{
		FP_ROLLING_L_ERR = On;
		T_ROLLING_ERR = 0;
	}
 }
 else   {

 	FP_ROLLING_H_ERR = Off;
 	FP_ROLLING_L_ERR = Off;
 	T_ROLLING_ERR = 0; 
 }
*/  
  BUF1 = 0;
  if ( FP_ROLLING_H_ERR)       BUF1 = 1;
  //if ( FP_ROLLING_L_ERR)       BUF1 = 1;
  if ( FP_STROCK_H_ERR)       BUF1 = 1;
  if ( FP_STROCK_L_ERR)        BUF1 = 1;
  if ( FP_BATTERY_ERR)       BUF1 = 1;
  
  if ( MODE_SETTING_1 != 0 )      BUF1 = 0;         // 셋팅 모드이면 종료..
  if ( MODE_SETTING_2 != 0 )      BUF1 = 0;         // 셋팅 모드이면 종료..
  if ( MODE_CHECK     != 0 )      BUF1 = 0;         // 셋팅 모드이면 종료..
  
  if ( BUF1 ==0)        //에러가 없으면 종료..
  {
     T_LAMP=0;	return;
  }

  if ( FP_10mSec ) T_LAMP++;

  //
  //  에러 발생시 램프 점멸 제어 ...

  BUF1 = VA_LAMP1_ON;			// 빠르게
  BUF2 = VA_LAMP1_CYCLE;
  
  if ( (FP_STROCK_H_ERR==1)|| (FP_ROLLING_H_ERR==1) || (FP_STROCK_L_ERR==1)|| (FP_ROLLING_L_ERR==1)  )
  {
   FOL_MANUAL = Off;
   FOL_FLAT = Off;
   FOL_SLOPE = Off;
   FOL_MIDDLE = Off; 
   FOL_SENSTIVE = Off;  
   FOL_SLOW = Off;  
  }

  if ( T_LAMP < BUF1 )	 {
       if ( FP_ROLLING_H_ERR)   FOL_MANUAL =  On;
       if ( FP_ROLLING_L_ERR)   FOL_FLAT = On;
       if ( FP_STROCK_H_ERR )   FOL_MIDDLE= On;
       if ( FP_STROCK_L_ERR )   FOL_SENSTIVE  = On;
 
       if ( FP_BATTERY_ERR )  { 
         FOL_MANUAL = On;
         FOL_FLAT = On;
         FOL_SLOPE = On;
       }
  }

  if ( T_LAMP > BUF2 )	T_LAMP = 0;
}

//
//
//
void	S_CAL_FLAT()
{

  s16  BUF1, BUF2;
  s16  FLAT_SAVE_CENTER;


//
//  수평 센서와 스트록센서와의 비율 계산 루틴..
//  ( 수평센서 센터에서 기울어진 편차 * 2)
//

//  if ( AD_ROLLING_SAMPLING > VA_SET_CENTER )	BUF1 = AD_ROLLING_SAMPLING - VA_SET_CENTER;
//  else				BUF1 = VA_SET_CENTER - AD_ROLLING_SAMPLING;
  
  BUF2 = (s16 )AD_FLAT_SAVE        - (s16 )VA_SET_CENTER;
  BUF1 = (s16 )AD_ROLLING_SAMPLING - (s16 )VA_SET_CENTER;

  FLAT_SAVE_CENTER  =  BUF2 - BUF1;

 if ( FLAT_SAVE_CENTER == 0 )  FLAT_SAVE_CENTER = 0;
 else                          FLAT_SAVE_CENTER = (FLAT_SAVE_CENTER * (int)11 )/(int)10;       // 스트록센서와 수평센서 비율 1.8배 차이남.

//  if ( FLAT_SAVE_CENTER == 0 )  FLAT_SAVE_CENTER = 0;
//  else                          FLAT_SAVE_CENTER = FLAT_SAVE_CENTER;    // 2021.04.01. 1:1임.

//
// 수평 센서값과 수평 설정값을 합산해서 제어 목표값 계산 ...
// 
  BUF1 = SET_BAL +  FLAT_SAVE_CENTER + (int)VA_STROCK_CENTER;
  
  if ( BUF1 <= 0 ) BUF1 = 0;
  if ( BUF1 >= 255 ) BUF1 = 255;
  
  OBJECT_POSITION = BUF1;
//
// 계산에 따른 리미트 구간 체크..
// 
  if (OBJECT_POSITION      > VA_STROCK_UP_LIMIT )	OBJECT_POSITION = VA_STROCK_UP_LIMIT; // 자동 리미트 구간 다시 정함.
  else if (OBJECT_POSITION < VA_STROCK_DOWN_LIMIT) 	OBJECT_POSITION = VA_STROCK_DOWN_LIMIT;
 

}

//
//
//
void	S_CAL_SLOPE()
{
  s16  BUF1, BUF2;

 
//
//  수평 센서와 스트록센서와의 비율 계산 루틴..
//  ( 수평센서 센터에서 기울어진 편차 * 2)
//

//  AD_ROLLING_SAMPLING= AD_ROLLING;

  if ( AD_ROLLING_SAMPLING > VA_SET_CENTER )	BUF1 = AD_ROLLING_SAMPLING - VA_SET_CENTER;
  else				                BUF1 = VA_SET_CENTER - AD_ROLLING_SAMPLING;
  
  BUF2 = (BUF1*(int)11)/(int)10;  // 수평센서와 스트록센서 비율 1.8배 치이남.

//    BUF2 = BUF1 ;  // 수평센서와 스트록센서 비율  1 : 1배 치이남. 2021.04.01
//
// 수평 센서값과 수평 설정값을 합산해서 제어 목표값 계산 ...
// 
  if ( AD_ROLLING_SAMPLING > VA_SET_CENTER )	BUF1 = (s16 )VA_STROCK_CENTER  - (s16 )BUF2;
  else				                BUF1 = (s16 )BUF2           + (s16 )VA_STROCK_CENTER;
  

  BUF2 = SET_BAL + BUF1;		
  if ( BUF2 <= 0 ) BUF2 = 0;
  if ( BUF2 >= 255 ) BUF2 = 255;

  OBJECT_POSITION = BUF2;
//
// 계산에 따른 리미트 구간 체크..
// 
  if (OBJECT_POSITION      > VA_STROCK_UP_LIMIT )	OBJECT_POSITION = VA_STROCK_UP_LIMIT; // 자동 리미트 구간 다시 정함.
  else if (OBJECT_POSITION < VA_STROCK_DOWN_LIMIT) 	OBJECT_POSITION = VA_STROCK_DOWN_LIMIT;
}

//
// 수평제어 밸브 출력 제어 루틴..
//
void    BALANCE_FUZZY(u8  Init)
{
 static   u8  BALANCE_STATUS = 0;
 u8     STATUS_BEFORE, Out_Run; 
 
 static u8    T_OUT_UP = 0;
 static u8    T_OUT_DOWN = 0;     
// static u8    T_OUT_GYRO = 0;
// static u8    STATUS_GYRO_Before = 0;
 

 if (Init==0)
 {
      BALANCE_AUTO_STATUS= 0;   // 수평 제어 모드 clear..
      BALANCE_STATUS = 0;       // 수평제어 clear..
      T_OUT_UP = 0;
      T_OUT_DOWN = 0;
 }
//
//  목표값과 스트록센서값의 차이를 구하여 상승/하강 제어..
//
 
  STATUS_BEFORE = BALANCE_STATUS;        // 과거 제어 상태 저장..
 

        if ( AD_STROCK + BALANCE_DEADBAND < OBJECT_POSITION )  BALANCE_STATUS=  VAB_DOWN_MODE;  
  else  if ( AD_STROCK - BALANCE_DEADBAND > OBJECT_POSITION )  BALANCE_STATUS=  VAB_UP_MODE;    
  else 
  { 
     //  불감대 영역 안에서 수평 설정 다이얼을 조정
     //  했을 경우는 바로 움직이도록 제어.. 
     //
     if (  BALANCE_AUTO_STATUS  &  VAB_SET_ADJUST )
     {
       if ( AD_STROCK < OBJECT_POSITION )        BALANCE_STATUS=  VAB_DOWN_MODE;  
       else if ( AD_STROCK > OBJECT_POSITION )   BALANCE_STATUS=  VAB_UP_MODE;  
     }
  }

  BALANCE_AUTO_STATUS  &=  ~VAB_SET_ADJUST;  // 수평 조정 VR조작 FLAG CLEAR
      
  //
  // Fuzzy controll...
  //
  Out_Run = VAB_STOP_MODE;     // 멈춤 출력 제어로..  

/*   


//
// Gyro 우선 제어 루틴..
//  
#define VAC_GYRO_DETECT         5000
#define VAC_GYRO_DELAY           5             // 출력 DELAY 시간..50mSec 더 출력..
  
  if ( GYRO_X >= VAC_GYRO_DETECT )   
  {
    Out_Run        = VAB_DOWN_MODE;             // 상승 출력모드..
    STATUS_GYRO_Before = Out_Run;
    T_OUT_GYRO = VAC_GYRO_DELAY;
  }
  else if ( GYRO_X <= -VAC_GYRO_DETECT ) {
    Out_Run        = VAB_UP_MODE;             // 상승 출력모드..
    STATUS_GYRO_Before = Out_Run;
    T_OUT_GYRO = VAC_GYRO_DELAY;
  }
 
  if (  T_OUT_GYRO != 0 ) {
      if  (FP_10mSec ) T_OUT_GYRO--;
      Out_Run = STATUS_GYRO_Before;
      return;
  }
*/  
//
// 수평센서 제어 루틴..
//    
    
  if (  STATUS_BEFORE == VAB_UP_MODE  )
  {  
       // 현재도 상승 해야 되고 과거도 상승 했으면 상승..
       // 그 이외에는 이단 멈춤.
       if (BALANCE_STATUS == VAB_UP_MODE )
       {
         if (AD_STROCK <= OBJECT_POSITION+2 )       
         { // 목표에 도달 했거나 이상 위치일경우 멈춤.
            Out_Run = VAB_STOP_MODE;     // 멈춤 출력 제어로..  
         }
         else Out_Run        = VAB_UP_MODE;             // 상승 출력모드..
       }  
  }
  else if ( STATUS_BEFORE == VAB_DOWN_MODE )
  {     // 현재도 하강 해야 되고 과거도 하강 했으면 하강..
        if (BALANCE_STATUS == VAB_DOWN_MODE )
        {
          if (AD_STROCK >= OBJECT_POSITION-2 )       
          { // 목표에 도달 했거나 이상 위치일경우 멈춤.
            Out_Run = VAB_STOP_MODE;     // 멈춤 출력 제어로..  
          }
          else Out_Run        = VAB_DOWN_MODE;          // 상승 출력모드..
        }
  }
  else   // 과거 상태가 멈춤 상태에서 출발 했으면..
  {      // 플로팅 영역에 있을땐 멈춤 그외에는 출력함..
             if ( BALANCE_STATUS == VAB_UP_MODE )    Out_Run        = VAB_UP_MODE;      // 상승 출력모드..
        else if ( BALANCE_STATUS == VAB_DOWN_MODE )  Out_Run        = VAB_DOWN_MODE;    // 하강 출력모드..
        else                                         Out_Run        = VAB_STOP_MODE;    // 하강 출력모드..
  }

 //
 // 수평 솔레노이드 출력제어 루틴..
 //
  if (  Out_Run == VAB_UP_MODE )
  {
        T_OUT_DOWN = 0;
        BALANCE_STATUS=  VAB_UP_MODE;  // 2번 상태 반복 입력 시킴(프로그램 이해 쉽게)으로 

        if ( FP_10mSec ) T_OUT_UP++;
        if ( T_OUT_UP >= VA_BAL_UP_DELAY )      {   
        
           T_OUT_UP= VA_BAL_UP_DELAY;
           FOS_BAL_DOWN   = On;
        }
     
        if (AD_STROCK < VA_STROCK_DOWN_LIMIT)     FOS_BAL_DOWN = Off;
        
        return;
  }
  else if ( Out_Run == VAB_DOWN_MODE )
  {
        T_OUT_UP=  0;
        BALANCE_STATUS=  VAB_DOWN_MODE;    
        if ( FP_10mSec ) T_OUT_DOWN++;
        if ( T_OUT_DOWN >= VA_BAL_DOWN_DELAY )      {   
        
           T_OUT_DOWN = VA_BAL_DOWN_DELAY;
           FOS_BAL_UP   = On;
        }
        if (AD_STROCK > VA_STROCK_UP_LIMIT)       FOS_BAL_UP = Off;
        return;
  }
  
  BALANCE_STATUS =  VAB_STOP_MODE; 
  T_OUT_UP = 0;
  T_OUT_DOWN = 0;
  FOS_BAL_DOWN = Off; 
  FOS_BAL_UP   = Off;
  return;
 

}

//
// 수평제어 자동루틴..
//
void	AUTO_SLOPE()
{
  static u8   T_SLOPE = 0;
//
// 수평자동 스위치 ON ( TIME DELAY ROUTINE )
//

 if ( (FP_ROLLING_H_ERR==1) || (FP_ROLLING_L_ERR==1) ) FP_SLOPE = Off;     // 센서 에러면 자동 off
 if ( (FP_STROCK_H_ERR==1) || (FP_STROCK_L_ERR==1) ) FP_SLOPE = Off;     // 센서 에러면 자동 off

  if  (FP_SLOPE==0 ) 
  {
    if ( FP_10mSec ) T_SLOPE++;

    if ( T_SLOPE > VA_BALANCE_DELAY )  {
      FP_SLOPE = On;
      T_SLOPE  = 0;
      BALANCE_FUZZY(0); // 초기화..

    }
    return;
 }

//
//  수평 조정 VR을 중심으로  조정 목표값을 설정한다..
//  수평 보정값 = 조정VR/ 2 - 63값
//  조정 VR값과 수평센서 값과 MIX 하여 목표값을 계산 한다.
//
//  BUF1 = AD_BALANCE_SET / 2 ;
//  OBJECT_POSITION = AD_BALANCE_SET - 63;

  S_CAL_SLOPE();

  BALANCE_FUZZY(1);

}
//
// 수평제어 자동루틴..
//
void	AUTO_FLAT()
{

 static u8   T_FLAT = 0;

//
// 수평자동 스위치 ON ( TIME DELAY ROUTINE )
//
 if ( (FP_ROLLING_H_ERR==1) || (FP_ROLLING_L_ERR==1) ) FP_FLAT = Off;     // 센서 에러면 자동 off
 if ( (FP_STROCK_H_ERR==1) || (FP_STROCK_L_ERR==1) ) FP_FLAT = Off;     // 센서 에러면 자동 off

  if  (FP_FLAT==0 ) 
  {  
    if ( FP_10mSec ) T_FLAT++;
    if ( T_FLAT > VA_BALANCE_DELAY )  {
      FP_FLAT = On;
      T_FLAT  = 0;
      BALANCE_FUZZY(0); // 초기화..
    }
    return;
 }
//
//  수평 조정 VR을 중심으로  조정 목표값을 설정한다..
//  수평 보정값 = 조정VR/ 2 - 63값
//  조정 VR값과 수평센서 값과 MIX 하여 목표값을 계산 한다.
//
//  BUF1 = AD_BALANCE_SET / 2 ;
//  OBJECT_POSITION = AD_BALANCE_SET - 63;
  S_CAL_FLAT();
  BALANCE_FUZZY(1);
}


//
//  평행화 제어 루틴..
//
void    AUTO_PALL()
{
    
  if ( FP_POWER_ON )       FP_BAL_PALL_RUN = Off;
  
  if ( EEPROM_OPTION_MANUAL == VA_OPTION_MANUAL_OFF )   {  
     if ( AD_STROCK < (u8 )VA_STROCK_CENTER )     FOS_BAL_UP     = On;
     if ( AD_STROCK > (u8 )VA_STROCK_CENTER )     FOS_BAL_DOWN   = On;
     if ( (AD_STROCK < (u8 )VA_STROCK_CENTER+3) && (AD_STROCK  > (u8 )VA_STROCK_CENTER-3) )    FP_BAL_PALL_RUN = Off; // 보정 루틴 없으면 종료..
     return;
  }
 
  //
  // 리크 보정시..
  //

  if (  Leak_Option_Flag == 0 )  // 보정 루틴 없으면 종료..
  {
    if ( AD_STROCK < (u8 )VA_STROCK_CENTER )     FOS_BAL_UP    = On;
    if ( AD_STROCK > (u8 )VA_STROCK_CENTER )     FOS_BAL_DOWN   = On;
    if ( (AD_STROCK < (u8 )VA_STROCK_CENTER+3) && (AD_STROCK  > (u8 )VA_STROCK_CENTER-3) )   Leak_Option_Flag = 1;
    AUTO_MANUAL(1);            // 옵션 셋팅하면 현재값을 기준값으로 저장..
    return;
  }
  //
  // 센타 기준해서 +-인이면 해제..
  //
 // if( VA_STROCK_CENTER+3 < AD_STROCK )   FOS_BAL_DOWN = 1;
    if ( AD_STROCK < (u8 )VA_STROCK_CENTER-3 )   FOS_BAL_UP= On; // 상승 모상은 하지 안도록 수정..
    AUTO_MANUAL(1);            // 옵션 셋팅하면 현재값을 기준값으로 저장..
    return;
}
//
// 수평제어 자동루틴..
//  

void	AUTO_BALANCE()
{
  
  static s16   AD_BALANCE_SET_BEFORE = 0;
  static u8  BALANCE_MODE = 0;
  static u8  BALANCE_MODE_BEFORE = 0;
  static u8   T_Out_Limit = 0;
    u8           BUF1;
    
  s16  SiBuf ;
  
  
  if ( (MODE_SETTING_1 != 0) || (MODE_SETTING_2 != 0) || (MODE_CHECK != 0 ))  
  {
      return;         // 셋팅 모드이면 종료..
  }
//
// 감도 제어 루틴..
//   
  switch ( MODE_SENSTIVE )      {
  case 0:
         BALANCE_DEADBAND =  EE_MODE_SENSTIVE; 
         ROLLING_CHANGE = EE_MODE_SENSTIVE_TIME; 
         break;
  case 1:
         BALANCE_DEADBAND =  EE_MODE_STANDARD; 
         ROLLING_CHANGE = EE_MODE_STANDARD_TIME; 
         break;
  default:        
         BALANCE_DEADBAND =  EE_MODE_SLOW;
         ROLLING_CHANGE = EE_MODE_SLOW; 
         break;
  }
  
//ROLLING_CHANGE = VA_ROLLING_CHANGE_MIN;  
// 
// 수평 조절 제어 루틴..
//

  gu32Balance_set  = (u16 )(AD_BALANCE_SET);
  gu32Balance_set = ((gu32Balance_set*185)/255)+35;//  조정 범위 << 0.685V ~ 4.312V >>

//  if(gu32Balance_set > VA_STROCK_UP_LIMIT )      gu32Balance_set = VA_STROCK_UP_LIMIT;//VA_STROCK_UP_LIMIT
//  if(gu32Balance_set < VA_STROCK_DOWN_LIMIT )    gu32Balance_set = VA_STROCK_DOWN_LIMIT;// VA_STROCK_DOWN_LIMIT

  //
  // 수평 목표값 계산 루틴..
  // 설정 VR 2.5±0.2 구간은 수평센서 센터값으로..
  //   0     ~  117 ~ 137 ~ 255
  //   
  //  최종 목표값 =  VA_VAL_CENTER ± X  ( 117 * 0.0196 =  2.23V)
  //  ± 22.9 도 조절폭을 가짐 ...
  //
  //

  if (  VA_SET_CENTER + VA_EFFECTIVE_RANGE < gu32Balance_set )   // 조정 범위 0.885V ~ 4.112V로 변환..
  {
   	BUF1 = gu32Balance_set - (VA_SET_CENTER + VA_EFFECTIVE_RANGE);
   	if ( VA_SETTING_LIMIT < BUF1 )  BUF1 = VA_SETTING_LIMIT;                // 설정값 제한..
  	SET_BAL = (s16 )BUF1;
  }
  else if (  VA_SET_CENTER - VA_EFFECTIVE_RANGE > gu32Balance_set )
  {
   	BUF1 = (VA_SET_CENTER - VA_EFFECTIVE_RANGE) - gu32Balance_set;
   	if ( VA_SETTING_LIMIT < BUF1 )  BUF1 = VA_SETTING_LIMIT;                // 설정값 제한..
   	SET_BAL =  - (s16 )BUF1; 
  }
  else  SET_BAL = 0;
 

//
// 설정 다이얼을 조작하였는지 확인후 바로 동작 되도록 제어..
//  
 SiBuf = AD_BALANCE_SET_BEFORE;
 
  if ( AD_BALANCE_SET_BEFORE != AD_BALANCE_SET )
  {
     SiBuf = SiBuf - AD_BALANCE_SET_BEFORE;
     
     if (  (SiBuf >= 2) || (SiBuf <= -2 ) )
     {
       AD_BALANCE_SET_BEFORE = AD_BALANCE_SET;
       BALANCE_AUTO_STATUS |= VAB_SET_ADJUST;             // 조작 Flag ON..
     }
  }
 
 
  BALANCE_MODE_BEFORE = BALANCE_MODE;   // 수평제어 모드 과거상태 저장..

 //
 // 수평 자동 제어 모드에 따라서 제어 하는 루틴..
 // 
  if ( FI_SEL )         // 보튼을 누르고 있는 동안은 자동 OFF되도록 수정..
  {
        BALANCE_MODE = 3;                       // 수동 모드 
        AD_FLAT_SAVE = AD_ROLLING_SAMPLING;              // 경사지 제어 수평센서값 저장..
  }
  else if ( FM_SLOPE )
  {
        BALANCE_MODE = 1;                        // 자동 모드
        AD_FLAT_SAVE = AD_ROLLING_SAMPLING;      // 경사지 제어 수평센서값 저장..
  }
  else if ( FM_FLAT )
  {     
        BALANCE_MODE = 2;                       // 경사지 모드 
  }
  else 
  {     
        BALANCE_MODE = 3;                       // 수동 모드 
        AD_FLAT_SAVE = AD_ROLLING_SAMPLING;              // 경사지 제어 수평센서값 저장..
  }    
  

//
// 수평 평행화 제어 검출 루틴..
//
  if (  FI_UPLIMIT==0 ) {
   
    if ( FIO_UPLIMIT==1)  {
         if(BALANCE_MODE !=3)    { FP_BAL_PALL_RUN =  On;  Leak_Option_Flag = Off; }
    }
    // 상승중에 수평자동/평행화로 조작시 평행화가 동작 되도록 수정함.. 2010.06.21. 
    if ( (BALANCE_MODE_BEFORE != BALANCE_MODE) && (BALANCE_MODE !=3) )     {  FP_BAL_PALL_RUN = On; Leak_Option_Flag = 0; }
  }

//
//
//
  if ( FI_UPLIMIT==0 )  {
    if ( FP_BAL_PALL_RUN==1)  {
      if ( EEPROM_OPTION_MANUAL == VA_OPTION_MANUAL_OFF)  {  
      // LEAK 보상 ON && 상한 위치면 평행화가 항상 실행 되도록 수정함..
       AUTO_PALL();       // 평행화 제어 루틴..
       if ( FP_100mSec )  T_Out_Limit++;
       if ( T_Out_Limit >= VA_PALL_LIMIT_TIME )    FP_BAL_PALL_RUN = Off;  // 10초후 종료..
      }
      else  {
       AUTO_PALL();       // 평행화 제어 루틴..
     } 
    }
  }
  else  
  {
    T_Out_Limit = 0;
    
    if (BALANCE_MODE ==1)  {
      FP_FLAT =Off;
      AUTO_SLOPE();
      AUTO_MANUAL(0);            // 옵션 셋팅하면 현재값을 기준값으로 저장..
      return;
    }

    if (BALANCE_MODE ==2)  {
     if ( BALANCE_MODE_BEFORE != BALANCE_MODE ) { // 모드가 과거상태와 다르면 실행..  
          AD_FLAT_SAVE = AD_ROLLING_SAMPLING;             // 전환 될때 수평센서값을 저장한다..
      }
      FP_SLOPE = Off;
      AUTO_FLAT();              // 경사지 모드..
      AUTO_MANUAL(0);            // 옵션 셋팅하면 현재값을 기준값으로 저장..
      return;
   }
 }

//
//  수평제어 수동제어 루틴..
//
if (BALANCE_MODE == 3) 
{
   AUTO_MANUAL(0);

}
//
//  자동제어 해제 루틴..
//
  FP_SLOPE = Off;
  FP_FLAT =Off;
  return;
     
}

  