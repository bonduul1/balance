/*================ COPYRIGHT 2020 (��)������� ���������  ====================

        ȭ  ��  �� : MAIN.C
	���α׷��� :  Ʈ���� �������� ��Ʈ�ѷ�
	�� �� ��   : �� �� ȣ

	��    ��   :CORETEX-M3(STM32F103R8T6) 64kByte)
                    EWARM Compiler 5.4Vre
 

   2020 .11.18.
           1. �ۼ� ����.

   2021.03.31.
           1. ���򼾼��� CAN���� �����Ͽ� ���� �ǵ��� ����.

    2021.04.01. 
           1. ��� ��� ���� ����.
            2. �۾��� �ִ�  ��½� ��Ʈ�ϼ��� 46(0.9V), ���� -8.27(�ܺμ��򼾼�)
                         �ִ�  �ϰ��� ��Ʈ�ϼ��� 199(3.9V) ���� +8.88
                         ��� :  ��Ʈ�ϼ��� ���� 127           
           3. ���Ѹ���Ʈ ����ġ ���� ����.

    2021.04.05.
           1. ���ӵ� ���� ����.

    2021.04.07.
           1. ���ӵ� ���� ����.
           2. �ΰ��� ����.

    2021.04.22.
           1. SETTING ROUTINE COMPLETE


   NVIC_SETPRIMASK();  // ��� ���ͷ�Ʈ DISABLE...
   NVIC_RESETPRIMASK();   // ���ͷ�Ʈ Enable..

** ���� : ���׷��̵�� �Ҷ� �ݵ�� ���α׷���� â �ٽ� ���� ( �׷��� ������ �� ȭ�� ����)

        1). ���� ���� 0x08003000 ���� ������ 3�� ���� ������ ��.
        2). stm32f10x_nvic.h���� #define NVIC_VectTab_FLASH           ((u32)0x08003000)�� ������ ��.

        3). �˼� ������ Ÿ�̸� ���� �ð��� UPGRADE�� �ܵ� ���� �� �� 10�� ���̳� ���� Ȯ�� �ʿ�.
            SysTick_SetReload(rcc_clocks.SYSCLK_Frequency/10000);    // 1.00mSec  ��Ʈ �δ� ��� ���� ���� �� ��� .
       //   SysTick_SetReload(rcc_clocks.SYSCLK_Frequency/1000);    // 1.00mSec  ��Ʈ ���� ���� ����.


   ���ܱ� CAN �ڵ� �߼� ��û ���α׷� ���� ��.. �ݵ�� ..



  CAN��� ������ 2���̻� �� ���� �߻� ( �۾� 1,2�� ���� ���� )..


   ���׷��̵� ��Ʈ�ѷ� ���׷��̵� ID 2���� 

 2�ð� ���� ��� �ϸ� ecu���� ���� ���� ���� �Ͽ� ���� rpm���� �ȴٰ� �ؼ� �м� ��.

================================================================================*/

#define __MAIN_H__

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_lib.h"
#include "main.h"


extern void CAN0_Exe(void);
extern void Variable_init(void);  // ���� �ʱ�ȭ..
extern void  CAN_TRASMITTER_PROC(void);    // CAN �۽� ���� ���α׷�.
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
      
      Variable_init();         // ���� �ʱ�ȭ..

//    put_str ( " (C)KIWON ELECTRONICS CO.,LTD \n\r");
      timer_setup(0,1);        // 10mSec ��������  CAN DATA �߼�..2011.02.17. ��ǻ�� �����ӵ��� ������ 10mSec->30mSec����..
      timer_setup(1,1);        // 10mSec ��������  CAN DATA �߼�..2011.02.17. ��ǻ�� �����ӵ��� ������ 10mSec->30mSec����..
      timer_setup(2,1);        // 10mSec ��������  CAN DATA �߼�..2011.02.17. ��ǻ�� �����ӵ��� ������ 10mSec->30mSec����..

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
/************************************** COPYRIGHT 2011 (��)������� ���������*************************************/