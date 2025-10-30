/*================ COPYRIGHT 2020 (주)기원전자 기술연구소  ====================

        화  일  명 : MAIN.C
	프로그래명 :  트랙터 수평제어 컨트롤러
	작 성 자   : 박 강 호

	도    구   :CORETEX-M3(STM32F103R8T6) 64kByte)
                    EWARM Compiler 5.4Vre
 

   2020 .11.18.
           1. 작성 시작.

   2021.03.31.
           1. 수평센서를 CAN으로 수신하여 제어 되도록 수정.

    2021.04.01. 
           1. 밸브 출력 방향 변경.
            2. 작업기 최대  상승시 스트록센서 46(0.9V), 각도 -8.27(외부수평센서)
                         최대  하강시 스트록센서 199(3.9V) 각도 +8.88
                         결론 :  스트록센서 수평 127           
           3. 상한리미트 스위치 상태 반전.

    2021.04.05.
           1. 각속도 센서 적용.

    2021.04.07.
           1. 각속도 적용 삭제.
           2. 민감도 조정.

    2021.04.22.
           1. SETTING ROUTINE COMPLETE


   NVIC_SETPRIMASK();  // 모든 인터럽트 DISABLE...
   NVIC_RESETPRIMASK();   // 인터럽트 Enable..

** 주의 : 업그레이드시 할때 반드시 프로그램모든 창 다시 열것 ( 그렇지 않으면 옛 화일 열림)

        1). 시작 번지 0x08003000 으로 수정등 3곳 번지 수정할 것.
        2). stm32f10x_nvic.h내의 #define NVIC_VectTab_FLASH           ((u32)0x08003000)로 수정할 것.

        3). 알수 없지만 타이머 실행 시간이 UPGRADE와 단독 실행 할 대 10배 차이남 나중 확인 필요.
            SysTick_SetReload(rcc_clocks.SYSCLK_Frequency/10000);    // 1.00mSec  부트 로더 사용 하지 않을 때 사용 .
       //   SysTick_SetReload(rcc_clocks.SYSCLK_Frequency/1000);    // 1.00mSec  부트 로터 사용시 설정.


   진단기 CAN 코드 발송 요청 프로그램 넣을 것.. 반드시 ..



  CAN통신 수신이 2초이상 시 에러 발생 ( 작업 1,2번 램프 점멸 )..


   업그레이드 컨트롤러 업그레이드 ID 2번임 

 2시간 정도 사용 하면 ecu에서 엑셀 에러 검출 하여 최저 rpm으로 된다고 해서 분석 함.

================================================================================*/

#define __MAIN_H__

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_lib.h"
#include "main.h"


extern void CAN0_Exe(void);
extern void Variable_init(void);  // 변수 초기화..
extern void  CAN_TRASMITTER_PROC(void);    // CAN 송신 관리 프로그램.
uchar proctrl=0;


//=======================================================================
//
//=======================================================================
void Delay_Nop(void) {
u16 i;

//	for(i=0;i<7800;i++); //1ms Delay at 36MHz  1mSec
//	for(i=0;i<3900;i++); //1ms Delay at 36MHz  500uSec
	for(i=0;i<1950;i++); //1ms Delay at 36MHz  250uSec
}
//=======================================================================
//1ms Nop Delay
//=======================================================================
void Delay_1ms(u16 Dly) {
u16 i;

	for(i=0;i<Dly;i++)	Delay_Nop();

}



/*******************************************************************************
* Function Name  : main
* Description    : Main program.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
int main(void)
{
#ifdef DEBUG
  debug();
#endif

      CORTEX_initial();
while(1)

{
  CAN_TRASMITTER_PROC();
}  
      
      Variable_init();         // 변수 초기화..

//    put_str ( " (C)KIWON ELECTRONICS CO.,LTD \n\r");
      timer_setup(0,1);        // 10mSec 간격으로  CAN DATA 발송..2011.02.17. 컴퓨터 반응속도가 느려서 10mSec->30mSec수정..
      timer_setup(1,1);        // 10mSec 간격으로  CAN DATA 발송..2011.02.17. 컴퓨터 반응속도가 느려서 10mSec->30mSec수정..
      timer_setup(2,1);        // 10mSec 간격으로  CAN DATA 발송..2011.02.17. 컴퓨터 반응속도가 느려서 10mSec->30mSec수정..

      while(1){

      switch(proctrl){

        case 0:   Internal_timer_Proc();      iexec_control_proc();
                  proctrl++;
                  break;

        case 1:  
                 CAN_TRASMITTER_PROC();

//               UART1_control_proc();
//               UART2_control_proc();
                 proctrl=0;
                 break;

        case 7:   proctrl=0;       break;
      }
    }

}

#ifdef  DEBUG
/*******************************************************************************
* Function Name  : assert_failed
* Description    : Reports the name of the source file and the source line number
*                  where the assert error has occurred.
* Input          : - file: pointer to the source file name
*                  - line: assert error line source number
* Output         : None
* Return         : None
*******************************************************************************/
void assert_failed(u8* file, u32 line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif
/************************************** COPYRIGHT 2011 (주)기원전자 기술연구소*************************************/