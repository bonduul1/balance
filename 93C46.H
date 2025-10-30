/*==============================================================

	화  일  명 : 93C46.H
	프로그래명 :
	작 성 자   : 박 강 호
	도    구   :CORETEX-M3(STM32F10X)
                    EWARM Compiler 5.4Vre


*****************************************************/

extern void WR_9346(u8 addr, u16 dat);
extern u16  RD_9346(u8 addr);
extern void Delay_1uSec(void);
 