/************************************** COPYRIGHT 2011 (주)기원전자 기술연구소************************************

	화  일  명 : SETTING.C
	프로그래명 :  설정 프로그램..
	작 성 자   : 박 강 호
	도    구   :
                    COMPILER : IAR ARM COMFILER

    2014.01.10.  Default 셋팅 루틴 삽입.

     93C46은 64 WORD


******************************************************/

#define   __SETTING_DEFINE__

#include "main.h"
#include "balance.h"
#include "Settings.h"
extern void Delay_1ms(u16 Dly) ;
extern u16     Flash_FULL_Auto(u8 How, u8 ID, u16 Value);

// 설정 관련 상수 선언
//
#define VAC_SET_READ      BIT_7 // DATA READ
#define VAC_SET_WRITE     BIT_6 // DATA WRITE
#define VAC_SET_INIT      BIT_3 // DATA INITI

#define VAC_SET_SEND_EN    BIT_7 // DATA INITI

#define VAC_SET_SEND_READ  BIT_6 // DATA INITI
#define VAC_SET_SEND_WRITE BIT_5 // DATA INITI
#define VAC_SET_SEND_OK    BIT_4 // DATA OK
#define VAC_SET_SEND_NG    BIT_3 // DATA NG

static u8 Setting_Default = 0;

//static char Text[80];

typedef struct  {
   u8   SETTING_NUM;
   u8   ID;
   u16  Min;
   u16  Max;
   u16  Default;
   u16  *Value;
} EE_DATA_FORMAT;

#define  VAC_EE_Data_Total   10

const EE_DATA_FORMAT  EE_Data_Range [VAC_EE_Data_Total]= {
  
//
//     SET No  ID (EEPROM ADDRESS)    MIN MAX DEAFULT   VALUE
//
      {    1, VAC_ID_ADD_ROLLING,                  VA_ROLLING_SETTING_MIN, VA_ROLLING_SETTING_MAX,  VA_SET_CENTER,                      &EE_BAL_SET_DATA },         // 0 ROLLING SENSOR SETTING 
      {    2, VAC_ID_ADD_STROCK,                  VA_STROCK_SETTING_MIN,  VA_STROCK_SETTING_MAX,   VA_STROCK_CENTER,                &EE_STROCK_SET_DATA},    // 1 STROCK SENSOR SETTING 
      
      {    3, VAC_ID_ADD_MODE_SENSTIVE,      VA_MODE_SENSTIVE_MIN,   VA_MODE_SENSTIVE_MAX,   VA_MODE_SENSTIVE_DEFAULT,    &EE_MODE_SENSTIVE },        // 2 SENSTIVE MODE 
      {    4, VAC_ID_ADD_MODE_STANDARD,    VA_MODE_STANDARD_MIN,  VA_MODE_STANDARD_MAX, VA_MODE_STANDARD_DEFAULT,  &EE_MODE_STANDARD},       // 3 SENSTIVE  MODE STANDARD 
      {    5, VAC_ID_ADD_MODE_FAST,            VA_MODE_SLOW_MIN,         VA_MODE_SLOW_MAX,         VA_MODE_SLOW_DEFAULT,          &EE_MODE_SLOW},               // 4 SENSTIVE  MODE FAST
      
      {    6, VAC_ID_ADD_MODE_SENSTIVE_TIME,      VA_MODE_SENSTIVE_TIME_MIN,   VA_MODE_SENSTIVE_TIME_MAX,   VA_MODE_SENSTIVE_TIME_DEFAULT,   &EE_MODE_SENSTIVE_TIME },        // 5 SENSTIVE MODE 
      {    7, VAC_ID_ADD_MODE_STANDARD_TIME,    VA_MODE_STANDARD_TIME_MIN,  VA_MODE_STANDARD_TIME_MAX, VA_MODE_STANDARD_TIME_DEFAULT,  &EE_MODE_STANDARD_TIME},       // 6 SENSTIVE  MODE STANDARD 
      {    8, VAC_ID_ADD_MODE_SLOW_TIME,            VA_MODE_SLOW_TIME_MIN,          VA_MODE_SLOW_TIME_MAX,      VA_MODE_SLOW_TIME_DEFAULT,         &EE_MODE_SLOW_TIME},               // 7 SENSTIVE  MODE FAST
      
      {    9, VAC_ID_ADD_OPTION,       0,   10, 10,   &EEPROM_OPTION_MANUAL},    // 2 후진 FILL 1 값 
};
//
// 설정 값 최소, 최대, default , 저장 주소 확인 루틴..
//
u8  Search_SETTING_NUM( u8 ID, u16 *Min, u16 *Max, u16 *Defalut){
u8  ii;
 for ( ii =0; ii < VAC_EE_Data_Total; ii++ ) {

   if ( EE_Data_Range[ii].ID == ID ) {
      *Min       =  EE_Data_Range[ii].Min;
      *Max       =  EE_Data_Range[ii].Max;
      *Defalut   =  EE_Data_Range[ii].Default;
      return ii;
   }

  }
  return 255;
}
//
// 설정 번호로 설정 ID(EEPROM ADDRESS 찾기.
//
u8  Search_SETTING_NO( u8 NO) {
u8  ii;
 for ( ii =0; ii < VAC_EE_Data_Total; ii++ ) {

   if ( EE_Data_Range[ii].SETTING_NUM == NO ) {
      return EE_Data_Range[ii].ID;
   }

  }
  return 255;
}

//
// ID만 넣으면 자동으로 저장 처리 하는 루틴..
//
u16    Flash_FULL_Auto(u8 How, u8 ID, u16 Value)
{
   u16 Num, Min, Max, Default, Chk;
   u8 EEPROM_ADD, ii;
   for ( Num =0; Num < VAC_EE_Data_Total; Num++ ) {
     if ( EE_Data_Range[Num].ID == ID ) {
        Min       =  EE_Data_Range[Num].Min;
        Max       =  EE_Data_Range[Num].Max;
        Default   =  EE_Data_Range[Num].Default;
        break;
     }
   }

   if ( Num >= VAC_EE_Data_Total)    return VAC_NG;

   EEPROM_ADD =  EE_Data_Range[Num].ID;

   Chk = VAC_NG;

   if ( How == VAC_VALUE_Save ) { // 지정값 저장 모드..
       if ( (Max>= Value) && (Min <= Value) )     Chk = VAC_OK;
   }
   else if ( How == VAC_AUTO_Save )  {  // 읽고 쓰기 모드..

     WATCH_DOG();             // Watch Dog Run...
     Value = EEPROM_READ(EEPROM_ADD); // EEPROM READ VAC_EEPROM_ADD_ROLLING..
     WATCH_DOG();             // Watch Dog Run...
     //
     // 셋팅 범위 확인 벗어 나 있으면..
     //
     if ( (Max < Value ) || (Min > Value ) ) {
        Value = Default;
        Chk = VAC_OK;
     }
     else    *EE_Data_Range[Num].Value = Value; // 정상 범위 이면 저장.
   }
   else if ( How == VAC_DEFAULT_Save )  {  // 기본값 입력 모드 
        Value = Default; // 정상 범위 이면 저장.
        Chk = VAC_OK;
   }
   else if ( How == VAC_VALUE_Read ) {  // 값 읽기..
     return *EE_Data_Range[Num].Value;
   }
//
// 저장 flag on이면 저장하고 종료.
//
   if ( Chk == VAC_OK)  {

        GPIOB->ODR &= ~BIT_5;  // EEPROM WR HIGH  // Enkhbat
        for(ii = 0; ii < 10; ii++){          // Delay
          WATCH_DOG();
          Delay_1ms(1);
        }
        
        *EE_Data_Range[Num].Value = Value;
         EEPROM_WRITE ( EEPROM_ADD, Value);
        for(ii = 0; ii < 10; ii++){          // Delay
          WATCH_DOG();
          Delay_1ms(1);
        }
        GPIOB->ODR |= BIT_5;  // EEPROM WR HIGH  // Enkhbat
        return VAC_OK;
   }
   return VAC_NG;

}


//
// EEPROM 읽기, 쓰기 루틴..
//   인수 설명 :
//   1. How     =  VAC_DEFAULT_Save = 표준값 저장,  VAC_AUTO_Save= 읽고 유효범위외이면 표준값 저장,  VAC_VALUE_Save = 지정값 저장.
//   2. Add     =  자장 주소 ( 자동으로 *2 한 주소에 저장함)
//   3. Min     = 값의 최소값 범위.
//   4. Max     = 값의 최대값 범위.
//   5. Default = 값의 표준값
//   6. Value   = 저장장소(반드시 &변수명으로 넘겨야됨.)
//

u16    Flash_Auto(u8 How, u32 Add, u16 Min, u16 Max, u16 Default, u16* Value)
{
   u16 Chk;
u8 ii;

//   if ( Add >= NumbOfVar )     return VAC_NG;  // 30개 이상이면 에러..

//   Add = Add * 2; // 16데이타 임..

   Chk = VAC_NG;

   if ( How == VAC_VALUE_Save ) { // 지정값 저장 모드..
       if ( (Max>= *Value) && (Min <= *Value) ) {
       Chk = VAC_OK;
     }
   }
   else if ( How == VAC_AUTO_Save )  {  // 읽고 쓰기 모드..

     WATCH_DOG();             // Watch Dog Run...
     *Value = EEPROM_READ(Add); // EEPROM READ VAC_EEPROM_ADD_ROLLING..

     WATCH_DOG();             // Watch Dog Run...

//     *Value =  Eeprom_Read_Int(Eepprom_Add);
     //
     // 셋팅 범위 확인 벗어 나 있으면..
     //
     if ( (Max < *Value ) || (Min > *Value ) ) {
        *Value = Default;
        Chk = VAC_OK;
     }
   }
//
// 저장 flag on이면 저장하고 종료.
//
   if ( Chk == VAC_OK)  {

       GPIOB->ODR &= ~BIT_5;  // EEPROM WR HIGH  // Enkhbat
        for(ii = 0; ii < 10; ii++){          // Delay
          WATCH_DOG();
          Delay_1ms(1);
        }
        WATCH_DOG();             // Watch Dog Run...
        EEPROM_WRITE ( Add, *Value);
        for(ii = 0; ii < 10; ii++){          // Delay
          WATCH_DOG();
          Delay_1ms(1);
        }
       WATCH_DOG();             // Watch Dog Run...
       GPIOB->ODR |= BIT_5;  // EEPROM WR HIGH  // Enkhbat
        return VAC_OK;
   }
   return VAC_NG;
}

//
// Flash_Init() 초기화 루틴..
//
void  EEPROM_Init(u8  How)
{

//
// 변수 테스트..
//
  u8    Resault, ii;
  
        if (How==0)     Resault = VAC_AUTO_Save;
  else  if (How==1)     Resault = VAC_DEFAULT_Save;
  else  return;


  WATCH_DOG();
  Flash_FULL_Auto( Resault, VAC_ID_ADD_ROLLING,0);
  for(ii = 0; ii < 100; ii++){  WATCH_DOG();     Delay_1ms(1);     }
  
  Flash_FULL_Auto( Resault, VAC_ID_ADD_STROCK,0);
  for(ii = 0; ii < 100; ii++){  WATCH_DOG();     Delay_1ms(1);     }

  Flash_FULL_Auto( Resault, VAC_ID_ADD_MODE_SENSTIVE,0);
  for(ii = 0; ii < 100; ii++){  WATCH_DOG();     Delay_1ms(1);     }
  Flash_FULL_Auto( Resault, VAC_ID_ADD_MODE_STANDARD,0);
  for(ii = 0; ii < 100; ii++){  WATCH_DOG();     Delay_1ms(1);     }
  Flash_FULL_Auto( Resault, VAC_ID_ADD_MODE_FAST,0);
  for(ii = 0; ii < 100; ii++){  WATCH_DOG();     Delay_1ms(1);     }

  Flash_FULL_Auto( Resault, VAC_ID_ADD_MODE_SENSTIVE_TIME,0);
  for(ii = 0; ii < 100; ii++){  WATCH_DOG();     Delay_1ms(1);     }
  Flash_FULL_Auto( Resault, VAC_ID_ADD_MODE_STANDARD_TIME,0);
  for(ii = 0; ii < 100; ii++){  WATCH_DOG();     Delay_1ms(1);     }
  Flash_FULL_Auto( Resault, VAC_ID_ADD_MODE_SLOW_TIME,0);

  
  //Flash_FULL_Auto( Resault, VAC_ID_ADD_OPTION,0);

  
//
// 시작이 항상 0인 설정값 초기화...
//
  


}
//
// CAN READ DATA 처리 루틴..
//
void  SUB_CAN_READ()
{    
     u16 DATA;
     u8  SETTING_ID;

     Setting_Send_Data0  = VAC_SET_SEND_EN;
     Setting_Send_Data0  |= VAC_SET_SEND_READ;
     Setting_Send_Data1   = Setting_Recive_Data1;

     if ( Setting_Recive_Data1== 255 ) {
              Setting_Send_Data0 |= VAC_SET_SEND_OK;
              Setting_Send_Data2  = (u8)(0X00ff &(Setting_Default  ));
              Setting_Send_Data3  = (u8)((Setting_Default  >>8));
              Setting_Default = 0;    // 한번 읽으면 초기화..
    }
    else {     
        SETTING_ID = Search_SETTING_NO(Setting_Recive_Data1); 
        if ( SETTING_ID == 255 ) Setting_Send_Data0 |= VAC_SET_SEND_NG;
        else {
           DATA = Flash_FULL_Auto( VAC_VALUE_Read, SETTING_ID, 0);
              Setting_Send_Data0 |= VAC_SET_SEND_OK;
              Setting_Send_Data2  = (u8)(0X00ff &(DATA));
              Setting_Send_Data3  = (u8)((DATA>>8));
        }
    }
//     if (  Setting_Send_Data0 & VAC_SET_SEND_OK ) put_str("\r\nRD ");
//     else                                         put_str("\r\nNG ");
//     sprintf(Text,"Num=%3d D=%3d %3d", Setting_Send_Data1, Setting_Send_Data2, Setting_Send_Data3);
//     put_str(Text);
   return;
}

//
//
//
void  SUB_CAN_WRITE()
{
    u16 Data, Chk;
    u8  SETTING_ID;


    Setting_Send_Data0  = VAC_SET_SEND_EN;
    Setting_Send_Data0  |= VAC_SET_SEND_WRITE;
    Setting_Send_Data1   = Setting_Recive_Data1;

    Data= ((u16)(Setting_Send_Data3)<<8) | (u16)Setting_Send_Data2;


    if ( Setting_Recive_Data1 == 255 ) {
        // Default 초기화 루틴..
         if ( Data == 9999 ) { // 초기화 코드 일치 할경우만 초기화.
            EEPROM_Init (1);   // EEPROM READ...
            Setting_Send_Data0 |= VAC_SET_SEND_OK;
            Setting_Default = 1;
         }   
         else  { 
            Setting_Send_Data0 |= VAC_SET_SEND_NG;;
            Setting_Default = 0;
         }
         Setting_Send_Data2  = (u8)( 0x00ff & Setting_Default    );       // LSB
         Setting_Send_Data3  = (u8)((0xff00 & Setting_Default    )>>8);   // MSB
    }
    else {
        SETTING_ID = Search_SETTING_NO(Setting_Recive_Data1); 
        if ( SETTING_ID == 255 ) {
              Setting_Send_Data0  = VAC_SET_SEND_EN;
              Setting_Send_Data0 |= VAC_SET_SEND_NG;
        }
        else {
          Chk = Flash_FULL_Auto( VAC_VALUE_Save, SETTING_ID, Data);
          if ( Chk == VAC_NG  ) Setting_Send_Data0 |= VAC_SET_SEND_NG;
          else                  Setting_Send_Data0 |= VAC_SET_SEND_OK;
          Setting_Send_Data2  = (u8)( 0x00ff & Data);       // LSB
          Setting_Send_Data3  = (u8)((0xff00 & Data)>>8);   // MSB
        }
    }

/*
     if (  Setting_Send_Data0 & VAC_SET_SEND_OK ) put_str("\r\nWD ");
     else                                         put_str("\r\nNG ");

     sprintf(Text,"Num=%3d D=%3d %3d", Setting_Send_Data1, Setting_Send_Data2, Setting_Send_Data3);
     put_str(Text); 
*/
}


//
// Setting DATA RECIVE PROCESS ROUTINE
//
void    Setting_Proc(void)
{
//  if ( Can_Sentd_Setting == On ) return;   // 발송 처리중이면 종료..

  if ( Can_Recive_Setting == On ) {
   Can_Recive_Setting    = Off;

   Setting_Send_Data0  = 0;
   Setting_Send_Data1  = Setting_Recive_Data1;
   Setting_Send_Data2  = Setting_Recive_Data2;
   Setting_Send_Data3  = Setting_Recive_Data3;


//   sprintf(Text,"\r\nRecive Num = %4d Data1,2 = %4d %4d", Setting_Recive_Data1, Setting_Send_Data2, Setting_Send_Data3);
//   put_str(Text);

        if ( (Setting_Recive_Data0 & VAC_SET_READ) !=0  )         SUB_CAN_READ();   // 읽기데이터 처리 루틴..
   else if ( (Setting_Recive_Data0 & VAC_SET_WRITE) !=0 )       SUB_CAN_WRITE();  // 데이터 쓰기 처리 루틴.

//   View_Setting();

 }
}
