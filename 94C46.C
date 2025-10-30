/************************************** COPYRIGHT 2011 (주)기원전자 기술연구소************************************

	화  일  명 : 93C46.C
	프로그래명 : 동양 이앙기 8조 컨트롤러.
	작 성 자   : 박 강 호
	도    구   :
                    COMPILER : IAR ARM COMFILER

******************************************************/


#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "bit_define.h"
#include "stm32f10x_init.h"
//#include "93C46.H"

extern  void NVIC_SETPRIMASK(void);
extern  void NVIC_RESETPRIMASK(void);

#define   CS_93      GpioD->Bit.b2
#define   SK_93      GpioC->Bit.b12
#define   DI_93      GpioC->Bit.b11
#define   DO_93      (GPIOC->IDR & 0x0400)  // 11 BIT
/*
#define   CS_93      GpioB->Bit.b13
#define   SK_93      GpioB->Bit.b12
#define   DI_93      GpioB->Bit.b10
#define   DO_93      (GPIOB->IDR & 0x0800)  // 11 BIT
*/
//
// 1uSec 지연
//
void Delay_1uSec(void) {
u8 i;

	for(i=0;i<6;i++); //1us Delay at 36MHz
}

void WR_9346(u8 addr, u16 dat)
{
  u8 i;
  u8 check;
  u32 test;

  NVIC_SETPRIMASK();  // 모든 인터럽트 DISABLE...

  CS_93=1;              /*  Chip Enable for AT93C66  */
  Delay_1uSec();

  SK_93=1; Delay_1uSec(); SK_93=0;
  DI_93=1; Delay_1uSec();             /* 1 */
  SK_93=1; Delay_1uSec(); SK_93=0;
  DI_93=0; Delay_1uSec();             /* 0 */
  SK_93=1; Delay_1uSec(); SK_93=0;
  DI_93=0; Delay_1uSec();             /* 0 */
  SK_93=1; Delay_1uSec(); SK_93=0;

  check = 0x30;         /* Write Enable :  11xxxx  */
  Delay_1uSec();
  /*  Specify Address to write  */
  for(i=0; i<6; i++)    /* 9346: A5,  9356/66: A7  */
  {
    if(check & 0x20) DI_93=1;
       else          DI_93=0;
    SK_93=1; Delay_1uSec(); SK_93=0; Delay_1uSec();
    check <<= 1;
  }
  CS_93=0;  /* End of Write Enable command */
  Delay_1uSec();


  /*  Process to Write Enable  */
  CS_93=1;              /*  Chip Enable for AT93C46  */
  Delay_1uSec();

  DI_93=1; Delay_1uSec();             /* 1 */
  SK_93=1; Delay_1uSec(); SK_93=0;
  DI_93=0; Delay_1uSec();             /* 0 */
  SK_93=1; Delay_1uSec(); SK_93=0;
  DI_93=1; Delay_1uSec();             /* 1 */
  SK_93=1; Delay_1uSec(); SK_93=0;

  /*  To specify Address  */
  for(i=0; i<6; i++)    /* 9346: A5,  9356/66: A7  */
  {
    if(addr & 0x20) DI_93=1;
       else         DI_93=0;
    SK_93=1; Delay_1uSec(); SK_93=0; Delay_1uSec();
    addr <<= 1;
  }

  /* Data Write to a specified address */
  for(i=0; i<16; i++) {
    if(dat & 0x8000) DI_93=1;
     else            DI_93=0;
    SK_93=1; Delay_1uSec(); SK_93=0; Delay_1uSec();
    dat <<=1;
      }

  DI_93=0; Delay_1uSec();
  CS_93=0; Delay_1uSec(); Delay_1uSec();
  CS_93=1;

  /*  Busy Check  */
  check=0;
  while(!check) {
     test = GPIOB->IDR;
     if ( DO_93 != 0) check++;
     SK_93=1; Delay_1uSec();
     SK_93=0; Delay_1uSec();  }
  test++;
  CS_93=0; Delay_1uSec();

  /*  Process to Write Disable  */
  CS_93=1; Delay_1uSec();

  SK_93=1; Delay_1uSec(); SK_93=0;
  DI_93=1; Delay_1uSec();             /* 1 */
  SK_93=1; Delay_1uSec(); SK_93=0;
  DI_93=0; Delay_1uSec();             /* 0 */
  SK_93=1; Delay_1uSec(); SK_93=0;
  DI_93=0; Delay_1uSec();             /* 0 */
  SK_93=1; Delay_1uSec(); SK_93=0;
  Delay_1uSec();

  check = 0x00;         /* Write Disable : 00xxxx  */
  /*  Specify Address to read  */
  for(i=0; i<6; i++)    /* 9346: A5,  9356/66: A7  */
  {
    if(check & 0x20) DI_93=1;
       else          DI_93=0;
    SK_93=1; Delay_1uSec(); SK_93=0; Delay_1uSec();
    check <<= 1;
  }
  CS_93=0;  /* End of Write Disable command */
  NVIC_RESETPRIMASK();   // 인터럽트 Enable..

}


u16 RD_9346(u8 addr)
{
  u8 i;
  u16 buf;

  NVIC_SETPRIMASK();  // 모든 인터럽트 DISABLE...

  CS_93=1;              /*  Chip Enable for AT93C46  */
  Delay_1uSec();

  DI_93=1; Delay_1uSec();              /* 1 */
  SK_93=1; Delay_1uSec(); SK_93=0;
  DI_93=1; Delay_1uSec();              /* 1 */
  SK_93=1; Delay_1uSec(); SK_93=0;
  DI_93=0; Delay_1uSec();              /* 0 */
  SK_93=1; Delay_1uSec(); SK_93=0;

  /*  Specify Address to read  */
  for(i=0; i<6; i++)    /* 9346: A5,  9356/66: A7  */
  {
    if(addr & 0x20) DI_93=1;
       else         DI_93=0;
    SK_93=1; Delay_1uSec(); SK_93=0; Delay_1uSec();
    addr <<= 1;
  }
  DI_93=0;  /*  End of Address  */
  Delay_1uSec();

  buf=0;
  for(i=0; i<17; i++) {
    if(DO_93 != 0) buf+=(1<<(16-i));
    SK_93=1; Delay_1uSec(); SK_93=0; Delay_1uSec();
      }

  CS_93=0;              /*  Chip Disable for AT93C46  */
  NVIC_RESETPRIMASK();   // 인터럽트 Enable..
  return  buf;
}
