/*******************************(C) COPYRIGHT 2007 INSEM Inc.****************************************/
/* processor 	  : CORETEX-M3(STM32F10X)         		    				    */
/* compiler       : EWARM Compiler								    */
/* program by	  : 								    */
/* History:											    */
/* 04/13/2007     : Version 1.0									    */
/* copy right	  : Insem Inc.									    */
/****************************************************************************************************/
void iexec_control_proc(void);
void __no_operation(void);
void Delay(u32 i);

#define NOP         __no_operation();

#ifdef __IEXEC_H__

__bits iexec;
uchar cnt;
ushort sum_buf;

#else

extern __bits iexec;
extern uchar cnt;
extern ushort sum_buf;
#endif

#define   PFO_WATCH_DOG  GpioA->Bit.b8         // ¿öÄ¡µ¶ Port 
#define   VAC_PRG_RUN_TIME    2                // PROGRAM RUNING TIME..

#define   On   TRUE
#define   Off  FALSE


/******************************** bit flag define ******************************************************/
#define test_f		iexec.bit.b0

/******************* (C) COPYRIGHT 2007 INSEM Inc ***************************************END OF FILE****/

