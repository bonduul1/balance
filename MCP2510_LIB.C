/*================ COPYRIGHT 2010 (주)기원전자 기술연구소  =========================

        화  일  명 : MCP2510_LIB.C
	프로그래명 : CAN제어 PRG
	작 성 자   : 박 강 호
	도    구   :CORETEX-M3(STM32F103V16)
                    EWARM Compiler 5.4Vre
       MCP2510.제어 코드 .

   2011.06.13. atmel code를 변환 시킴.. 
      ( 인터럽트 발생시 에러 발생되면 CHIP RSET 되도록 추가..)
   2011.06.14.
     ** CAN데이타 발송중 CAN DATA수신 인터럽트 발생되면 발송 완료후 처리 하도록 프로그램 수정.
     ** 주의 : 수신 인터럽트 발생시 사용 CPU 포트와 일반 IO제어 IO 포트와 나누어 사용 하도록.
               자주 인터럽트 발생되어 동작이 잘 안됨..
*****************************************************************************************************/

#include <stdio.h>
//#include <string.h>
//#include <stdlib.h>

#include "bit_define.h"
#include "stm32f10x_init.h"

#include "MCP2510_LIB.H"


extern void CAN_Ext_Tx_nByte(u32 ExtId,u8 *pBuff,u8 nByte);

u8    MCP_CAN_Readdata[8];
u8    MCP_CAN_Writedata[8];
u32   MCP_Read_id_ING;
u8    MCP_Read_dlc_ING;
u16   MCP_Read_flag_ING;  


bool  MCP_SEND_Busy;    // 발송 처리중 flag
bool  MCP_Reception_Flag;  // 발송중 수신 flag on되었으면 인터럽트 처리후 실행.
bool  MCP_Transmit_Busy;    // 발송   발신 flag on되었으면 인터럽트 처리후 실행. 다시 발송..

#define   VAC_MCP_Acceptance_Max    3        // 총 CAN DATA 수 3개. 

const u32 MCP_Acceptance_ID[VAC_MCP_Acceptance_Max] ={  // 접수 CAN ID.
  0x0CFF0027, 0x18E00021, 0x18FF6121
};


// ;***************25Cxxx command definitions
#define   WREN  6                 //write enable latch
#define   WRDI  4                 //reset the write enable latch
#define   RDSR  5                 //read status register
#define   WRSR  1                 //write status register
#define   READ  3                 //read data from memory
#define   WRITE 2                 //write data to memory

//    ; Bit defines within status register
#define   WIP   0                 //write in progress 
#define   WEL   1                 //write enable latch 
#define   BP0   2                 //block protection bit 
#define   BP1   3                 //block protection bit 

// ***************** MCP Commands on SPI ****************
#define  RESET      0xc0        // Reset internal registers to default state, set conf mode
#define  RTS        0x80        // Trigg transmission
#define  RD_STAT    0xA0        // Start reading status
#define  BIT_MOD    0x05        // Bit modify command data == MASK, BITS
//#define  READ       0x03        // Read data from memory
//#define  WRITE      0x02        // Write data to memory

#define TX0RTS      0x01        // Bit flags in the RTS command
#define TX1RTS      0x02
#define TX2RTS      0x04


#define MEMBLOCK1 0x01
#define MEMBLOCK2 0x02
#define MEMBLOCK3 0x04
#define MEMBLOCK4 0x08

/* This function sets the SPI unit to communicate with Memory and MCP2510 */
#define CKP_high        0x10
#define SSPEN           0x20
#define SPI_master_F4   0x00
#define SPI_master_F16  0x01
#define SPI_master_F64  0x02
#define SPI_master_TMR2 0x03

#define SPI_OK          0
#define SPI_NOT_FREE    1
#define SPI_OVERFLOW    2
#define SPI_COLLISION   3       

/*
** Register offsets into the transmit buffers.
*/
#define TXBnCTRL        0
#define TXBnSIDH        1
#define TXBnSIDL        2
#define TXBnEID8        3
#define TXBnEID0        4
#define TXBnDLC        5
#define TXBnD0          6
#define TXBnD1          7
#define TXBnD2          8
#define TXBnD3          9
#define TXBnD4          10
#define TXBnD5          11
#define TXBnD6          12
#define TXBnD7          13
#define CANSTAT         14
#define CANCTRL         15

#define SIDH        0
#define SIDL        1
#define EID8        2
#define EID0        3

/*
** Register offsets into the receive buffers.
*/
#define RXBnCTRL        0
#define RXBnSIDH        1
#define RXBnSIDL        2
#define RXBnEID8        3
#define RXBnEID0        4
#define RXBnDLC         5
#define RXBnD0          6
#define RXBnD1          7
#define RXBnD2          8
#define RXBnD3          9
#define RXBnD4          10
#define RXBnD5          11
#define RXBnD6          12
#define RXBnD7          13

/*
** Bits in the TXBnCTRL registers.
*/
#define TXB_TXBUFE_M    0x80
#define TXB_ABTF_M      0x40
#define TXB_MLOA_M      0x20
#define TXB_TXERR_M     0x10
#define TXB_TXREQ_M     0x08
#define TXB_TXIE_M      0x04
#define TXB_TXP10_M     0x03

#define DLC_MASK        0x0F
#define RTR_MASK        0x40


#define TXB0CTRL        0x30
#define TXB0SIDH        0x31

#define TXB1CTRL        0x40
#define TXB1SIDH        0x41

#define TXB2CTRL        0x50
#define TXB2SIDH        0x51

#define TXPRIOHIGH      0x03
#define TXPRIOHIGHLOW   0x02
#define TXPRIOLOWHIGH   0x01
#define TXPRIOLOW       0x00

#define TXB_EXIDE_M     0x08    // In TXBnSIDL
#define TXB_RTR_M       0x40    // In TXBnDLC

#define RXB_IDE_M       0x08    // In RXBnSIDL
#define RXB_RTR_M       0x40    // In RXBnDLC

#define BFPCTRL         0x0C

#define B2RTS           0x20
#define B1RTS           0x10
#define B0RTS           0x08
#define B2RTSM          0x04
#define B1RTSM          0x02
#define B0RTSM          0x01

#define TEC             0x1C
#define REC             0x1D
#define CLKCTRL         CANCTRL

#define RXF0SIDH        0
#define RXF0SIDL        1
#define RXF0EID8        2
#define RXF0EID0        3
#define RXF1SIDH        4
#define RXF1SIDL        5
#define RXF1EID8        6
#define RXF1EID0        7
#define RXF2SIDH        8
#define RXF2SIDL        9
#define RXF2EID8        10
#define RXF2EID0        11

#define RXF3SIDH        16
#define RXF3SIDL        17
#define RXF3EID8        18
#define RXF3EID0        19
#define RXF4SIDH        20
#define RXF4SIDL        21
#define RXF4EID8        22
#define RXF4EID0        23
#define RXF5SIDH        24
#define RXF5SIDL        25
#define RXF5EID8        26
#define RXF5EID0        27

#define RXF_EXIDE_M     0x08

#define RXM0SIDH        0x20
#define RXM1SIDH        0x24
#define CNF3            0x28
#define CNF2            0x29
#define CNF1            0x2A    // 
#define CANINTE         0x2B
#define CANINTF         0x2C
#define EFLG            0x2D
#define TXRTSCTRL       0x0D

#define EFLG_RX1OVR     0x80
#define EFLG_RX0OVR     0x40
#define EFLG_TXBO       0x20
#define EFLG_TXEP       0x10
#define EFLG_RXEP       0x08
#define EFLG_TXWAR      0x04
#define EFLG_RXWAR      0x02
#define EFLG_EWARN      0x01

#define SJW1            0x00
#define SJW2            0x40
#define SJW3            0x80
#define SJW4            0xC0

#define BTLMODE_CNF3    0x80

#define SAMP1           0x00
#define SAMP3           0x40

#define SEG1            0x00
#define SEG2            0x01
#define SEG3            0x02
#define SEG4            0x03
#define SEG5            0x04
#define SEG6            0x05
#define SEG7            0x06
#define SEG8            0x07

#define BRP1            0x00
#define BRP2            0x01
#define BRP3            0x02
#define BRP4            0x03
#define BRP5            0x04
#define BRP6            0x05
#define BRP7            0x06
#define BRP8            0x07

#define IVRIE           0x80
#define WAKIE           0x40
#define ERRIE           0x20
#define TX2IE           0x10
#define TX1IE           0x08
#define TX0IE           0x04
#define RX1IE           0x02
#define RX0IE           0x01
#define NO_IE           0x00

#define IVRINT          0x80
#define WAKINT          0x40
#define ERRINT          0x20
#define TX2INT          0x10
#define TX1INT          0x08
#define TX0INT          0x04
#define RX1INT          0x02
#define RX0INT          0x01
#define NO_INT          0x00

#define RXB0CTRL        0x60
#define RXB1CTRL        0x70

#define RXB_RXRDY       0x80
#define RXB_RXM1        0x40
#define RXB_RXM0        0x20
#define RXB_RX_ANY      0x60
#define RXB_RX_EXT      0x40
#define RXB_RX_STD      0x20
#define RXB_RX_STDEXT   0x00
#define RXB_RXMx_M      0x60
// #define RXB_RXIE_M      0x10
#define RXB_RXRTR       0x08    // In RXBnCTRL
#define RXB_BUKT        0x04
#define RXB_BUKT_RO     0x02

#define RXB_FILHIT      0x01
#define RXB_FILHIT2     0x04
#define RXB_FILHIT1     0x02
#define RXB_FILHIT_M    0x07
#define RXB_RXF5        0x05
#define RXB_RXF4        0x04
#define RXB_RXF3        0x03
#define RXB_RXF2        0x02
#define RXB_RXF1        0x01
#define RXB_RXF0        0x00

#define CLKEN           0x04

#define CLK1            0x00
#define CLK2            0x01
#define CLK4            0x02
#define CLK8            0x03

#define MODE_NORMAL     0x00
#define MODE_SLEEP      0x20
#define MODE_LOOPBACK   0x40
#define MODE_LISTENONLY 0x60
#define MODE_CONFIG     0xE0
#define ABORT           0x10

#define RECEIVE_BUFFER(x)   (0x60 + 0x10*(x))
#define TRANSMIT_BUFFER(x)  (0x30 + 0x10*(x))



#define canMSG_RTR              0x0001      // Message is a remote request
#define canMSG_STD              0x0002      // Message has a standard ID
#define canMSG_EXT              0x0004      // Message has a extended ID

#define CAN_ERROR         0x80              // can error..

u8 S_CNF1, S_CNF2, S_CNF3;

void MCP_DelayNop(void) {
u8 i;
	for(i=0;i<15;i++); //1us Delay at 36MHz
}


//clock generate
void CAN_sck(void)
{
   MCP_SCK = 1;
   MCP_DelayNop();
   MCP_SCK = 0;
// MCP_DelayNop();
}

void    SPI_init_hw(void)
{
//  DDRB  = 0B10111111;           // 1이면출력..( SS_0,SCK_1,MOSI_3 출력)
//  PORTB = 0XFF;	// PORT B

// SPI initialization
// SPI Type: Master
// SPI Clock Rate:  
// SPI Clock Phase: Cycle Half
// SPI Clock Polarity: Low
// SPI Data Order: MSB First

//SPCR=0x51;
//SPSR=0x00;

    MCP_CS = 1;
    MCP_CKP = 0;
    MCP_RESET = 0;    // CHIP RESET...
    MCP_DelayNop();
    MCP_DelayNop();
    MCP_DelayNop();
    MCP_RESET = 1;
    MCP_DelayNop();

}
u8 SPI_putch(u8 outdata)
{
  u8  count_bit;
  u8  SPDR;
  u16 input;
  
// SPDR = outdata;                      // 발송 데이타를 적재..
// while(!((SPSR & (1<<7))));
// return SPDR;                         // 입력도 동시에 처리된다..(입력시에는 출력데이타는 쓰레기 값이 된다...좀이해 하기 힘들겠지만.^^;;
  
  SPDR = 0x00;
  for(count_bit = 0; count_bit < 8; ++count_bit) {
    if( outdata & 0x80) MCP_MOSI = 1;
    else                MCP_MOSI = 0;
    
    outdata <<= 1;
    SPDR <<= 1;
    if ( MCP_MISO )     SPDR |= 0x01;
    CAN_sck();
  }
  return SPDR;   
}

// Selects the MCP2510.
// Note: MCP_CS is set to zero also by SPI_MCP_RD_address(),
// SPI_MCP_WR_address() and SPI_MCP_RD_status().

void SPI_MCP_select (void)
{
    MCP_CS = 0;
    MCP_CKP = 0;
    //  PORTB = 0X00;
}

// To avoid a bug, SCK must be low when CS is raising. But thing doesn't work
// if MCP_CKP remains low, so we set it to 1 as soon as CS is 1.
void SPI_MCP_unselect (void)
{
     MCP_CKP = 0;
     MCP_DelayNop();
     MCP_CS = 1; 
     MCP_DelayNop();
     MCP_CKP = 1;
}

void SPI_MCP_reset ( void )
{
    SPI_MCP_select();
    SPI_putch(RESET);
    SPI_MCP_unselect();
}

void SPI_MCP_RD_address(u8 output)
{
    u8 store;
    MCP_CS=0;              // Select the memory
    store = SPI_putch( READ );       // Write the command
    // store = SPI_putch( 0x00 );    // Write high byte in address
    store = SPI_putch( output );     // Write low byte in address 
}

void SPI_MCP_WR_address(u8 output)
{
    u8 store;
    MCP_CS=0;              // Select the memory
    store = SPI_putch( WRITE );    // Write the command
    // store = SPI_putch( 0x00 );    // Write high byte in address
    store = SPI_putch( output );    // Write low byte in address 
}

u8 SPI_MCP_RD_status ( void )
{
    u8 store;
    MCP_CS=0;              // Select the memory
    store =  SPI_putch( RD_STAT );   // Write the command
    store =  SPI_putch( RD_STAT );   // Write any byte to get a byte in return 
    // MCP_CKP = 0;
    // memCS=1;              // deselect the memory to effectuate the command
    // MCP_CKP = 1;
    return store;        // Command done OK
} 

void SPI_MCP_write_bits( u8 MCPaddr, u8 data, u8 mask )
{
    SPI_MCP_select ();
    SPI_putch( BIT_MOD );
    SPI_putch( MCPaddr );
    SPI_putch( mask  );
    SPI_putch( data );
    SPI_MCP_unselect ();
}
/*
 ** Read one or more registers in the MCP2510, starting at address
 ** readdata.
 */
void MCP_read( u8 MCPaddr, u8* readdata, u8 length )
{
    u8 loopCnt;
    SPI_MCP_select(); // Select the MCP device at the SPI bus
    // Start reading and set first address
    SPI_MCP_RD_address(MCPaddr);

    for (loopCnt=0; loopCnt < length; loopCnt++) {
        // Get a byte and store at pointer
        *readdata  = SPI_putch(MCPaddr);
        // Increment the pointers to next location
        // Test++;
        MCPaddr++;
        readdata++;
    }

    SPI_MCP_unselect();
}
/*
 ** Write to one or more registers in the MCP2510, starting at address
 ** writedata.
 */
void MCP_write( u8 MCPaddr, const u8* writedata, u8 length )
{
    u8 loopCnt;
    SPI_MCP_select();
    // Start write and set first address
    SPI_MCP_WR_address( MCPaddr );
    for (loopCnt=0; loopCnt < length; loopCnt++) {
        // Write a byte
        SPI_putch( *writedata  );
        // Increment the pointer to next location
        writedata++;
    }
    SPI_MCP_unselect();
}

/*
 ** Start the transmission from one of the tx buffers.
*/
void MCP_transmit(u8 MCP_addr)        // addr 30, 40, 50
{
    if ( MCP_addr == 1 )    MCP_addr = 0x30;    // 채널을 주소로 변환..
    if ( MCP_addr == 2 )    MCP_addr = 0x40;
    if ( MCP_addr == 3 )    MCP_addr = 0x50;
    
    SPI_MCP_write_bits(MCP_addr, TXB_TXREQ_M, TXB_TXREQ_M);
    //  u8 data;
    //  MCP_read( MCP_addr, &data, 1);
    //  data |= TXB_TXREQ_M;
    //  MCP_write( MCP_addr, &data, 1);
}

void MCP_read_can_id( u8 MCP_addr, u8* ext, u32* can_id )
{
    u8 tbufdata[4];
    *ext = 0;
    *can_id = 0;
    MCP_read( MCP_addr, tbufdata, 4);
    *can_id = (u32)((u16)tbufdata[SIDH]<<3) + (tbufdata[SIDL]>>5);
    if ( (tbufdata[SIDL] & TXB_EXIDE_M) ==  TXB_EXIDE_M ) {
        *can_id = (*can_id<<2) + (tbufdata[SIDL] & 0x03);
        *can_id <<= 16;
        *can_id = *can_id +((u32)tbufdata[EID8]<<8) + (u32)tbufdata[EID0];
        *ext = 1;
    }
}

// Buffer can be 4..5
void MCP_read_can( u8 buffer, u8* ext, u32* can_id,
                   u8* dlc, u8* rtr, u8* data )
{

    u8 MCP_addr , ctrl;
    
    MCP_addr = buffer*16 + 0x21;
    
    MCP_read_can_id( MCP_addr, ext, can_id );
    MCP_read( MCP_addr-1, &ctrl, 1 );
    MCP_read( MCP_addr+4, dlc, 1 );
    if (/*(*dlc & RTR_MASK) || */(ctrl & 0x08)) {
        *rtr = 1;
    } else {
        *rtr = 0;
    }
    *dlc &= DLC_MASK;
    MCP_read( MCP_addr+5, data, *dlc );
}


void MCP_write_can_id( u8 MCP_addr, u8 ext, u32 can_id )
{
    u16 canid;
    u8 tbufdata[4];
    canid = (u16)(can_id & 0x0FFFF);
    if ( ext == 1) {
        tbufdata[EID0] = (u8) (canid & 0xFF);
        tbufdata[EID8] = (u8) (canid / 256);
        canid = (u16)( can_id / 0x10000L );
        tbufdata[SIDL] = (u8) (canid & 0x03);
        tbufdata[SIDL] += (u8) ((canid & 0x1C )*8);
        tbufdata[SIDL] |= TXB_EXIDE_M;
        tbufdata[SIDH] = (u8) (canid / 32 );
    }
    else {
        tbufdata[SIDH] = (u8) (canid / 8 );
        tbufdata[SIDL] = (u8) ((canid & 0x07 )*32);
        tbufdata[EID0] = 0;
        tbufdata[EID8] = 0;
    }
    MCP_write( MCP_addr, tbufdata, 4 );
}


void MCP_write_can( u8 buffer, u8 ext, u32 can_id,
                    u8 dlc, u8 rtr, const u8* data )
{
    u8 MCP_addr;
    
    MCP_addr = buffer*16 + 0x21; 
    
    MCP_write(MCP_addr+5, data, dlc );  // write data bytes
    MCP_write_can_id( MCP_addr, ext, can_id );  // write CAN id
    if ( rtr == 1)  dlc |= RTR_MASK;  // if RTR set bit in byte
    MCP_write((MCP_addr+4), &dlc, 1 );            // write the RTR and DLC
}

/*
 ** Read all registers in the MCP2510 and print them in tabular form.
 */
void MCP_read_all ( void )
{
    u8 i, j;
    u8 Store;

//    printf("Reading all registers in the MCP2510...\n\r" );

    SPI_MCP_RD_address(0); // Makes a SPI_MCP_select()

    for (j =0; j < 8 ; j++ ) {
//        printf("Address %02X to %02X: ", (j*16), (j*16+15) );
        for (i=0; i < 0x10 ; i++ ) {
            Store = SPI_putch(i);
//            printf("%02X ", Store );
        }
//        printf("\n\r");
    }

    SPI_MCP_unselect();
    Store = SPI_MCP_RD_status(); // Makes a SPI_MCP_select()
    SPI_MCP_unselect();
//    printf("Status = 0x%02x\n\r", Store );
}


/*
 ** Read one of the CAN transmit or receive buffers in the MCP2510.
 ** The result is printed.
 */
void MCP_read_buffer( u8 buffer )
{
    u8 loopCnt, dlc, rtr, ext;
    u8 databytes[8];
    u32 can_id;
    MCP_read_can ( buffer, &ext, &can_id, &dlc, &rtr, databytes );
/*    
    if( ext == 1 ) {
        printf("CAN Id = 0x%08lX (Extended)", can_id);
    } else {
        printf("CAN Id = 0x%04X (Standard)", (u16) can_id );
    }
    printf(", with dlc=%d and RTR=%d\n\r", dlc, rtr );
    if (buffer > 3) {
        printf("Data in receive buffer %d is:", buffer-3 );
    } else {
        printf("Data in transmit buffer %d is:", buffer );
    }
    for ( loopCnt=0 ; loopCnt < dlc ; loopCnt++ ) {
        printf(" %02X", databytes[loopCnt] );
    }
    printf("\n\r"); 
*/    
}


/*
 ** Read one of the transmit buffers in the MCP2510.
 */
int MCP_read_tbuf ( u8 buffer )
{
    if ( (buffer < 1) || (buffer > 3) ) {
//        printf("The only buffers supported are 1,2 and 3, not %d\n\r", buffer );
        return 255;
    }
    MCP_read_buffer( buffer );
    return 0;
}

/*
 ** Read one of the receive buffers in the MCP2510.
 */
int MCP_read_rbuf ( u8 buffer )
{
    if ( (buffer < 1) || (buffer > 2) ) {
//        printf("The only receive buffers supported are 1 and 2, not %d\n\r",
//               buffer );
        return 255;
    }
    MCP_read_buffer(buffer + 3);
    return 0;
}


void MCP_reset(void)
{
    SPI_MCP_reset();
}

void  SET_INIT_DATA(u8 R_CNF1, u8 R_CNF2, u8 R_CNF3 )
{
    u8 data;

    data = R_CNF1;
    MCP_write(CNF1, &data, 1);
    data=  R_CNF2;
    MCP_write(CNF2, &data, 1);
    data = R_CNF3; 
    MCP_write(CNF3, &data, 1);
}

void  SET_INIT_SET(void)
{
    u8 data;

    data = SJW1 + BRP1;
    MCP_write(CNF1, &data, 1);
    data=(BTLMODE_CNF3+SEG4*8+SEG7);  // Phase Seg 1 = 4, Prop Seg = 7    
    MCP_write(CNF2, &data, 1);
    data = SEG4; // Phase Seg 2 = 4
    MCP_write(CNF3, &data, 1);
}



/*
 ** Initialize the MCP2510.
 */
void MCP_init(void)
{
    u8 data;
    u8 i,j,a;

    data = MODE_CONFIG;
    // Go into configuration mode
    MCP_write(CANCTRL, &data, 1);

    //
    // Bit rate calculations.
    //
    // In this case, we'll use a speed of 125 kbit/s.
    // If we set the length of the propagation segment to 7 bit time quanta,
    // and we set both the phase segments to 4 quanta each,
    // one bit will be 1+7+4+4 = 16 quanta in length.
    //
    // If you have a 4 MHz clock, setting the prescaler (BRP) to 1
    // gives a bit length of 16 * 0.5 us = 8 us => 125 kbit/s.
    //
    // If you have a 16 MHz clock, setting the prescaler (BRP) to 4
    // gives the same result.
    //

    data = SJW1 + BRP1;
    MCP_write(CNF1, &data, 1);

    data=(BTLMODE_CNF3+SEG4*8+SEG7);  // Phase Seg 1 = 4, Prop Seg = 7    
    MCP_write(CNF2, &data, 1);

    data = SEG4; // Phase Seg 2 = 4
    MCP_write(CNF3, &data, 1);

    // Disable interrups.
    data = NO_IE;
//  data = 0x01;                //인터럽트 지정(RX0 Enable)...
    MCP_write(CANINTE, &data, 1);

    // Mark all filter bits as don't care:
//    MCP_write_can_id(RXM0SIDH, 1, 0);  // 2011.06.29. 수신되지 않도록 수정.
//    MCP_write_can_id(RXM1SIDH, 1, 0);
    MCP_write_can_id(RXM0SIDH, 1, 0xffffffff);  // 2011.06.29. 수신되지 않도록 수정.
    MCP_write_can_id(RXM1SIDH, 1, 0xffffffff);
    // Anyway, set all filters to 0:
    MCP_write_can_id(RXF0SIDH, 0, 0);
    MCP_write_can_id(RXF1SIDH, 0, 0);
    MCP_write_can_id(RXF2SIDH, 0, 0);
    MCP_write_can_id(RXF3SIDH, 0, 0);
    MCP_write_can_id(RXF4SIDH, 0, 0);
    MCP_write_can_id(RXF5SIDH, 0, 0);


    data = MODE_NORMAL + CLKEN + CLK2;  // 외부 클럭 출력 on, 1/8 CLK
    MCP_write(CLKCTRL, &data, 1);

    // Clear, deactivate the three transmit buffers
    data = 0;
    a = TXB0CTRL;
    for (i = 0; i < 3; i++) {
        for (j = 0; j < 14; j++) {
            MCP_write(a, &data, 1);
            a++;
        }
        a += 2; // We did not clear CANSTAT or CANCTRL
    }

    // and the two receive buffers.
//    data = RXB_RX_ANY;                 // 모든 데이타 수신..
    data = 0x00;
    MCP_write(RXB0CTRL, &data, 1);
    data = 0x00;
    MCP_write(RXB1CTRL, &data, 1);
}

void MCP_init_ports(void)
{
    // Set RTS pins as inputs (used for the buttons)
    SPI_MCP_write_bits(TXRTSCTRL, 0 , 0xFF);            //  
    // The two pins RX0BF and RX1BF are used to control two LEDs; set them as outputs and
    // set them as 00.
    SPI_MCP_write_bits(BFPCTRL, 0x3C, 0xFF );           // 인터럽트 Rx0,Rx1 정의..
}

void MCP_setupModule(void) {

//  printf("Module started\n\r");

    SPI_init_hw();
    MCP_reset();
    MCP_init();
    MCP_init_ports();
    MCP_Transmit_Busy=0;
    MCP_SEND_Busy= 0;
    MCP_Reception_Flag=0;  // 발송중 수신 flag on되었으면 인터럽트 처리후 실행.
}
    
// Test if there is a message pending; if so, read it.
// We check RXB0 first, and if it is empty, RXB1.
// This simple approach may cause the order to be wrong.
// A better way would be to check RXB1 after a message was read from RXB0, and if there is
// a message in RXB1 mark a flag. If on the next call this flag is set, read RXB1 instead of
// RXB0. We know that there will always ne a message in RXB0 before one ends up in RXB1.
//
// Return values: canOK         There was a message
//                canERR_NOMSG  Nothing to be read

//BANK1 static u8 crReadB1 = 0;
u8 crReadB1 = 0;     // 인터럽트가 동시에 걸릴 수 있기 때문에 읽은 인터럽트 저장 변수..
u8 MCP_canRead(u32 *id, u8 *msg,  u8 *dlc, u16 *flag) 
{
    u8 msgF, extF;
    u8 byte, rtr;
    u8 i,Chk;
    u8 Err1, Err2;
    static u8 Error_Count = 0;

    msgF = 0;
 
    MCP_read(CANINTF, &byte, 1);
    
    
    if ( byte & CAN_ERROR )  {

       MCP_read(CANINTE, &Err1, 1);
       MCP_read(EFLG   , &Err2, 1);
       MCP_setupModule();       // 인터럽트 에러 발생..( Overflow...)
       Error_Count++;           // CAN 에러 카운트..

       MCP_CAN_Writedata[0] = 0x00;  
       MCP_CAN_Writedata[1] = 0xFF;
       MCP_CAN_Writedata[2] = 0x00;
       MCP_CAN_Writedata[3] = 0X00;
       MCP_CAN_Writedata[4]=  byte;
       MCP_CAN_Writedata[5] = Err1;                  //에러 내용1.  CANINTE
       MCP_CAN_Writedata[6] = Err2;                  //에러 내용2.  EFLG
       MCP_CAN_Writedata[7] = Error_Count;           //에러 카운트.
   
       MCP_DelayNop();                          // DELAY..
       MCP_DelayNop();                          // DELAY..
       
       CAN_Ext_Tx_nByte(VAC_ID_ERROR,MCP_CAN_Writedata,8); // 발송 주소 0x19FFA800
       MCP_write_can (0x01, 0x01,VAC_ID_ERROR,8,0, MCP_CAN_Writedata); // 주소, CAN2.0B,...
       MCP_transmit(1);    // 데이타 1번 채널 .발송  */
       return;
    }
    
    
    if (crReadB1) 
    {
        if (byte & RX1INT) { 
            MCP_read_can(5, &extF, id, dlc, &rtr, msg);
            SPI_MCP_write_bits(CANINTF, ~RX1INT, RX1INT); // Clear interrupt
            msgF = 1;
        } //else
        //  printf("<CRerr>\n");
        crReadB1 = 0;
    }
 
    if (byte & RX0INT) 
    {
        MCP_read_can(4, &extF, id, dlc, &rtr, msg);
        SPI_MCP_write_bits(CANINTF, ~RX0INT, RX0INT); // Clear interrupt
        msgF = 1;
        MCP_read(CANINTF, &byte, 1);
        if (byte & RX1INT)
            crReadB1 = 1;
    }

    if (msgF) {
        if (extF)
            *flag = canMSG_EXT;
        else
            *flag = canMSG_STD;
        if (rtr) {
            //    printf("<RTR>\n\r");
            *flag |= canMSG_RTR;
        }

        Chk = 0;
        for ( i=0; i < VAC_MCP_Acceptance_Max; i++ ) {    // 등록된 ID인지 확인 한다..
          if (  *id == MCP_Acceptance_ID[i] )  { 
            Chk = 1;    // 등록 ID.. CHECK..
            break;
          }
        }
       
        if ( Chk ==1 )   {
           CAN_Ext_Tx_nByte(*id, msg ,*dlc); // 발송 주소 0x19FFA800
        }
        return 1;
    } else   return 0;
}

// Not implemented
//void canSetCommMode(commModesT commMode) {
//}
    

/*================ COPYRIGHT 2010 (주)기원전자 기술연구소  =========================*/