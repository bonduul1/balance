//=============================================================================
//PROJECT   : PIONEER NAVI+DMB(VER 0.1 / 2007.11.07)
//PROCESSOR : STM32F101C6(FLASH=32KB,SRAM=6KB)
//COMPILER  : EWARM V4.41A
//AUTHOR    :  
//UPDATE    : 2007.11.07
//FUNCTION  : 
//=============================================================================

#define CAN0BUF_SZ	10

#ifndef _CAN_APP_H_
#define _CAN_APP_H_

#ifdef   CAN_GLOBALS
#define  CAN_EXT
#else
#define  CAN_EXT  extern
#endif

  #define   VAC_CAN_UPGRADE_ID    0X19F00000    // 업그레이드 ID 1번임..
//#define   VAC_CAN_UPGRADE_ID    0X19F00010    // 업그레이드 ID 2번임..
//#define   VAC_CAN_UPGRADE_ID    0X19F00020    // 업그레이드 ID 3번임..
//#define   VAC_CAN_UPGRADE_ID    0X19F00030    // 업그레이드 ID 4번임..

CAN_EXT u8 CAN0_INC,CAN0_USE;
CAN_EXT CanRxMsg RxMessage[CAN0BUF_SZ];

CAN_EXT void CAN0_Exe(void);
CAN_EXT void CAN_Init_Variable(void);
CAN_EXT void CAN_Std_Tx_nByte(u32 StdId,u8 *pBuff,u8 nByte);
CAN_EXT void CAN_Ext_Tx_nByte(u32 ExtId,u8 *pBuff,u8 nByte);

#endif


