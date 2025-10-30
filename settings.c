/************************************** COPYRIGHT 2011 (��)������� ���������************************************

	ȭ  ��  �� : SETTING.C
	���α׷��� :  ���� ���α׷�..
	�� �� ��   : �� �� ȣ
	��    ��   :
                    COMPILER : IAR ARM COMFILER

    2014.01.10.  Default ���� ��ƾ ����.

     93C46�� 64 WORD


******************************************************/

#define   __SETTING_DEFINE__

#include "main.h"
#include "balance.h"
#include "Settings.h"
extern void Delay_1ms(u16 Dly) ;
extern u16     Flash_FULL_Auto(u8 How, u8 ID, u16 Value);

// ���� ���� ��� ����
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
      
      {    9, VAC_ID_ADD_OPTION,       0,   10, 10,   &EEPROM_OPTION_MANUAL},    // 2 ���� FILL 1 �� 
};
//
// ���� �� �ּ�, �ִ�, default , ���� �ּ� Ȯ�� ��ƾ..
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
// ���� ��ȣ�� ���� ID(EEPROM ADDRESS ã��.
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
// ID�� ������ �ڵ����� ���� ó�� �ϴ� ��ƾ..
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

   if ( How == VAC_VALUE_Save ) { // ������ ���� ���..
       if ( (Max>= Value) && (Min <= Value) )     Chk = VAC_OK;
   }
   else if ( How == VAC_AUTO_Save )  {  // �а� ���� ���..

     WATCH_DOG();             // Watch Dog Run...
     Value = EEPROM_READ(EEPROM_ADD); // EEPROM READ VAC_EEPROM_ADD_ROLLING..
     WATCH_DOG();             // Watch Dog Run...
     //
     // ���� ���� Ȯ�� ���� �� ������..
     //
     if ( (Max < Value ) || (Min > Value ) ) {
        Value = Default;
        Chk = VAC_OK;
     }
     else    *EE_Data_Range[Num].Value = Value; // ���� ���� �̸� ����.
   }
   else if ( How == VAC_DEFAULT_Save )  {  // �⺻�� �Է� ��� 
        Value = Default; // ���� ���� �̸� ����.
        Chk = VAC_OK;
   }
   else if ( How == VAC_VALUE_Read ) {  // �� �б�..
     return *EE_Data_Range[Num].Value;
   }
//
// ���� flag on�̸� �����ϰ� ����.
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
// EEPROM �б�, ���� ��ƾ..
//   �μ� ���� :
//   1. How     =  VAC_DEFAULT_Save = ǥ�ذ� ����,  VAC_AUTO_Save= �а� ��ȿ�������̸� ǥ�ذ� ����,  VAC_VALUE_Save = ������ ����.
//   2. Add     =  ���� �ּ� ( �ڵ����� *2 �� �ּҿ� ������)
//   3. Min     = ���� �ּҰ� ����.
//   4. Max     = ���� �ִ밪 ����.
//   5. Default = ���� ǥ�ذ�
//   6. Value   = �������(�ݵ�� &���������� �Ѱܾߵ�.)
//

u16    Flash_Auto(u8 How, u32 Add, u16 Min, u16 Max, u16 Default, u16* Value)
{
   u16 Chk;
u8 ii;

//   if ( Add >= NumbOfVar )     return VAC_NG;  // 30�� �̻��̸� ����..

//   Add = Add * 2; // 16����Ÿ ��..

   Chk = VAC_NG;

   if ( How == VAC_VALUE_Save ) { // ������ ���� ���..
       if ( (Max>= *Value) && (Min <= *Value) ) {
       Chk = VAC_OK;
     }
   }
   else if ( How == VAC_AUTO_Save )  {  // �а� ���� ���..

     WATCH_DOG();             // Watch Dog Run...
     *Value = EEPROM_READ(Add); // EEPROM READ VAC_EEPROM_ADD_ROLLING..

     WATCH_DOG();             // Watch Dog Run...

//     *Value =  Eeprom_Read_Int(Eepprom_Add);
     //
     // ���� ���� Ȯ�� ���� �� ������..
     //
     if ( (Max < *Value ) || (Min > *Value ) ) {
        *Value = Default;
        Chk = VAC_OK;
     }
   }
//
// ���� flag on�̸� �����ϰ� ����.
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
// Flash_Init() �ʱ�ȭ ��ƾ..
//
void  EEPROM_Init(u8  How)
{

//
// ���� �׽�Ʈ..
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
// ������ �׻� 0�� ������ �ʱ�ȭ...
//
  


}
//
// CAN READ DATA ó�� ��ƾ..
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
              Setting_Default = 0;    // �ѹ� ������ �ʱ�ȭ..
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
        // Default �ʱ�ȭ ��ƾ..
         if ( Data == 9999 ) { // �ʱ�ȭ �ڵ� ��ġ �Ұ�츸 �ʱ�ȭ.
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
//  if ( Can_Sentd_Setting == On ) return;   // �߼� ó�����̸� ����..

  if ( Can_Recive_Setting == On ) {
   Can_Recive_Setting    = Off;

   Setting_Send_Data0  = 0;
   Setting_Send_Data1  = Setting_Recive_Data1;
   Setting_Send_Data2  = Setting_Recive_Data2;
   Setting_Send_Data3  = Setting_Recive_Data3;


//   sprintf(Text,"\r\nRecive Num = %4d Data1,2 = %4d %4d", Setting_Recive_Data1, Setting_Send_Data2, Setting_Send_Data3);
//   put_str(Text);

        if ( (Setting_Recive_Data0 & VAC_SET_READ) !=0  )         SUB_CAN_READ();   // �бⵥ���� ó�� ��ƾ..
   else if ( (Setting_Recive_Data0 & VAC_SET_WRITE) !=0 )       SUB_CAN_WRITE();  // ������ ���� ó�� ��ƾ.

//   View_Setting();

 }
}
