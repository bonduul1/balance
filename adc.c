/*******************************(C) COPYRIGHT 2007 INSEM Inc.****************************************/
/* processor 	  : CORETEX-M3(STM32F10X)         		    				    */
/* compiler       : EWARM Compiler								    */
/* program by	  : Park Kangho  								    */
/* History:											    */
/* 04/13/2007     : Version 1.0									    */
/* copy right	  : Insem Inc.									    */
/****************************************************************************************************/
#define __ADC_H__

#include "stm32f10x_lib.h"
#include "main.h"

//
//  아날로그 READ..
//
u16  AD_READ(u8 Channel)
{
/*  if ( Channel == ADC_Channel_16 ) {
    ADC_RegularChannelConfig(ADC1, Channel, 1, ADC_SampleTime_239Cycles5);  
    ADC_Cmd(ADC1, ENABLE); //enable ADC1
    while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET); //wait EOC flag set
    return ADC1->DR;
  }
*/

   ADC_RegularChannelConfig(ADC2, Channel, 1, ADC_SampleTime_55Cycles5);  
   ADC_Cmd(ADC2, ENABLE); //enable ADC2
   while(ADC_GetFlagStatus(ADC2, ADC_FLAG_EOC) == RESET); //wait EOC flag set
   return ADC2->DR;
}

/*
void get_adc(void) //per25ms
{
   switch(adc_sel) {
   case 0: ADC_RegularChannelConfig(ADC1, ADC_Channel_8,  1, ADC_SampleTime_55Cycles5); break;
   case 1: ADC_RegularChannelConfig(ADC1, ADC_Channel_9,  1, ADC_SampleTime_55Cycles5); break;
   case 2: ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 1, ADC_SampleTime_55Cycles5); break;
   case 3: ADC_RegularChannelConfig(ADC1, ADC_Channel_11, 1, ADC_SampleTime_55Cycles5); break;
   case 4: ADC_RegularChannelConfig(ADC1, ADC_Channel_12, 1, ADC_SampleTime_55Cycles5); break;
   case 5: ADC_RegularChannelConfig(ADC1, ADC_Channel_13, 1, ADC_SampleTime_55Cycles5); break;
   case 6: ADC_RegularChannelConfig(ADC1, ADC_Channel_14, 1, ADC_SampleTime_55Cycles5); break;
   case 7: ADC_RegularChannelConfig(ADC1, ADC_Channel_15, 1, ADC_SampleTime_55Cycles5); break;
   }
   
   ADC_Cmd(ADC1, ENABLE); //enable ADC1
   while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET); //wait EOC flag set

//   adc_Data[0].dsp = ADC1->DR;
//   sort_ad_value =adc_Data[0].dsp ;
// adc[adc_sel].dsp = ADC1->DR;
// if(++adc_sel > 7) adc_sel=0;
}
*/