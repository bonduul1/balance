/************************************** COPYRIGHT 2011 (주)기원전자 기술연구소************************************

	화  일  명 : SETTING.H
	프로그래명 : 셋팅 HEAD
	작 성 자   : 박 강 호
	도    구   :
                    COMPILER : IAR ARM COMFILER

******************************************************/
//

//
// FLASH MEMORY ADDRESS
//
#define   VAC_ID_ADD_ROLLING                          1
#define   VAC_ID_ADD_STROCK                           2

#define   VAC_ID_ADD_MODE_SENSTIVE              3
#define   VAC_ID_ADD_MODE_STANDARD             4
#define   VAC_ID_ADD_MODE_FAST                      5
#define   VAC_ID_ADD_MODE_SENSTIVE_TIME      6
#define   VAC_ID_ADD_MODE_STANDARD_TIME     7
#define   VAC_ID_ADD_MODE_SLOW_TIME            8
#define   VAC_ID_ADD_OPTION                             9


/*
#define   VAE_ADD_ROLLING       1
#define   VAE_ADD_STROCK        2
#define   VAE_ADD_OPTION         3
*/

//
// 설정관련 제어 상수 정의..
//
#define     VAC_NG                   50000      // 셋팅 범위 벗어남. 불량.
#define     VAC_OK                   60000      // 섯텡 완료
#define     VAC_DEFAULT_Save            11
#define     VAC_VALUE_Save              12
#define     VAC_VALUE_Read              13
#define     VAC_AUTO_Save               14


#ifdef __SETTING_DEFINE__
	#define EXTRN
#else
	#define EXTRN extern
#endif

//
// 설정 관련 변수 선언
//
EXTRN u8    Setting_Recive_Data0, Setting_Recive_Data1, Setting_Recive_Data2, Setting_Recive_Data3;
EXTRN u8    Setting_Recive_Data4, Setting_Recive_Data5, Setting_Recive_Data6, Setting_Recive_Data7;
EXTRN bool  Can_Recive_Setting;

EXTRN u8    Setting_Send_Data0, Setting_Send_Data1, Setting_Send_Data2, Setting_Send_Data3;
EXTRN u8    Setting_Send_Data4, Setting_Send_Data5, Setting_Send_Data6, Setting_Send_Data7;
//EXTRN bool  Can_Sentd_Setting;

EXTRN u16 EE_BAL_SET_DATA;
EXTRN u16 EE_STROCK_SET_DATA;
EXTRN u16 EEPROM_OPTION_MANUAL;

EXTRN u16 EE_MODE_SENSTIVE;
EXTRN u16 EE_MODE_STANDARD;
EXTRN u16 EE_MODE_SLOW;

EXTRN u16 EE_MODE_SENSTIVE_TIME;
EXTRN u16 EE_MODE_STANDARD_TIME;
EXTRN u16 EE_MODE_SLOW_TIME;


//
// 함수 선언
//
extern void  EEPROM_Init(u8 How);
extern void   Setting_Proc(void); // 셋팅 수신 처리 루틴.
extern u16     Flash_Auto(u8 How, u32 Add, u16 Min, u16 Max, u16 Default, u16* Value);  // 저장,읽기 루틴..
extern u16    Flash_FULL_Auto(u8 How, u8 ID, u16 Value);
extern void  EEPROM_Init(u8  How);
