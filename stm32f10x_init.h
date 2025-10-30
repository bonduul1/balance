/*******************************(C) COPYRIGHT 2007 INSEM Inc.****************************************/
/* processor 	  : CORETEX-M3(STM32F10X)         		    				    */
/* compiler       : EWARM Compiler								    */
/* program by	  :  								    */
/* History:											    */
/* 04/13/2007     : Version 1.0									    */
/* copy right	  : Insem Inc.									    */
/****************************************************************************************************/
void Internal_timer_Proc(void);
void timer_setup(uchar tmno, ushort timer);
void CORTEX_initial(void);

//#define STM32F101_ENABLE
//#define STM32F102_ENABLE
#define STM32F103_ENABLE

/* GPIOB offset */
#define GPIOB_OFFSET                 (GPIOB_BASE - PERIPH_BASE)

/* GPIOB BSRR offset */
#define GPIOB_BSRR_OFFSET                 (GPIOB_OFFSET + 0x10)

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define LAB1_GPIO_ODR         // used for odr register
//#define LAB1_BIT_BANDING      // used for bit banding

#ifdef LAB1_BIT_BANDING
/* define the bit set number for PB0.5, PB0.6, PB.07, PB.08 & PB.09 in the BSRR*/
  #define GPIOB_Bit5_SET  ((u8)0x05)
  #define GPIOB_Bit6_SET  ((u8)0x06)
  #define GPIOB_Bit7_SET  ((u8)0x07)
  #define GPIOB_Bit8_SET  ((u8)0x08)
  #define GPIOB_Bit9_SET  ((u8)0x09)

  /* define the bit set address in the alias region for PB0.5, PB0.6, PB.07, PB.08 & PB.09*/
  #define GPIOB_Bit5_SET_BB  (PERIPH_BB_BASE + (GPIOB_BSRR_OFFSET * 32) + (GPIOB_Bit5_SET * 4)) //0x42218214(virtual addr)
  #define GPIOB_Bit6_SET_BB  (PERIPH_BB_BASE + (GPIOB_BSRR_OFFSET * 32) + (GPIOB_Bit6_SET * 4)) //0x42218218(virtual addr)
  #define GPIOB_Bit7_SET_BB  (PERIPH_BB_BASE + (GPIOB_BSRR_OFFSET * 32) + (GPIOB_Bit7_SET * 4)) //0x4221821C(virtual addr)
  #define GPIOB_Bit8_SET_BB  (PERIPH_BB_BASE + (GPIOB_BSRR_OFFSET * 32) + (GPIOB_Bit8_SET * 4)) //0x42218220(virtual addr)
  #define GPIOB_Bit9_SET_BB  (PERIPH_BB_BASE + (GPIOB_BSRR_OFFSET * 32) + (GPIOB_Bit9_SET * 4)) //0x42218224(virtual addr)
#endif

/*****************************************************************************************************/
#ifdef __STM32F10X_H__

ushort timer_buf[8];
__bits flag0;
__tm tm;	

#else

extern ushort timer_buf[8];

extern __bits flag0;
extern __tm tm;

#endif
/******************************** bit flag define ******************************************************/
#define   tm1ms_f	flag0.bit.b0

/******************* (C) COPYRIGHT 2007 INSEM Inc ***************************************END OF FILE****/

