/************************************** COPYRIGHT 2011 (주)기원전자 기술연구소************************************

	화  일  명 : 24LC02
	프로그래명 : \EEPROM 구동 프로그램.
	작 성 자   : 박 강 호
	도    구   :
                    COMPILER : IAR ARM COMFILER

******************************************************/

#define __24C01_H__

#include "stm32f10x_lib.h"

extern void Delay_1ms(u16 Dly);
extern void  WATCH_DOG(void);
void i2c_bytewrite (u8 StartAddress, u8 Data);
u8 i2c_byteread(u8 StartAddress) ;

#define EEPROM_CHIP_ADDRESS     0xA0
//#include "main.h"


void    EEPROM_WRITE( u8 StartAddress, u8 Data)
{
     i2c_bytewrite (  StartAddress, Data);
}

u8    EEPROM_READ( u8 StartAddress)
{
     return i2c_byteread( StartAddress);
}


/***************************************************************************************************/
/* M24LC16_ByteWrite	:									   */												
/***************************************************************************************************/
void i2c_bytewrite (u8 StartAddress, u8 Data) 
{
  I2C_Cmd(I2C1, DISABLE);  
  /* Clear EV6 by setting again the PE bit */
  I2C_Cmd(I2C1, ENABLE);  
  
  /* Send STRAT condition */
  I2C_GenerateSTART(I2C1, ENABLE);
  
  /* Test on EV5 and clear it */
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));  
  
  /* Send EEPROM address for write */
  I2C_Send7bitAddress(I2C1, EEPROM_CHIP_ADDRESS, I2C_Direction_Transmitter);
  
  /* Test on EV6 and clear it */
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
      
  /* Send the EEPROM's internal address to write to */
  I2C_SendData(I2C1, (u8)StartAddress);
  
  /* Test on EV8 and clear it */
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

  /* Send the byte to be written */
  I2C_SendData(I2C1, Data); 
   
  /* Test on EV8 and clear it */
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
  
  /* Send STOP condition */
  I2C_GenerateSTOP(I2C1, ENABLE);
  
  /* Clear EV6 by setting again the PE bit */
  //I2C_Cmd(I2C1, DISABLE);  

}
/***************************************************************************************************/
/* M24LC16_ByteRead	:									   */												
/***************************************************************************************************/
u8 i2c_byteread(u8 StartAddress) {
  unsigned char Data;

  /* Clear EV6 by setting again the PE bit */
  I2C_Cmd(I2C1, DISABLE);  
  I2C_Cmd(I2C1, ENABLE);  
  
  while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));

   /* Send START condition */
   I2C_GenerateSTART(I2C1, ENABLE);
   
   /* Test on EV5 and clear it */
   while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));

   I2C_AcknowledgeConfig(I2C1, DISABLE);
   
   /* Send EEPROM address for write */
   I2C_Send7bitAddress(I2C1, EEPROM_CHIP_ADDRESS, I2C_Direction_Transmitter);

   /* Test on EV6 and clear it */
   while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

   /* Clear EV6 by setting again the PE bit */
   I2C_Cmd(I2C1, ENABLE);

   /* Send the EEPROM's internal address to write to */
   I2C_SendData(I2C1, (u8)StartAddress);

   /* Test on EV8 and clear it */
   while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

   /* Send STRAT condition a second time */
   I2C_GenerateSTART(I2C1, ENABLE);

   /* Test on EV5 and clear it */
   while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));

   /* Send EEPROM address for read */
   I2C_Send7bitAddress(I2C1, EEPROM_CHIP_ADDRESS | 0x01, I2C_Direction_Receiver);

   /* Test on EV6 and clear it */
   while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));

   while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED));
   
//  I2C_AcknowledgeConfig(I2C1, DISABLE);
  I2C_GenerateSTOP(I2C1, ENABLE);

  
  Data=I2C_ReceiveData(I2C1);

  //I2C_Cmd(I2C1, DISABLE);

return Data;

 
}

/***************************************************************************************************/
/* M24LC16_WordWrite	:									   */												
/***************************************************************************************************/
void i2c_wordwrite (u8 StartAddress, u16 Data) {
  u8 Temp;
  u8 ii;
  
  for ( ii=0; ii < 10;ii++)  { 
    Delay_1ms(1);             // Delay 타임 없이 바로 i2c읽으면 에러 발생됨..  
    WATCH_DOG();
  }
  Temp = Data&0x00ff;
  i2c_bytewrite (StartAddress,   Temp);
  for ( ii=0; ii < 10;ii++)  { 
    Delay_1ms(1);             // Delay 타임 없이 바로 i2c읽으면 에러 발생됨..  
    WATCH_DOG();
  }
  Temp = (u8)(Data>>8);   
  i2c_bytewrite ( StartAddress+1, Temp);
   
}

/***************************************************************************************************/
/* M24LC16_WordRead	:									   */												
/***************************************************************************************************/
u16 i2c_wordread( u8 StartAddress) {
 u16 Temp1, Temp2;
  u8 ii;
 
  for ( ii=0; ii < 10;ii++)  { 
    Delay_1ms(1);             // Delay 타임 없이 바로 i2c읽으면 에러 발생됨..  
    WATCH_DOG();
  }
 Temp1 = i2c_byteread( StartAddress) ;

 for ( ii=0; ii < 10;ii++)  { 
    Delay_1ms(1);             // Delay 타임 없이 바로 i2c읽으면 에러 발생됨..  
    WATCH_DOG();
  }
 Temp2= i2c_byteread( StartAddress+1);
 Temp1 = (Temp2<<8) + Temp1;
 return Temp1;
}
/************************************** COPYRIGHT 2011 (주)기원전자 기술연구소*************************************/
