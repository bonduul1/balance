/*==============================================================

	화  일  명 : MAIN.H
	프로그래명 : 동양 8조 이앙기 컨트롤로.
	작 성 자   : 박 강 호
	도    구   :CORETEX-M3(STM32F10X)
                    EWARM Compiler 5.4Vre
*****************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "bit_define.h"
#include "stm32f10x_init.h"
#include "iexec.h"


#define   VAC_PROGRAM_VERSION   11 // 프로그램 버젼. 5

#define         VAC_LED_CONTRAST                     900    // LED 밝기 DEAFULT 값..
#define         VAC_BACKLIGHT_CONTRAST           15    // LED 밝기 DEAFULT 값..

#define   BIT_0    0x0001
#define   BIT_1    0x0002
#define   BIT_2    0x0004
#define   BIT_3    0x0008
#define   BIT_4    0x0010
#define   BIT_5    0x0020
#define   BIT_6    0x0040
#define   BIT_7    0x0080

#define   BIT_8    0x0100
#define   BIT_9    0x0200
#define   BIT_A    0x0400
#define   BIT_B    0x0800
#define   BIT_C    0x1000
#define   BIT_D    0x2000
#define   BIT_E    0x4000
#define   BIT_F    0x8000

#define   BIT_10    0x0400
#define   BIT_11    0x0800
#define   BIT_12    0x1000
#define   BIT_13    0x2000
#define   BIT_14    0x4000
#define   BIT_15    0x8000

#define  VAE_WORK_MODE_1       BIT_0
#define  VAE_WORK_MODE_2       BIT_1
#define  VAE_HMS_MODE_1         BIT_2
#define  VAE_HMS_MODE_2         BIT_3
#define  VAE_BEACON                BIT_4

/******************************************************************************************************/


#ifdef __MAIN_H__
  #define EXTRN


#else
  #define EXTRN extern 
#endif

EXTRN  s16  ROLLING_ANGLE, PITCH_ANGLE, YAW_ANGLE;
EXTRN  s16  GYRO_X;


EXTRN u32 ADC_TEMP;
 
EXTRN bool  FP_POWER_ON, FP_PRG_RUN;
EXTRN bool  FP_mSec, FP_10mSec, FP_100mSec, FP_Sec, FP_CAN_Timer_2mSec;


EXTRN u8    ROLL_Recive_Flag, GYRO_Recive_Flag; 

EXTRN u8    Roll_Recive_Data_1, Roll_Recive_Data_2, Roll_Recive_Data_3, Roll_Recive_Data_4;
EXTRN u8    Roll_Recive_Data_5, Roll_Recive_Data_6, Roll_Recive_Data_7, Roll_Recive_Data_8;

EXTRN u8    Gyro_Recive_Data_1, Gyro_Recive_Data_2, Gyro_Recive_Data_3, Gyro_Recive_Data_4;
EXTRN u8    Gyro_Recive_Data_5, Gyro_Recive_Data_6, Gyro_Recive_Data_7, Gyro_Recive_Data_8;


EXTRN u16   EEPROM_DATA_1,EEPROM_CONTRAST_LED, EEPROM_CONTRAST_BACK;


EXTRN bool  FP_CAN_ERROR ; 


EXTRN u16    T_POWER; 
EXTRN u8      T_CAN_ERROR;

EXTRN u8      SENSOR_PWR_STATUR;
EXTRN u16     ADC_VALVE_CURRENT,ADC_SENSOR;

EXTRN u16     ADM_POWER[4];
//
// 제어 변수 
// 
 
//
EXTRN u8 CAN_MsgSend[8];

//
// MSS SW 저으이 
//
 
extern void   WATCH_DOG();
extern void    OUT_PUT(void);
extern void    EEPROM_WRITE( u8 StartAddress, u8 Data);
extern  u8      EEPROM_READ( u8 StartAddress);
