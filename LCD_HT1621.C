/*================ COPYRIGHT 2011 (林)扁盔傈磊 扁贱楷备家  =========================

        拳  老  疙 : LCD.C
	橇肺弊贰疙 : LCD HOLTEK HT1621力绢 橇肺弊伐.
	累 己 磊   : 冠 碍 龋
        档    备   :CORETEX-M3(
                    EWARM Compiler 5.4Vre

**************************************************************************************/

#define   LCD_DATA  GpioC->Bit.b6         //  Chip select
#define   LCD_WR    GpioC->Bit.b7         //  Chip select
#define   LCD_RD    GpioC->Bit.b8         //  Chip select
#define   LCD_CS    GpioC->Bit.b9         //  Chip select
   
#define   LCD_CMD_BAIS        0x29
#define   LCD_CMD_SYSEN       0x01
#define   LCD_CMD_OFF         0x02
#define   LCD_CMD_ON          0x03
#define   LCD_CMD_RC256       0x18

void delay(unsigned u16 k)
	    { 
		   unsigned u16 j,i;
		   for(i=0;i<k;i++)
		       {
			      for( j=0;j<1000;j++);
			   }
        }
//
//
//
void SendBit_1621(u8 LCD_DATAa,u8 cnt) //LCD_DATAa 的高cnt 位写入HT1621，高位在前
{
 u8 i;
 u16 j;
 for(i =0; i <cnt; i ++) {
   if((LCD_DATAa&0x80)==0) LCD_DATA=0;
   else LCD_DATA=1;
   LCD_WR=0;

   for(j=0; j<1; j++)
   //_nop();
   LCD_WR=1;
   LCD_DATAa<<=1;
 }
}

void SendDataBit_1621(u8 LCD_DATAa,u8 cnt) //LCD_DATAa 的低cnt 位写入HT1621，低位在前
{
u8 i;
u16 j;

for(i =0; i <cnt; i ++) {
  if((LCD_DATAa&0x01)==0) LCD_DATA=0;
  else LCD_DATA=1;
  LCD_WR=0;

  for(j=0; j<1; j++)
  //_nop();
  LCD_WR=1;
  LCD_DATAa>>=1;
 }
}

void SendCmd(u8 command)
{
  LCD_CS=0;
  SendBit_1621(0x80,3); //写入标志码"100"
  SendBit_1621(command,9); //写入9 位数据,其中前8 位为command 命令,最后1 位任意
  LCD_CS=1;
}

void Write_1621(u8 addr,u8 LCD_DATAa)
{
  LCD_CS=0;
  SendBit_1621(0xa0,3); //写入标志码"101"
  SendBit_1621(addr<<2,6); //写入6 位addr
  SendDataBit_1621(LCD_DATAa,4); //写入LCD_DATAa 的低4 位
  LCD_CS=1;
}

void WriteAll_1621(u8 addr,u8 *p,u8 cnt)
{
  u8 i;
  
  LCD_CS=0;
  SendBit_1621(0xa0,3); //写入标志码"101"
  SendBit_1621(addr<<2,6); //写入6 位addr
  for(i =0; i <cnt; i ++,p++) { //连续写入数据
    SendDataBit_1621(*p,8);
  } 
  LCD_CS=1;
}


//
// LCD 檬扁拳 风凭..
//
void LCD_INIT(){

  u8 a[20];

 LCD_DATA = 1;
  LCD_CS = 1;
  LCD_WR = 0;

  a[0]=0xff; a[1]=0xff; a[2]=0xff; a[3]=0xff; a[4]=0xff; a[5]=0xff; a[6]=0xff; a[7]=0xff;
  a[8]=0xff; a[9]=0xff; a[10]=0xff; a[11]=0xff; a[12]=0xff; a[13]=0xff; a[14]=0xff; 
  a[15]=0xff; a[16]=0xff;  
  
  //SendCmd(LCD_CMD_RC256); //内部RC
  SendCmd(LCD_CMD_BAIS);  //设置偏压和占空比
  SendCmd(LCD_CMD_SYSEN); //打开系统振荡器
  SendCmd(LCD_CMD_ON); //打开LCD 偏压发生器
  Write_1621(0x09,0x01); //在地址0x09 处写入数据0x01
  WriteAll_1621(0,a,5); //在起始地址为0 处连续写入5 个字节数据
  //SendCmd(LCD_CMD_OFF); //关闭LCD 显示
}
  
