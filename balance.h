/*===============================================================//	            
//
//	화  일  명 : BALANCE.H
//	프로그래명 : LS엠트론 트랙터 수평제어 컨트롤러 헤더 화일
//	작 성 자   : 박 강 호 
//	도    구   : CPU : ATMEL MEGA16
//		     COMPILER : CodeVisionAVR
//
//	내    용   :  2010. 4.28. 프로그램 시작.

   ** 확인 사항 : 수평제어 리미트 위치 확인 필요함..
                                                                                                    
//===============================================================*/	            
//#include "stm32f10x_lib.h"
//#include "main.h"

#ifdef __BALANCE_H__
  #define BAL_EXTRN
#else
  #define BAL_EXTRN extern 
#endif



//
//	자동 램프 점멸 관련 선언.

#define		VA_LAMP1_ON		15			
#define		VA_LAMP1_OFF	        15
#define		VA_LAMP1_CYCLE	VA_LAMP1_ON+VA_LAMP1_OFF

#define		VA_LAMP2_ON		100			
#define		VA_LAMP2_OFF	        100
#define		VA_LAMP2_CYCLE	VA_LAMP2_ON+VA_LAMP2_OFF
 

//
//	시간 관련 선언
// 

#define		VA_ROLL_ERR_TIME	50	// 센서 에러 체크 시간 500mSec
#define		VA_STROCK_ERR_TIME	50	// 스트록 센서 에러 체크 시간 ( 500mSec )
 

//
//	리미트값 선언

#define         VA_SET_CENTER           127     // 
#define         VA_STROCK_CENTER      (u8) ( ((float)2.4/(float)0.0196) )      // 스트록 센서 중앙값 2.4V

/*
#define		VA_STROCK_LIMIT_H	204	// 스트록 센서 리미트 값.(4.0[V])
#define		VA_STROCK_LIMIT_L	61	// 스트록 센서 리미트 값.(1.2[V]) */

#define		VA_STROCK_LIMIT_H	(u8 )((float)4.8/(float)0.0196)	// 스트록 센서 리미트 값.(4.0[V])       // 2010.06.21. 릴리프가 터지도록 값 수정..
#define		VA_STROCK_LIMIT_L	(u8 )((float)0.2/(float)0.0196)	// 스트록 센서 리미트 값.(1.2[V]) 

#define		VA_STROCK_ERR_H	(u8 )((float)4.9/(float)0.0196)	// 스트록 센서 리미트 값.(4.0[V])               // 2010.06.21. 릴리프가 터지도록 값 수정..
#define		VA_STROCK_ERR_L	(u8 )((float)0.1/(float)0.0196)	// 스트록 센서 리미트 값.(1.2[V]) 

/*
#define		VA_STROCK_ERR_H		230	// 스트록 센서 에러 값.(4.5[V])
#define		VA_STROCK_ERR_L		25	// 스트록 센서 에러 값.(0.4[V]
*/  

#define		VA_ROLLING_ERR_H	220	// 수평센서 리미트 값.
#define		VA_ROLLING_ERR_L	30	// 수평센서 리미트 값.

//
// 밧데리 전압 
//
#define   VAC_BATTERY_8V                (u8 )((float)2.0/(float)0.0196)    //(float)8/(float)4/(float)0.0196)
#define   VAC_BATTERY_10V                (u8 )((float)2.5/(float)0.0196)    //(float)10/(float)4/(float)0.0196)
#define   VAC_BATTERY_14V               (u8 )((float)3.5/(float)0.0196)  //(float)14/(float)4/(float)0.0196)
#define   VAC_BATTERY_18V               (u8 )((float)4.5/(float)0.0196)  //(float)18/(float)4/(float)0.0196)


//
// 수평제어 
//		
#define	    VA_EFFECTIVE_RANGE 	     10	          // 0.2[V]
 
#define     VA_MODE_SENSTIVE_MIN                             1           // 수평 민감 DEADBAND        작업기 +-10도 범위 190 0.6도
#define     VA_MODE_SENSTIVE_MAX                           10           // 수평 민감 DEADBAND        작업기 +-10도 범위 190 0.6도
#define     VA_MODE_SENSTIVE_DEFAULT                     2           // 수평 민감 DEADBAND        작업기 +-10도 범위 190 0.6도


#define     VA_MODE_STANDARD_MIN                            1           // 수평 표준 DEADBAND        1.3도 6 => 4  
#define     VA_MODE_STANDARD_MAX                          10           // 수평 표준 DEADBAND        1.3도 6 => 4  
#define     VA_MODE_STANDARD_DEFAULT                    3           // 수평 표준 DEADBAND        1.3도 6 => 4  

#define     VA_MODE_SLOW_MIN                                    1            // 수평 둔감 DEADBAND        2.5도 10 => 8
#define     VA_MODE_SLOW_MAX                                 15            // 수평 둔감 DEADBAND        2.5도 10 => 8
#define     VA_MODE_SLOW_DEFAULT                            6            // 수평 둔감 DEADBAND        2.5도 10 => 8

#define     VA_MODE_SENSTIVE_TIME_MIN                    2           // 150mSec 간격 수평 민감샘플링.. => 100 =>  50
#define     VA_MODE_SENSTIVE_TIME_MAX                  30           // 150mSec 간격 수평 민감샘플링.. => 100 =>  50
#define     VA_MODE_SENSTIVE_TIME_DEFAULT           15           // 150mSec 간격 수평 민감샘플링.. => 100 =>  50

#define     VA_MODE_STANDARD_TIME_MIN                   2           // 200mSec 간격 수평 표준샘플링.. => 150 => 100
#define     VA_MODE_STANDARD_TIME_MAX                 40           // 200mSec 간격 수평 표준샘플링.. => 150 => 100
#define     VA_MODE_STANDARD_TIME_DEFAULT          20           // 200mSec 간격 수평 표준샘플링.. => 150 => 100

#define     VA_MODE_SLOW_TIME_MIN                           2           // 400mSec 간격 수평 둔감샘플링.. => 250 => 150
#define     VA_MODE_SLOW_TIME_MAX                        60           // 400mSec 간격 수평 둔감샘플링.. => 250 => 150
#define     VA_MODE_SLOW_TIME_DEFAULT                 30           // 400mSec 간격 수평 둔감샘플링.. => 250 => 150

#define	    VA_STROCK_DOWN_LIMIT	(u8 )((float)1.2/(float)0.0196)	// 수평 스트록센서 하한리미트 2010.06.21. 수정함.
#define	    VA_STROCK_UP_LIMIT	        (u8 )((float)3.7/(float)0.0196)	// 수평 스트록센서 상한리미트 2010.06.21. 수정함.

#define     VA_SETTING_LIMIT            (u8 )((float)1.5/(float)0.0196)        // 수평 설정 VR 범위를 0.9~3.9V로 제한..)

#define     VA_BALANCE_DELAY     50                 // 수평 제어 진입 500mSec 
#define     VAB_SET_ADJUST       0x01               // 수평 조절 다이얼 조작.
#define     VAB_UP_MODE          2
#define     VAB_DOWN_MODE        1
#define     VAB_STOP_MODE        0

#define     VA_BAL_UP_DELAY      10                // 솔레노이드 30mSec 지연 출력..
#define     VA_BAL_DOWN_DELAY    10                // 솔레노이드 30mSec 지연 출력..

#define     VA_PALL_LIMIT_TIME   50               // 평행화 5초후 종료.,.

#define     VA_OPTION_MANUAL_OFF         1               // 수동 보정..OFF
#define     VA_OPTION_MANUAL_ON         2               // 수동 보정..ON

//
// 셋팅 관련상수 
// 
#define         VA_STROCK_SETTING_MAX   (u8)( ((float)3.2/(float)0.0196) )      // 스트록 센서 중앙값 2.4V 셋팅범위 3.2 ~ 2.4 ~1.6
#define         VA_STROCK_SETTING_MIN    (u8)( ((float)1.6/(float)0.0196) )      // 스트록 센서 중앙값 2.4V
#define         VA_ROLLING_SETTING_MAX   (u8)( ((float)3.0/(float)0.0196) )     // 수평 센서  3.0 셋팅 상한.
#define         VA_ROLLING_SETTING_MIN   (u8)( ((float)2.0/(float)0.0196) )     // 수평 센서  2.0 셋팅 하한.


#define         VAC_SETTING_TIME        20        // 2초 셋팅 타임.. 

#define         VAC_SETTING_MODE               0X01      // 셋팅모드 ..
#define         VAC_SETTING_SENSOR_MODE        0X02      // 셋팅모드 ..센서
#define         VAC_SETTING_OPTION_MODE        0X04      // 셋팅모드 ..옵션
#define         VAC_SETTING_DEFAULT_MODE       0X08      // 셋팅모드 ..디폴트
#define         VAC_SETTING_OK                 0X80      // 셋팅모드 ..OK
#define         VAC_SETTING_NG                 0X40      // 셋팅모드 ..NG


// 
BAL_EXTRN u8	      T_LAMP, T_STROCK_ERR, T_ROLLING_ERR;
BAL_EXTRN u8        T_ROLLING_CAL,T_ROLLING_LAMP;
 
BAL_EXTRN u8        T_ROLLING_CHAGE; 

//
// 스위치 변수 정의..
//
BAL_EXTRN bool  FI_SEL,FIO_SEL ;                   // SWITCH INPUT BIT DEFINE
BAL_EXTRN bool  FI_SENSTIVE, FIO_SENSTIVE;
BAL_EXTRN bool  FI_BAL_UP, FIO_BAL_UP ;            // SWITCH INPUT BIT DEFINE
BAL_EXTRN bool  FI_BAL_DOWN, FIO_BAL_DOWN ;
BAL_EXTRN bool  FI_EX_BAL_UP, FIO_EX_BAL_UP ;            // SWITCH INPUT BIT DEFINE
BAL_EXTRN bool  FI_EX_BAL_DOWN, FIO_EX_BAL_DOWN ;
BAL_EXTRN bool  FI_UPLIMIT, FIO_UPLIMIT;           // SWITCH INPUT BIT DEFINE

BAL_EXTRN bool  FM_SLOPE;
BAL_EXTRN bool  FM_FLAT;
BAL_EXTRN bool  FM_MANUAL ;

//
// 출력 변수 정의..
//
BAL_EXTRN bool  FOS_BAL_DOWN ;
BAL_EXTRN bool  FOS_BAL_UP   ;

BAL_EXTRN bool  FOL_MANUAL ;       // 수동 램프.
BAL_EXTRN bool  FOL_FLAT ;         // 수평.
BAL_EXTRN bool  FOL_SLOPE ;        // 경사지.

BAL_EXTRN bool  FOL_SENSTIVE;      // 민감 .
BAL_EXTRN bool  FOL_MIDDLE;        // 표준.
BAL_EXTRN bool  FOL_SLOW;          // 둔감.

BAL_EXTRN bool  FP_SLOPE;
BAL_EXTRN bool  FP_FLAT ;
BAL_EXTRN bool  FP_ROLLING_H_ERR ;
BAL_EXTRN bool  FP_ROLLING_L_ERR ;
BAL_EXTRN bool  FP_STROCK_H_ERR ;
BAL_EXTRN bool  FP_STROCK_L_ERR ;
BAL_EXTRN bool  FP_BAL_SET_ERR;
BAL_EXTRN bool  FP_BATTERY_ERR;    // 10V이하 2초 이상 점멸및 제어 OFF

BAL_EXTRN bool  FP_BAL_PALL_RUN;       // 평행화 FLAG
BAL_EXTRN bool  FP_BAL_SETTING;
BAL_EXTRN u8    Leak_Option_Flag;

BAL_EXTRN u8     MODE_SENSTIVE;
BAL_EXTRN u8	   AD_BALANCE_SET;
BAL_EXTRN u8    ADC_STROCK,  AD_STROCK; 
BAL_EXTRN u8    ADC_ROLLING, AD_ROLLING, ADM_ROLLING[4];
BAL_EXTRN u8    ADC_BATTERY;
 


//
// 제어 변수 정의..
// 
//BAL_EXTRN u8   EE_BAL_SET_DATA;              // 수평 센서 보정 값 저장
BAL_EXTRN s16  VA_BAL_CALIBRATION;
//BAL_EXTRN u8   EE_STROCK_SET_DATA; 

BAL_EXTRN s16  SET_BAL;
BAL_EXTRN u8   OBJECT_POSITION;
BAL_EXTRN s16  gu32Balance_set;     //061102 견인 설정부 0~255범위를 위한 변수 지정
BAL_EXTRN u8   BALANCE_DEADBAND; 
BAL_EXTRN u8   ROLLING_CHANGE; 

BAL_EXTRN u8   AD_FLAT_SAVE, AD_ROLLING_SAMPLING;    // 경사지 제어 저장 변수 
BAL_EXTRN u8   BALANCE_AUTO_STATUS;
BAL_EXTRN u8 MODE_AUTO;
BAL_EXTRN u8 MODE_AUTO_OLD;

//
// EEPROM 관련 제어 변수 .,.
//    

//BAL_EXTRN u8   EEPROM_OPTION_MANUAL;
BAL_EXTRN u8   MODE_SETTING_1;
BAL_EXTRN u8   MODE_SETTING_2;
BAL_EXTRN u8   MODE_CHECK;

extern void    BAL_CHECK();
extern void    BAL_SETTING_2();
extern void    BAL_SETTING_1();
extern void    AUTO_MANUAL(u8 Init);
extern void    BALANCE_INIT(void);
extern void	AUTO_BALANCE();
extern void	AUTO_SLOPE();
extern void	AUTO_FLAT();
extern void	ERROR_CHECK();

extern void     AUTO_PALL();
extern void     BALANCE_FUZZY(u8  Init);
extern void	S_CAL_SLOPE();
extern void	S_CAL_FLAT();
