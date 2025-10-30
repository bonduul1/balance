/*******************************(C) COPYRIGHT 2007 INSEM Inc.****************************************/
/* processor 	  : CORETEX-M3(STM32F10X)         		    				    */
/* compiler       : EWARM Compiler								    */
/* program by	  :  								    */
/* History:											    */
/* 04/13/2007     : Version 1.0									    */
/* copy right	  : Insem Inc.									    */
/****************************************************************************************************/

#define ADC1_DR_Address ((u32)0x4001244C)

u16  AD_READ(u8 Channel);