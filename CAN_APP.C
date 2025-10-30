/*================ COPYRIGHT 2010 (주)기원전자 기술연구소  =========================

        화  일  명 : CAN_APP.C
	프로그래명 : CAN제어 PRG
	작 성 자   : 박 강 호
	도    구   :CORETEX-M3(STM32F103V16)
                    EWARM Compiler 5.4Vre

    2015.04.14. 컴파일러 chp set 변경 STM32F10XX8로 

    1)  Diagnostic ID: 0X19FFFF10   1: 0xff, 2: 0x01 , 8: Page  On Message
         => Broadcasting ID: 0X19FFFF11

    2)  Diagnostic       ID: 0X19FFFF00

*****************************************************************************************************/

//#define CAN_GLOBALS
//#include "INCLUDES.H"
#include "stm32f10x_lib.h"
#include "CAN_APP.H"
#include "main.h"
#include "balance.h"
#include "settings.h"
//#include <stdio.h>
//#include <string.h>
//#include <stdlib.h>

#include "iexec.h"

extern void put_str(const char *str) ;
extern void SCI_Tx_Hex(u8 dat);
extern void CAN0_Init(void);

bool    CANTXERR_FG =Off;
bool    Diagnositc_On =Off;


static bool  HandChk_Send_Flag = Off;
static u8    Hand_Chk_Source_Addr_1 =0;
static u8    Hand_Chk_Source_Addr_2=0;
static u8    Hand_Chk_Source_Data_1=0;
static u8    Hand_Chk_Source_Data_2=0;
static u8    Hand_Chk_Target_Addr=0;
static u8  T_HandChk_Send = 0;

#define VAC_SET_WRITE     BIT_6 // DATA WRITE
#define VAC_SET_SEND_OK    BIT_4 // DATA OK


//static u16  CAN_BUF_1;
//static u8   CAN_BUF8_1;
//extern void dbg_put_char(uchar dat);
 
//u8 Test_Msg2[10];

  
#if DEBUG_EN
	#define DBG_CAN 1
#else
	#define DBG_CAN 1
#endif

//UART_EXT 
char Msg_Buff[64];

static u8 CANTXERR_COUNT = 0;

void  CAN_TRASMITTER_PROC(void);    // CAN 송신 관리 프로그램.
void  RUN_BOOTLOADER(void);         // 업그레이드 리셋 루틴..


//
// CAN DATA RECIVE 상태..
//
void  CAN_Recive_Ok()
{
    if ( GPIOB->ODR & BIT_F )      GPIOB->ODR  &= ~BIT_F;
    else                           GPIOB->ODR  |=  BIT_F;
}

//
// Update 모드 점프..
//
#define ApplicationAddress 0x8000000    // 업그레이드 시작 주소
typedef   void (*pFunction)(void);
u32       JumpAddress;
pFunction Jump_To_Application;

void  RUN_BOOTLOADER(void)
{
  
    
//  put_str("\n\r\n\r Jump Boot\n\r");

    JumpAddress = *(vu32*) (ApplicationAddress + 4);
    Jump_To_Application = (pFunction) JumpAddress;
    Jump_To_Application();
    while(1) ;      // 워치독 RESET 발생 되도록 ...
}

//
// CAN 수신 실행 부 
//

extern void  SUB_CAN_READ(void);

void CAN0_Exe(void) {
   if(CAN0_INC!=CAN0_USE) {
      
       NVIC_SETPRIMASK();  // 모든 인터럽트 DISABLE...

         if( RxMessage[CAN0_USE].ExtId == 0X19FFA050 ) {   // 핸드체커 진단기 연결..
              Hand_Chk_Target_Addr        = RxMessage[CAN0_USE].Data[0];     // 1
              Hand_Chk_Source_Addr_1      = RxMessage[CAN0_USE].Data[1];     // 2
              Hand_Chk_Source_Addr_2      = RxMessage[CAN0_USE].Data[2];     // 3
              Hand_Chk_Source_Data_1      = RxMessage[CAN0_USE].Data[3];     // 4
              Hand_Chk_Source_Data_2      = RxMessage[CAN0_USE].Data[4];     // 5
              T_HandChk_Send = 0;
              HandChk_Send_Flag = On;
        }
       else   if ( RxMessage[CAN0_USE].ExtId == 0x10FF5301 ) {  //   
           if ( ROLL_Recive_Flag == Off) {
              Roll_Recive_Data_1      = RxMessage[CAN0_USE].Data[0];  // 1
              Roll_Recive_Data_2     = RxMessage[CAN0_USE].Data[1];  // 2
              Roll_Recive_Data_3      = RxMessage[CAN0_USE].Data[2];  // 3
              Roll_Recive_Data_4      = RxMessage[CAN0_USE].Data[3];  // 4
              Roll_Recive_Data_5      = RxMessage[CAN0_USE].Data[4];  // 5
              Roll_Recive_Data_6      = RxMessage[CAN0_USE].Data[5];  // 6
              Roll_Recive_Data_7      = RxMessage[CAN0_USE].Data[6];  // 7
              Roll_Recive_Data_8      = RxMessage[CAN0_USE].Data[7];  // 8
              ROLL_Recive_Flag     = On;             // DATA READ OK CHECK
           }
       }
       if ( RxMessage[CAN0_USE].ExtId == 0x10FF5400 ) {  //   
           if ( GYRO_Recive_Flag == Off) {
              Gyro_Recive_Data_1      = RxMessage[CAN0_USE].Data[0];  // 1
              Gyro_Recive_Data_2      = RxMessage[CAN0_USE].Data[1];  // 2
              Gyro_Recive_Data_3      = RxMessage[CAN0_USE].Data[2];  // 3
              Gyro_Recive_Data_4      = RxMessage[CAN0_USE].Data[3];  // 4
              Gyro_Recive_Data_5      = RxMessage[CAN0_USE].Data[4];  // 5
              Gyro_Recive_Data_6      = RxMessage[CAN0_USE].Data[5];  // 6
              Gyro_Recive_Data_7      = RxMessage[CAN0_USE].Data[6];  // 7
              Gyro_Recive_Data_8      = RxMessage[CAN0_USE].Data[7];  // 8
              GYRO_Recive_Flag     = On;             // DATA READ OK CHECK
           }
       }
       else  if ( RxMessage[CAN0_USE].ExtId == 0x19FFA100 ) {  //   
              Diagnositc_On     = On;             // DATA READ OK CHECK
              
       }
       else if ( (RxMessage[CAN0_USE].ExtId == VAC_CAN_UPGRADE_ID) && (RxMessage[CAN0_USE].Data[0]==0x01) && ( RxMessage[CAN0_USE].Data[1]== 0x80) ) // 컨트롤러 ID와 업그래이드 모드 요청이면 진입..
       {
              //
              // 업그레이드 모드로 점프..
              //
               RUN_BOOTLOADER();
 	}
       //
       // 주의 ... 아래 내용 지우지 말것..
       //
      if(++CAN0_USE>=CAN0BUF_SZ) CAN0_USE=0;
   
      NVIC_RESETPRIMASK();   // 인터럽트 Enable..

    }
}

void CAN_Init_Variable(void) {
	CAN0_INC=CAN0_USE=0;
}

void CAN_Std_Tx_nByte(u32 StdId,u8 *pBuff,u8 nByte) {
u8 TxedMailBox;
u32 i;
CanTxMsg TxMessage;

	TxMessage.StdId=StdId;
	// transmit 
	TxMessage.RTR=CAN_RTR_DATA;
	TxMessage.IDE=CAN_ID_STD;
	TxMessage.DLC=nByte;
	for(i=0;i<nByte;i++) {
		TxMessage.Data[i]=*pBuff;
		pBuff++;
	}
	TxedMailBox=CAN_Transmit(&TxMessage); //3개의 메일박스중 비어있는것에 보냄
/*	
	for(i=0;i<0xFFFF;i++) {
		if(CAN_TransmitStatus(TxedMailBox)==CANTXOK) {
        		CANTXERR_FG= Off;	//No Error
			break;
		}
	}
	if(i==0xFFFF) CANTXERR_FG=On;	//Error
*/
}

void CAN_Ext_Tx_nByte(u32 ExtId,u8 *pBuff,u8 nByte) {
u8 TxedMailBox;
u32 i;
CanTxMsg TxMessage;

	// transmit message 
	TxMessage.StdId=0x00;//반드시 Clear할 것
	TxMessage.ExtId=ExtId;
	TxMessage.IDE=CAN_ID_EXT;
	TxMessage.RTR=CAN_RTR_DATA;
	TxMessage.DLC=nByte;
	for(i=0;i<nByte;i++) {
		TxMessage.Data[i]=*pBuff;
		pBuff++;
	}
	
	TxedMailBox=CAN_Transmit(&TxMessage); //3개의 메일박스중 비어있는것에 보냄
/*	
	for(i=0;i<0xFFFF;i++) {
		if(CAN_TransmitStatus(TxedMailBox)==CANTXOK) {
			CANTXERR_FG=Off;	//No Error
			break;
		}
	}
	if(i==0xFFFF) CANTXERR_FG=On;	//Error*/
   
        if ( TxedMailBox == CAN_NO_MB  ) {
	   CANTXERR_COUNT++;
           if ( CANTXERR_COUNT >= 200  )  CANTXERR_COUNT = 200;
        } 
        else if ( CANTXERR_COUNT != 0 ) 	CANTXERR_COUNT--;   // 발송이 잘되면 --처리..
        

        for(i=0;i<0x3FF;i++) {    //  2020.04.28. 주석 삭제.. 대기 하도록 수정..
		if(CAN_TransmitStatus(TxedMailBox)==CANTXOK) {
			CANTXERR_FG=Off;	//No Error
			return;
		}
	}
        CANTXERR_FG = On;
} 

//
//  핸드 체커에 제이타 발송..
//
void  Can_Send_HandChk(void)
{
//u16  Buf_I;

  CAN_MsgSend[0] = CAN_MsgSend[1] = CAN_MsgSend[2] = CAN_MsgSend[3] = 0;
  CAN_MsgSend[4] = CAN_MsgSend[5] = CAN_MsgSend[6] = CAN_MsgSend[7] = 0;


// EEPROM 읽기
//
  if ( Hand_Chk_Target_Addr == 0xF2 )  {  
    Setting_Recive_Data1 = Hand_Chk_Source_Addr_1;
    SUB_CAN_READ();
    if (  Setting_Send_Data0 | VAC_SET_SEND_OK ) {  // 등록된 주소이며 데이터 발송.
      CAN_MsgSend[3] = Setting_Send_Data2;  //하위 바이트 
      CAN_MsgSend[4] = Setting_Send_Data3;  //상위 바이트 
    }
    else  {
      CAN_MsgSend[3] = 0Xff;    //등록되지 않은 데이터 
      CAN_MsgSend[4] = 0xff;
      CAN_MsgSend[5] = 0xff;
      
    }
    Setting_Send_Data0 = 0; // 초기화.    
    Setting_Send_Data2 = 0;
    Setting_Send_Data3 = 0;
    
    CAN_MsgSend[0] = Hand_Chk_Source_Addr_1;
    CAN_MsgSend[1] = Hand_Chk_Source_Addr_2;
    CAN_MsgSend[2] = Hand_Chk_Target_Addr;

    // CAN 데이타 발송..
    //
    CAN_Ext_Tx_nByte(0X19FFA051  ,CAN_MsgSend,8); // 발송 주소 0X19FFA061
    return;
  }
  else if ( Hand_Chk_Target_Addr == 0xF3 )  {  
//
//  EEPROM 쓰기..
//    
   Setting_Recive_Data0 = 0;  
   Setting_Recive_Data0 |= VAC_SET_WRITE;
   Setting_Recive_Data1 = Hand_Chk_Source_Addr_1;   // 수신 데이터 ..
   Setting_Recive_Data2 = Hand_Chk_Source_Data_1;
   Setting_Recive_Data3 = Hand_Chk_Source_Data_2;
   Can_Recive_Setting = On;
   return;
  }
  if ( Hand_Chk_Target_Addr != 0xF1 )  return;  // 진단 아이디 확인 ..

  CAN_MsgSend[0] = Hand_Chk_Source_Addr_1;
  CAN_MsgSend[1] = Hand_Chk_Source_Addr_2;
  CAN_MsgSend[2] = Hand_Chk_Target_Addr;
  
  if ( (Hand_Chk_Source_Addr_1==0x01) && (Hand_Chk_Source_Addr_2==0x01) )
  {
         CAN_MsgSend[3] = AD_STROCK <<2;
         CAN_MsgSend[4] = AD_STROCK >>6;
  }
  else   if ( (Hand_Chk_Source_Addr_1==0x01) && (Hand_Chk_Source_Addr_2==0x02) )
  {
         CAN_MsgSend[3] = ADC_ROLLING <<2;
         CAN_MsgSend[4] = ADC_ROLLING >>6;

  }
  else
  {
  CAN_MsgSend[0] = Hand_Chk_Source_Addr_1;
  CAN_MsgSend[1] = Hand_Chk_Source_Addr_2;
  CAN_MsgSend[2] = 0x7F;                             // 에러 코드.
  CAN_MsgSend[3] = 0X01;
  CAN_MsgSend[4] = 0x02;
  CAN_MsgSend[5] = 0x03;
  }

 //
 // CAN 데이타 발송..
 //
  CAN_Ext_Tx_nByte(0X19FFA051  ,CAN_MsgSend,8); // 발송 주소 0X19FFA061


}

// 
//
// CAN 송신 프로그램 ..
//
void  CAN_TRASMITTER_PROC(void)
{ 
  static u8   T_SEQ = 0;
  static u8   T_SEND= 0;
  static u8   T_TEST= 0;
  
  u8         T_CAN_SEND_STEP ;

  for (T_CAN_SEND_STEP=0;T_CAN_SEND_STEP<10;T_CAN_SEND_STEP++)
  {
 CAN_MsgSend[0]= CAN_MsgSend[1] =CAN_MsgSend[2] =CAN_MsgSend[3] = 0XAA;// 
 CAN_MsgSend[4]= CAN_MsgSend[5] =CAN_MsgSend[6] =CAN_MsgSend[7] = 0XBB;// 

 CAN_Ext_Tx_nByte(0x19FFA102,  CAN_MsgSend,8); // 발송 주소 0x19FFA015
  }
  
  
  if  (FP_CAN_Timer_2mSec )
  {
     FP_CAN_Timer_2mSec = Off;
    
   }
  else return;


   if ( HandChk_Send_Flag == 0 ) T_HandChk_Send = 0;
   else
   {
      T_HandChk_Send += 2;
      if ( T_HandChk_Send >= 4 )      // 4mSec Response
      {
         Can_Send_HandChk();            // 핸드 체커 데이타 발송..
         HandChk_Send_Flag = Off;
         return;
      }
   }   
  
//  if ( FP_POWER_ON ) return;    // 전원 on이후 출력.
   //
   //  CAN 발송 에러에 따라서 발송 속도 변경 하는 루틴..
   //
            if ( CANTXERR_COUNT <= 5 )   T_CAN_SEND_STEP = 2; 
    else if ( CANTXERR_COUNT <= 10 )  T_CAN_SEND_STEP = 4; 
    else if ( CANTXERR_COUNT <= 20 )  T_CAN_SEND_STEP = 8; 
    else if ( CANTXERR_COUNT <= 40 )  T_CAN_SEND_STEP = 16; 
    else if ( CANTXERR_COUNT <= 80 )  T_CAN_SEND_STEP = 32;  
    else                              {
      CAN0_Init();    // CAN초기화
      CANTXERR_FG = Off; 
      T_CAN_SEND_STEP = 0; 
      CANTXERR_COUNT =0;
      T_TEST++;
    }
  
    T_SEND += 2; 
    
//    if (  T_SEND >= 123 )        // 99.1mSec간격 발송  
//    if (  T_SEND >= 130 )        // 104mSec간격 발송  
//    if (  T_SEND >= 250 )        // 200mSec간격 발송  
//    if (  T_SEND >= 150 )       // 120 mSec간격 발송  
    if (  T_SEND >= 126 )
    // 100 mSec간격 발송  
    {
      T_SEND = 0;
      CAN_MsgSend[0]= CAN_MsgSend[1] =CAN_MsgSend[2] =CAN_MsgSend[3] = 0X00;// 
      CAN_MsgSend[4]= CAN_MsgSend[5] =CAN_MsgSend[6] =CAN_MsgSend[7] = 0X00;// 
      

      if ( FI_SEL )               CAN_MsgSend[0] |= BIT_0;
      if ( FI_SENSTIVE )     CAN_MsgSend[0] |= BIT_1;
      if ( FI_BAL_UP )         CAN_MsgSend[0] |= BIT_2;
      if ( FI_BAL_DOWN )    CAN_MsgSend[0] |= BIT_3;
      
      if ( FI_EX_BAL_UP )       CAN_MsgSend[0] |= BIT_4;
      if ( FI_EX_BAL_DOWN )  CAN_MsgSend[0] |= BIT_5;
      if ( FI_UPLIMIT )            CAN_MsgSend[0] |= BIT_6;
      if ( FM_SLOPE )            CAN_MsgSend[0] |= BIT_7;
      
      if ( FM_FLAT )             CAN_MsgSend[1] |= BIT_0;
      if ( FM_MANUAL)         CAN_MsgSend[1] |= BIT_1;
      if ( FOS_BAL_DOWN)    CAN_MsgSend[1] |= BIT_2;
      if ( FOS_BAL_UP)         CAN_MsgSend[1] |= BIT_3;

      if ( FOL_MANUAL)       CAN_MsgSend[1] |= BIT_4;
      if ( FOL_FLAT)            CAN_MsgSend[1] |= BIT_5;
      if ( FOL_SLOPE)         CAN_MsgSend[1] |= BIT_6;
      if ( FOL_SENSTIVE)    CAN_MsgSend[1] |= BIT_7;

      if ( FOL_MIDDLE )               CAN_MsgSend[2] |= BIT_0;      // 와셔 전 출력.
      if ( FOL_SLOW )                  CAN_MsgSend[2] |= BIT_1;      // 와셔 후 
      if ( FP_SLOPE )                  CAN_MsgSend[2] |= BIT_2;      // 와이퍼 전
      if ( FP_FLAT )                    CAN_MsgSend[2] |= BIT_3;      // 와이퍼 후 
      if ( FP_ROLLING_H_ERR )    CAN_MsgSend[2] |= BIT_4;      // 열선 
      if ( FP_ROLLING_L_ERR )    CAN_MsgSend[2] |= BIT_5;      // 경광등
      if ( FP_STROCK_H_ERR )    CAN_MsgSend[2] |= BIT_6;       // 13번핀 예약출력 - 
      if ( FP_STROCK_L_ERR )    CAN_MsgSend[2] |= BIT_7;       // ? 출력 없음.
  
     if ( FP_BAL_SET_ERR )       CAN_MsgSend[3] |= BIT_0;       // 전방 상향 램프 
     if ( FP_BATTERY_ERR )      CAN_MsgSend[3] |= BIT_1;       // 전방 하향 램프 
     if ( FP_BAL_PALL_RUN )     CAN_MsgSend[3] |= BIT_2;       // 후방 상향 램프 
     if ( FP_BAL_SETTING )       CAN_MsgSend[3] |= BIT_3;       // 14번핀 예약 출력 
     if ( Leak_Option_Flag )        CAN_MsgSend[3] |= BIT_4;       // 후방 하항 램프

      CAN_MsgSend[4]  = AD_BALANCE_SET ;
      CAN_MsgSend[5]  = AD_STROCK;
      CAN_MsgSend[6]  = ADC_BATTERY ;                   // 전원전압.
      CAN_MsgSend[7] = VAC_PROGRAM_VERSION;     // 모델명.. 1783

      CAN_Ext_Tx_nByte(0x19FFA102,  CAN_MsgSend,8); // 발송 주소 0x19FFA015
      
      if ( CANTXERR_FG == On )  T_SEND = T_SEND - 40;   // 재발송 하도록 
      return;
    }

    
    T_SEQ ++;

    if ( Diagnositc_On == Off) {        // 진단 요청 코드 수신후 부터 코드 발송..
        T_SEQ =0;
        return;
    }
    
    if ( T_SEQ == 10 ) {  // 80mSSec 
      T_SEQ =0;
      CAN_MsgSend[0]= CAN_MsgSend[1] =CAN_MsgSend[2] =CAN_MsgSend[3] = 0X00;// 
      CAN_MsgSend[4]= CAN_MsgSend[5] =CAN_MsgSend[6] =CAN_MsgSend[7] = 0X00;// 

      CAN_MsgSend[0]  = AD_ROLLING; 
      CAN_MsgSend[1]  = AD_STROCK;
      CAN_MsgSend[2]  = OBJECT_POSITION;
      CAN_MsgSend[3]  = GYRO_X;

      CAN_MsgSend[4]  = YAW_ANGLE;
      CAN_MsgSend[5]  = PITCH_ANGLE;
      CAN_MsgSend[6]  = ADC_ROLLING;
      CAN_MsgSend[7]  = ADC_STROCK;
      CAN_Ext_Tx_nByte(0x19FFA202,CAN_MsgSend,8); // 발송 주소 0x19FFA015
      return;
   }  
}




