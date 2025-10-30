
/*================ COPYRIGHT 2010 (주)기원전자 기술연구소  =========================

        화  일  명 :  MCP2510_LIB.H
	프로그래명 : CAN제어 PRG
	작 성 자   : 박 강 호
	도    구   :CORETEX-M3(STM32F103V16)
                    EWARM Compiler 5.4Vre
     MCP2510. 
*****************************************************************************************************/


#define   MCP_CS           GpioC->Bit.b9    // MCP2510 CS
#define   MCP_CKP          GpioC->Bit.b6    // MCP2510 CLK 

#define   MCP_SCK          GpioC->Bit.b6    // MCP2510 CLK 
#define   MCP_MOSI         GpioC->Bit.b7    // MCP2510 MOSI
#define   MCP_RESET        GpioA->Bit.b8    // MCP2510 RESET

#define   MCP_MISO       ((GPIOC->IDR) & 0x0100)   // MCP2510 MISO
#define   MCP_INT        ((GPIOB->IDR) & 0x8000)   // MCP2510 INT

#define   VAC_ID_ERROR      0x19FFFF00              // 에러 발생시 송신 ID
#define   VAC_CAN_TIMEOVER  30                      // MCP CHIP 300mSec이상 LOW 유지시 에러 

 
extern u8    MCP_CAN_Readdata[8];
extern u8    MCP_CAN_Writedata[8];

extern  void  MCP_setupModule(void);
extern  void  MCP_write_can( u8 buffer, u8 ext, u32 can_id,u8 dlc, u8 rtr, const u8* data );
extern  void  MCP_write_can_id( u8 mcp_addr, u8 ext, u32 can_id );
extern  void  MCP_transmit(u8 mcp_addr);
extern  u8    MCP_canRead(u32 *id, u8 *msg,  u8 *dlc, u16 *flag);

extern  u32   MCP_Read_id_ING;
extern  u8    MCP_Read_dlc_ING;
extern  u16   MCP_Read_flag_ING;  
extern  bool  MCP_SEND_Busy;    // 발송 처리중 flag
extern  bool  MCP_Reception_Flag;
extern  bool  MCP_Transmit_Busy;    // 발송   발신 flag on되었으면 인터럽트 처리후 실행. 다시 발송..