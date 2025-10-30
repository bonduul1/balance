/*===============================================================//	            
//
//	ȭ  ��  �� : BALANCE.H
//	���α׷��� : LS��Ʈ�� Ʈ���� �������� ��Ʈ�ѷ� ��� ȭ��
//	�� �� ��   : �� �� ȣ 
//	��    ��   : CPU : ATMEL MEGA16
//		     COMPILER : CodeVisionAVR
//
//	��    ��   :  2010. 4.28. ���α׷� ����.

   ** Ȯ�� ���� : �������� ����Ʈ ��ġ Ȯ�� �ʿ���..
                                                                                                    
//===============================================================*/	            
//#include "stm32f10x_lib.h"
//#include "main.h"

#ifdef __BALANCE_H__
  #define BAL_EXTRN
#else
  #define BAL_EXTRN extern 
#endif



//
//	�ڵ� ���� ���� ���� ����.

#define		VA_LAMP1_ON		15			
#define		VA_LAMP1_OFF	        15
#define		VA_LAMP1_CYCLE	VA_LAMP1_ON+VA_LAMP1_OFF

#define		VA_LAMP2_ON		100			
#define		VA_LAMP2_OFF	        100
#define		VA_LAMP2_CYCLE	VA_LAMP2_ON+VA_LAMP2_OFF
 

//
//	�ð� ���� ����
// 

#define		VA_ROLL_ERR_TIME	50	// ���� ���� üũ �ð� 500mSec
#define		VA_STROCK_ERR_TIME	50	// ��Ʈ�� ���� ���� üũ �ð� ( 500mSec )
 

//
//	����Ʈ�� ����

#define         VA_SET_CENTER           127     // 
#define         VA_STROCK_CENTER      (u8) ( ((float)2.4/(float)0.0196) )      // ��Ʈ�� ���� �߾Ӱ� 2.4V

/*
#define		VA_STROCK_LIMIT_H	204	// ��Ʈ�� ���� ����Ʈ ��.(4.0[V])
#define		VA_STROCK_LIMIT_L	61	// ��Ʈ�� ���� ����Ʈ ��.(1.2[V]) */

#define		VA_STROCK_LIMIT_H	(u8 )((float)4.8/(float)0.0196)	// ��Ʈ�� ���� ����Ʈ ��.(4.0[V])       // 2010.06.21. �������� �������� �� ����..
#define		VA_STROCK_LIMIT_L	(u8 )((float)0.2/(float)0.0196)	// ��Ʈ�� ���� ����Ʈ ��.(1.2[V]) 

#define		VA_STROCK_ERR_H	(u8 )((float)4.9/(float)0.0196)	// ��Ʈ�� ���� ����Ʈ ��.(4.0[V])               // 2010.06.21. �������� �������� �� ����..
#define		VA_STROCK_ERR_L	(u8 )((float)0.1/(float)0.0196)	// ��Ʈ�� ���� ����Ʈ ��.(1.2[V]) 

/*
#define		VA_STROCK_ERR_H		230	// ��Ʈ�� ���� ���� ��.(4.5[V])
#define		VA_STROCK_ERR_L		25	// ��Ʈ�� ���� ���� ��.(0.4[V]
*/  

#define		VA_ROLLING_ERR_H	220	// ���򼾼� ����Ʈ ��.
#define		VA_ROLLING_ERR_L	30	// ���򼾼� ����Ʈ ��.

//
// �嵥�� ���� 
//
#define   VAC_BATTERY_8V                (u8 )((float)2.0/(float)0.0196)    //(float)8/(float)4/(float)0.0196)
#define   VAC_BATTERY_10V                (u8 )((float)2.5/(float)0.0196)    //(float)10/(float)4/(float)0.0196)
#define   VAC_BATTERY_14V               (u8 )((float)3.5/(float)0.0196)  //(float)14/(float)4/(float)0.0196)
#define   VAC_BATTERY_18V               (u8 )((float)4.5/(float)0.0196)  //(float)18/(float)4/(float)0.0196)


//
// �������� 
//		
#define	    VA_EFFECTIVE_RANGE 	     10	          // 0.2[V]
 
#define     VA_MODE_SENSTIVE_MIN                             1           // ���� �ΰ� DEADBAND        �۾��� +-10�� ���� 190 0.6��
#define     VA_MODE_SENSTIVE_MAX                           10           // ���� �ΰ� DEADBAND        �۾��� +-10�� ���� 190 0.6��
#define     VA_MODE_SENSTIVE_DEFAULT                     2           // ���� �ΰ� DEADBAND        �۾��� +-10�� ���� 190 0.6��


#define     VA_MODE_STANDARD_MIN                            1           // ���� ǥ�� DEADBAND        1.3�� 6 => 4  
#define     VA_MODE_STANDARD_MAX                          10           // ���� ǥ�� DEADBAND        1.3�� 6 => 4  
#define     VA_MODE_STANDARD_DEFAULT                    3           // ���� ǥ�� DEADBAND        1.3�� 6 => 4  

#define     VA_MODE_SLOW_MIN                                    1            // ���� �а� DEADBAND        2.5�� 10 => 8
#define     VA_MODE_SLOW_MAX                                 15            // ���� �а� DEADBAND        2.5�� 10 => 8
#define     VA_MODE_SLOW_DEFAULT                            6            // ���� �а� DEADBAND        2.5�� 10 => 8

#define     VA_MODE_SENSTIVE_TIME_MIN                    2           // 150mSec ���� ���� �ΰ����ø�.. => 100 =>  50
#define     VA_MODE_SENSTIVE_TIME_MAX                  30           // 150mSec ���� ���� �ΰ����ø�.. => 100 =>  50
#define     VA_MODE_SENSTIVE_TIME_DEFAULT           15           // 150mSec ���� ���� �ΰ����ø�.. => 100 =>  50

#define     VA_MODE_STANDARD_TIME_MIN                   2           // 200mSec ���� ���� ǥ�ػ��ø�.. => 150 => 100
#define     VA_MODE_STANDARD_TIME_MAX                 40           // 200mSec ���� ���� ǥ�ػ��ø�.. => 150 => 100
#define     VA_MODE_STANDARD_TIME_DEFAULT          20           // 200mSec ���� ���� ǥ�ػ��ø�.. => 150 => 100

#define     VA_MODE_SLOW_TIME_MIN                           2           // 400mSec ���� ���� �а����ø�.. => 250 => 150
#define     VA_MODE_SLOW_TIME_MAX                        60           // 400mSec ���� ���� �а����ø�.. => 250 => 150
#define     VA_MODE_SLOW_TIME_DEFAULT                 30           // 400mSec ���� ���� �а����ø�.. => 250 => 150

#define	    VA_STROCK_DOWN_LIMIT	(u8 )((float)1.2/(float)0.0196)	// ���� ��Ʈ�ϼ��� ���Ѹ���Ʈ 2010.06.21. ������.
#define	    VA_STROCK_UP_LIMIT	        (u8 )((float)3.7/(float)0.0196)	// ���� ��Ʈ�ϼ��� ���Ѹ���Ʈ 2010.06.21. ������.

#define     VA_SETTING_LIMIT            (u8 )((float)1.5/(float)0.0196)        // ���� ���� VR ������ 0.9~3.9V�� ����..)

#define     VA_BALANCE_DELAY     50                 // ���� ���� ���� 500mSec 
#define     VAB_SET_ADJUST       0x01               // ���� ���� ���̾� ����.
#define     VAB_UP_MODE          2
#define     VAB_DOWN_MODE        1
#define     VAB_STOP_MODE        0

#define     VA_BAL_UP_DELAY      10                // �ַ����̵� 30mSec ���� ���..
#define     VA_BAL_DOWN_DELAY    10                // �ַ����̵� 30mSec ���� ���..

#define     VA_PALL_LIMIT_TIME   50               // ����ȭ 5���� ����.,.

#define     VA_OPTION_MANUAL_OFF         1               // ���� ����..OFF
#define     VA_OPTION_MANUAL_ON         2               // ���� ����..ON

//
// ���� ���û�� 
// 
#define         VA_STROCK_SETTING_MAX   (u8)( ((float)3.2/(float)0.0196) )      // ��Ʈ�� ���� �߾Ӱ� 2.4V ���ù��� 3.2 ~ 2.4 ~1.6
#define         VA_STROCK_SETTING_MIN    (u8)( ((float)1.6/(float)0.0196) )      // ��Ʈ�� ���� �߾Ӱ� 2.4V
#define         VA_ROLLING_SETTING_MAX   (u8)( ((float)3.0/(float)0.0196) )     // ���� ����  3.0 ���� ����.
#define         VA_ROLLING_SETTING_MIN   (u8)( ((float)2.0/(float)0.0196) )     // ���� ����  2.0 ���� ����.


#define         VAC_SETTING_TIME        20        // 2�� ���� Ÿ��.. 

#define         VAC_SETTING_MODE               0X01      // ���ø�� ..
#define         VAC_SETTING_SENSOR_MODE        0X02      // ���ø�� ..����
#define         VAC_SETTING_OPTION_MODE        0X04      // ���ø�� ..�ɼ�
#define         VAC_SETTING_DEFAULT_MODE       0X08      // ���ø�� ..����Ʈ
#define         VAC_SETTING_OK                 0X80      // ���ø�� ..OK
#define         VAC_SETTING_NG                 0X40      // ���ø�� ..NG


// 
BAL_EXTRN u8	      T_LAMP, T_STROCK_ERR, T_ROLLING_ERR;
BAL_EXTRN u8        T_ROLLING_CAL,T_ROLLING_LAMP;
 
BAL_EXTRN u8        T_ROLLING_CHAGE; 

//
// ����ġ ���� ����..
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
// ��� ���� ����..
//
BAL_EXTRN bool  FOS_BAL_DOWN ;
BAL_EXTRN bool  FOS_BAL_UP   ;

BAL_EXTRN bool  FOL_MANUAL ;       // ���� ����.
BAL_EXTRN bool  FOL_FLAT ;         // ����.
BAL_EXTRN bool  FOL_SLOPE ;        // �����.

BAL_EXTRN bool  FOL_SENSTIVE;      // �ΰ� .
BAL_EXTRN bool  FOL_MIDDLE;        // ǥ��.
BAL_EXTRN bool  FOL_SLOW;          // �а�.

BAL_EXTRN bool  FP_SLOPE;
BAL_EXTRN bool  FP_FLAT ;
BAL_EXTRN bool  FP_ROLLING_H_ERR ;
BAL_EXTRN bool  FP_ROLLING_L_ERR ;
BAL_EXTRN bool  FP_STROCK_H_ERR ;
BAL_EXTRN bool  FP_STROCK_L_ERR ;
BAL_EXTRN bool  FP_BAL_SET_ERR;
BAL_EXTRN bool  FP_BATTERY_ERR;    // 10V���� 2�� �̻� ����� ���� OFF

BAL_EXTRN bool  FP_BAL_PALL_RUN;       // ����ȭ FLAG
BAL_EXTRN bool  FP_BAL_SETTING;
BAL_EXTRN u8    Leak_Option_Flag;

BAL_EXTRN u8     MODE_SENSTIVE;
BAL_EXTRN u8	   AD_BALANCE_SET;
BAL_EXTRN u8    ADC_STROCK,  AD_STROCK; 
BAL_EXTRN u8    ADC_ROLLING, AD_ROLLING, ADM_ROLLING[4];
BAL_EXTRN u8    ADC_BATTERY;
 


//
// ���� ���� ����..
// 
//BAL_EXTRN u8   EE_BAL_SET_DATA;              // ���� ���� ���� �� ����
BAL_EXTRN s16  VA_BAL_CALIBRATION;
//BAL_EXTRN u8   EE_STROCK_SET_DATA; 

BAL_EXTRN s16  SET_BAL;
BAL_EXTRN u8   OBJECT_POSITION;
BAL_EXTRN s16  gu32Balance_set;     //061102 ���� ������ 0~255������ ���� ���� ����
BAL_EXTRN u8   BALANCE_DEADBAND; 
BAL_EXTRN u8   ROLLING_CHANGE; 

BAL_EXTRN u8   AD_FLAT_SAVE, AD_ROLLING_SAMPLING;    // ����� ���� ���� ���� 
BAL_EXTRN u8   BALANCE_AUTO_STATUS;
BAL_EXTRN u8 MODE_AUTO;
BAL_EXTRN u8 MODE_AUTO_OLD;

//
// EEPROM ���� ���� ���� .,.
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
