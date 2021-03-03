//**************************************************************//
/*
File Name : Ra8876_Lite.cpp                                   
Author    : RAiO Application Team                             
Edit Date : 09/13/2017
Version   : v2.0  1.modify bte_DestinationMemoryStartAddr bug 
                  2.modify ra8876SdramInitial Auto_Refresh
                  3.modify ra8876PllInitial 
*/
//**************************************************************//
#include "simpletools.h"
#include "Ra8876_Lite.h"

//**************************************************************//
Ra8876_Lite::Ra8876_Lite(int xnscs, int xnreset, int clk, int data, int GT9271_INT)
{
  _xnscs = xnscs;
  _xnreset = xnreset;
  _GT9271_INT = GT9271_INT;
  _clk = clk;
  _data = data;
  sck = 1;
  miso = 3;
  mosi = 2;
}
//**************************************************************//
//**************************************************************//
bool Ra8876_Lite::begin(void)
{
  i2c_open(bus, _clk, _data, 0);
  i2c_addr = 0x5d;
  i2c_addr_write = (i2c_addr << 1) & 0b11111110;
  i2c_addr_read = (i2c_addr << 1) | 0b00000001;

  low(_xnreset);
  high(_GT9271_INT);
  pause(2);
  high(_xnreset);
  pause(20);
  set_direction(_GT9271_INT, 0);
  pause(1000);
  gt9271_Send_Cfg((uint8_t *)GTP_CFG_DATA, sizeof(GTP_CFG_DATA));
  high(8);

  high(_xnscs);
  low(sck);

  high(_xnreset);
  pause(1);
  low(_xnreset);
  pause(1);
  high(_xnreset);
  pause(10);
  //read ID code must disable pll, 01h bit7 set 0
  lcdRegDataWrite(0x01, 0x08);
  pause(100);
  if ((lcdRegDataRead(0xff) != 0x76) && (lcdRegDataRead(0xff) != 0x77))
  {
    print("RA8876 or RA8877 not found!\n");
    return false;
  }
  else
  {
    print("RA8876 or RA8877 connect pass!\n");
  }

  if (!ra8876Initialize())
  {
    print("ra8876 or RA8877 initial fail!\n");
    return false;
  }
  else
  {
    print("RA8876 or RA8877 initial Pass!\n");
  }

  return true;
}
//**************************************************************//

bool Ra8876_Lite::ra8876Initialize(void)
{
  if (!ra8876PllInitial())
  {
    print("PLL initial fail!");
    return false;
  }

  if (!ra8876SdramInitial())
  {
    print("SDRAM initial fail!");
    return false;
  }

  lcdRegWrite(RA8876_CCR); //01h
  lcdDataWrite(RA8876_PLL_ENABLE << 7 | RA8876_WAIT_NO_MASK << 6 | RA8876_KEY_SCAN_DISABLE << 5 | RA8876_TFT_OUTPUT24 << 3 | RA8876_I2C_MASTER_DISABLE << 2 | RA8876_SERIAL_IF_ENABLE << 1 | RA8876_HOST_DATA_BUS_SERIAL);

  lcdRegWrite(RA8876_MACR); //02h
  lcdDataWrite(RA8876_DIRECT_WRITE << 6 | RA8876_READ_MEMORY_LRTB << 4 | RA8876_WRITE_MEMORY_LRTB << 1);

  lcdRegWrite(RA8876_ICR); //03h
  lcdDataWrite(RA8877_LVDS_FORMAT << 3 | RA8876_GRAPHIC_MODE << 2 | RA8876_MEMORY_SELECT_IMAGE);

  lcdRegWrite(RA8876_MPWCTR); //10h
  lcdDataWrite(RA8876_PIP1_WINDOW_DISABLE << 7 | RA8876_PIP2_WINDOW_DISABLE << 6 | RA8876_SELECT_CONFIG_PIP1 << 4 | RA8876_IMAGE_COLOCR_DEPTH_16BPP << 2 | TFT_MODE);

  lcdRegWrite(RA8876_PIPCDEP); //11h
  lcdDataWrite(RA8876_PIP1_COLOR_DEPTH_16BPP << 2 | RA8876_PIP2_COLOR_DEPTH_16BPP);

  lcdRegWrite(RA8876_AW_COLOR); //5Eh
  lcdDataWrite(RA8876_CANVAS_BLOCK_MODE << 2 | RA8876_CANVAS_COLOR_DEPTH_16BPP);

  lcdRegDataWrite(RA8876_BTE_COLR, RA8876_S0_COLOR_DEPTH_16BPP << 5 | RA8876_S1_COLOR_DEPTH_16BPP << 2 | RA8876_S0_COLOR_DEPTH_16BPP); //92h

  /*TFT timing configure*/
  lcdRegWrite(RA8876_DPCR); //12h
  lcdDataWrite(XPCLK_INV << 7 | RA8876_DISPLAY_OFF << 6 | RA8876_OUTPUT_RGB);

  lcdRegWrite(RA8876_PCSR); //13h
  lcdDataWrite(XHSYNC_INV << 7 | XVSYNC_INV << 6 | XDE_INV << 5);

  lcdHorizontalWidthVerticalHeight(HDW, VDH);
  lcdHorizontalNonDisplay(HND);
  lcdHsyncStartPosition(HST);
  lcdHsyncPulseWidth(HPW);
  lcdVerticalNonDisplay(VND);
  lcdVsyncStartPosition(VST);
  lcdVsyncPulseWidth(VPW);

  /*image buffer configure*/
  displayImageStartAddress(PAGE1_START_ADDR);
  displayImageWidth(SCREEN_WIDTH);
  displayWindowStartXY(0, 0);
  canvasImageStartAddress(PAGE1_START_ADDR);
  canvasImageWidth(SCREEN_WIDTH);
  activeWindowXY(0, 0);
  activeWindowWH(SCREEN_WIDTH, SCREEN_HEIGHT);
  return true;
}
//**************************************************************//
//**************************************************************//
void Ra8876_Lite::lcdRegWrite(ru8 reg)
{
  //SPI.transfer(_xnscs,RA8876_SPI_CMDWRITE,SPI_CONTINUE);
  //SPI.transfer(_xnscs,reg);
  low(_xnscs); //enable communication using chip select
  shift_out(mosi, sck, MSBFIRST, 8, RA8876_SPI_CMDWRITE);
  shift_out(mosi, sck, MSBFIRST, 8, reg);
  high(_xnscs);
}
//**************************************************************//
//**************************************************************//
void Ra8876_Lite::lcdDataWrite(ru8 data)
{
  //SPI.transfer(_xnscs,RA8876_SPI_DATAWRITE,SPI_CONTINUE);
  //SPI.transfer(_xnscs,data);
  low(_xnscs); //enable communication using chip select
  shift_out(mosi, sck, MSBFIRST, 8, RA8876_SPI_DATAWRITE);
  shift_out(mosi, sck, MSBFIRST, 8, data);
  high(_xnscs);
}
//**************************************************************//
//**************************************************************//
ru8 Ra8876_Lite::lcdDataRead(void)
{
  //SPI.transfer(_xnscs,RA8876_SPI_DATAREAD,SPI_CONTINUE);
  //ru8 data = SPI.transfer(_xnscs,0xff);
  low(_xnscs); //enable communication using chip select
  shift_out(mosi, sck, MSBFIRST, 8, RA8876_SPI_DATAREAD);
  high(mosi);
  ru8 data = shift_in(miso, sck, MSBPRE, 8);
  high(_xnscs); //end communication
  print("lcdDataRead: %u\n", data);
  return data;
}
//**************************************************************//
//**************************************************************//
ru8 Ra8876_Lite::lcdStatusRead(void)
{
  //SPI.transfer(_xnscs,RA8876_SPI_STATUSREAD,SPI_CONTINUE);
  //ru8 data= SPI.transfer(_xnscs,0xff);
  low(_xnscs); //enable communication using chip select
  shift_out(mosi, sck, MSBFIRST, 8, RA8876_SPI_STATUSREAD);
  high(mosi);
  ru8 data = shift_in(miso, sck, MSBPRE, 8);
  high(_xnscs); //end communication
  print("lcdStatusRead: %u\n", data);
  return data;
}
//**************************************************************//
//**************************************************************//
void Ra8876_Lite::lcdRegDataWrite(ru8 reg, ru8 data)
{
  lcdRegWrite(reg);
  lcdDataWrite(data);
}
//**************************************************************//
//**************************************************************//
ru8 Ra8876_Lite::lcdRegDataRead(ru8 reg)
{
  lcdRegWrite(reg);
  return lcdDataRead();
}

//**************************************************************//
// support SPI interface to write 16bpp data after Regwrite 04h
//**************************************************************//
void Ra8876_Lite::lcdDataWrite16bbp(ru16 data)
{
  //SPI.transfer(_xnscs,RA8876_SPI_DATAWRITE,SPI_CONTINUE);
  //SPI.transfer(_xnscs,data,SPI_CONTINUE);
  //SPI.transfer(_xnscs,data>>8);
  low(_xnscs); //enable communication using chip select
  shift_out(mosi, sck, MSBFIRST, 8, RA8876_SPI_DATAWRITE);
  shift_out(mosi, sck, MSBFIRST, 8, data);
  shift_out(mosi, sck, MSBFIRST, 8, data >> 8);
  high(_xnscs); //end communication
}

//**************************************************************//
//RA8876 register
//**************************************************************//
/*[Status Register] bit7  Host Memory Write FIFO full
0: Memory Write FIFO is not full.
1: Memory Write FIFO is full.
Only when Memory Write FIFO is not full, MPU may write another one pixel.*/
//**************************************************************//
void Ra8876_Lite::checkWriteFifoNotFull(void)
{
  ru16 i;
  for (i = 0; i < 10000; i++) //Please according to your usage to modify i value.
  {
    if ((lcdStatusRead() & 0x80) == 0)
    {
      break;
    }
  }
}

//**************************************************************//
/*[Status Register] bit6  Host Memory Write FIFO empty
0: Memory Write FIFO is not empty.
1: Memory Write FIFO is empty.
When Memory Write FIFO is empty, MPU may write 8bpp data 64
pixels, or 16bpp data 32 pixels, 24bpp data 16 pixels directly.*/
//**************************************************************//
void Ra8876_Lite::checkWriteFifoEmpty(void)
{
  ru16 i;
  for (i = 0; i < 10000; i++) //Please according to your usage to modify i value.
  {
    if ((lcdStatusRead() & 0x40) == 0x40)
    {
      break;
    }
  }
}

//**************************************************************//
/*[Status Register] bit5  Host Memory Read FIFO full
0: Memory Read FIFO is not full.
1: Memory Read FIFO is full.
When Memory Read FIFO is full, MPU may read 8bpp data 32
pixels, or 16bpp data 16 pixels, 24bpp data 8 pixels directly.*/
//**************************************************************//
void Ra8876_Lite::checkReadFifoNotFull(void)
{
  ru16 i;
  for (i = 0; i < 10000; i++) //Please according to your usage to modify i value.
  {
    if ((lcdStatusRead() & 0x20) == 0x00)
    {
      break;
    }
  }
}

//**************************************************************//
/*[Status Register] bit4   Host Memory Read FIFO empty
0: Memory Read FIFO is not empty.
1: Memory Read FIFO is empty.*/
//**************************************************************//
void Ra8876_Lite::checkReadFifoNotEmpty(void)
{
  ru16 i;
  for (i = 0; i < 10000; i++) // //Please according to your usage to modify i value.
  {
    if ((lcdStatusRead() & 0x10) == 0x00)
    {
      break;
    }
  }
}

//**************************************************************//
/*[Status Register] bit3   Core task is busy
Following task is running:
BTE, Geometry engine, Serial flash DMA, Text write or Graphic write
0: task is done or idle.   1: task is busy*/
//**************************************************************//
void Ra8876_Lite::check2dBusy(void)
{
  ru32 i;
  for (i = 0; i < 1000000; i++) //Please according to your usage to modify i value.
  {
    //delayMicroseconds(1);
    if ((lcdStatusRead() & 0x08) == 0x00)
    {
      break;
    }
  }
}

//**************************************************************//
/*[Status Register] bit2   SDRAM ready for access
0: SDRAM is not ready for access   1: SDRAM is ready for access*/
//**************************************************************//
bool Ra8876_Lite::checkSdramReady(void)
{
  ru32 i;
  for (i = 0; i < 1000000; i++) //Please according to your usage to modify i value.
  {
    //delayMicroseconds(1);
    // set_pause_dt(CLKFREQ/1000000);
    pause(1);
    //  set_pause_dt(CLKFREQ/1000);
    if ((lcdStatusRead() & 0x04) == 0x04)
    {
      return true;
    }
  }
  return false;
}

//**************************************************************//
/*[Status Register] bit1  Operation mode status
0: Normal operation state  1: Inhibit operation state
Inhibit operation state means internal reset event keep running or
initial display still running or chip enter power saving state.	*/
//**************************************************************//
bool Ra8876_Lite::checkIcReady(void)
{
  ru32 i;
  for (i = 0; i < 1000000; i++) //Please according to your usage to modify i value.
  {
    //delayMicroseconds(1);
    // set_pause_dt(CLKFREQ/1000000);
    pause(1);
    // set_pause_dt(CLKFREQ/1000);
    if ((lcdStatusRead() & 0x02) == 0x00)
    {
      return true;
    }
  }
  return false;
}
//**************************************************************//
//**************************************************************//
//[05h] [06h] [07h] [08h] [09h] [0Ah]
//------------------------------------//----------------------------------*/
bool Ra8876_Lite::ra8876PllInitial(void)
{
  /*(1) 10MHz <= OSC_FREQ <= 15MHz 
  (2) 10MHz <= (OSC_FREQ/PLLDIVM) <= 40MHz
  (3) 250MHz <= [OSC_FREQ/(PLLDIVM+1)]x(PLLDIVN+1) <= 600MHz
PLLDIVM:0
PLLDIVN:1~63
PLLDIVK:CPLL & MPLL = 1/2/4/8.SPLL = 1/2/4/8/16/32/64/128.
ex:
 OSC_FREQ = 10MHz
 Set X_DIVK=2
 Set X_DIVM=0
 => (X_DIVN+1)=(XPLLx4)/10*/
  ru16 x_Divide, PLLC1, PLLC2;
  ru16 pll_m_lo, pll_m_hi;
  ru8 temp;

  // Set tft output pixel clock
  if (SCAN_FREQ >= 79) //&&(SCAN_FREQ<=100))
  {
    lcdRegDataWrite(0x05, 0x04); //PLL Divided by 4
    lcdRegDataWrite(0x06, (SCAN_FREQ * 4 / OSC_FREQ) - 1);
  }
  else if ((SCAN_FREQ >= 63) && (SCAN_FREQ <= 78))
  {
    lcdRegDataWrite(0x05, 0x05); //PLL Divided by 4
    lcdRegDataWrite(0x06, (SCAN_FREQ * 8 / OSC_FREQ) - 1);
  }
  else if ((SCAN_FREQ >= 40) && (SCAN_FREQ <= 62))
  {
    lcdRegDataWrite(0x05, 0x06); //PLL Divided by 8
    lcdRegDataWrite(0x06, (SCAN_FREQ * 8 / OSC_FREQ) - 1);
  }
  else if ((SCAN_FREQ >= 32) && (SCAN_FREQ <= 39))
  {
    lcdRegDataWrite(0x05, 0x07); //PLL Divided by 8
    lcdRegDataWrite(0x06, (SCAN_FREQ * 16 / OSC_FREQ) - 1);
  }
  else if ((SCAN_FREQ >= 16) && (SCAN_FREQ <= 31))
  {
    lcdRegDataWrite(0x05, 0x16); //PLL Divided by 16
    lcdRegDataWrite(0x06, (SCAN_FREQ * 16 / OSC_FREQ) - 1);
  }
  else if ((SCAN_FREQ >= 8) && (SCAN_FREQ <= 15))
  {
    lcdRegDataWrite(0x05, 0x26); //PLL Divided by 32
    lcdRegDataWrite(0x06, (SCAN_FREQ * 32 / OSC_FREQ) - 1);
  }
  else if ((SCAN_FREQ > 0) && (SCAN_FREQ <= 7))
  {
    lcdRegDataWrite(0x05, 0x36); //PLL Divided by 64
    lcdRegDataWrite(0x06, (SCAN_FREQ * 64 / OSC_FREQ) - 1);
  }

  // Set internal Buffer Ram clock
  if (DRAM_FREQ >= 158) //
  {
    lcdRegDataWrite(0x07, 0x02); //PLL Divided by 4
    lcdRegDataWrite(0x08, (DRAM_FREQ * 2 / OSC_FREQ) - 1);
  }
  else if ((DRAM_FREQ >= 125) && (DRAM_FREQ <= 157))
  {
    lcdRegDataWrite(0x07, 0x03); //PLL Divided by 4
    lcdRegDataWrite(0x08, (DRAM_FREQ * 4 / OSC_FREQ) - 1);
  }
  else if ((DRAM_FREQ >= 79) && (DRAM_FREQ <= 124))
  {
    lcdRegDataWrite(0x07, 0x04); //PLL Divided by 4
    lcdRegDataWrite(0x08, (DRAM_FREQ * 4 / OSC_FREQ) - 1);
  }
  else if ((DRAM_FREQ >= 63) && (DRAM_FREQ <= 78))
  {
    lcdRegDataWrite(0x07, 0x05); //PLL Divided by 4
    lcdRegDataWrite(0x08, (DRAM_FREQ * 8 / OSC_FREQ) - 1);
  }
  else if ((DRAM_FREQ >= 40) && (DRAM_FREQ <= 62))
  {
    lcdRegDataWrite(0x07, 0x06); //PLL Divided by 8
    lcdRegDataWrite(0x08, (DRAM_FREQ * 8 / OSC_FREQ) - 1);
  }
  else if ((DRAM_FREQ >= 32) && (DRAM_FREQ <= 39))
  {
    lcdRegDataWrite(0x07, 0x07); //PLL Divided by 16
    lcdRegDataWrite(0x08, (DRAM_FREQ * 16 / OSC_FREQ) - 1);
  }
  else if (DRAM_FREQ <= 31)
  {
    lcdRegDataWrite(0x07, 0x06);                    //PLL Divided by 8
    lcdRegDataWrite(0x08, (30 * 8 / OSC_FREQ) - 1); //set to 30MHz if out off range
  }

  // Set Core clock
  if (CORE_FREQ >= 158)
  {
    lcdRegDataWrite(0x09, 0x02); //PLL Divided by 2
    lcdRegDataWrite(0x0A, (CORE_FREQ * 2 / OSC_FREQ) - 1);
  }
  else if ((CORE_FREQ >= 125) && (CORE_FREQ <= 157))
  {
    lcdRegDataWrite(0x09, 0x03); //PLL Divided by 4
    lcdRegDataWrite(0x0A, (CORE_FREQ * 4 / OSC_FREQ) - 1);
  }
  else if ((CORE_FREQ >= 79) && (CORE_FREQ <= 124))
  {
    lcdRegDataWrite(0x09, 0x04); //PLL Divided by 4
    lcdRegDataWrite(0x0A, (CORE_FREQ * 4 / OSC_FREQ) - 1);
  }
  else if ((CORE_FREQ >= 63) && (CORE_FREQ <= 78))
  {
    lcdRegDataWrite(0x09, 0x05); //PLL Divided by 8
    lcdRegDataWrite(0x0A, (CORE_FREQ * 8 / OSC_FREQ) - 1);
  }
  else if ((CORE_FREQ >= 40) && (CORE_FREQ <= 62))
  {
    lcdRegDataWrite(0x09, 0x06); //PLL Divided by 8
    lcdRegDataWrite(0x0A, (CORE_FREQ * 8 / OSC_FREQ) - 1);
  }
  else if ((CORE_FREQ >= 32) && (CORE_FREQ <= 39))
  {
    lcdRegDataWrite(0x09, 0x06); //PLL Divided by 8
    lcdRegDataWrite(0x0A, (CORE_FREQ * 8 / OSC_FREQ) - 1);
  }
  else if (CORE_FREQ <= 31)
  {
    lcdRegDataWrite(0x09, 0x06);                    //PLL Divided by 8
    lcdRegDataWrite(0x0A, (30 * 8 / OSC_FREQ) - 1); //set to 30MHz if out off range
  }

  pause(1);
  lcdRegWrite(0x01);
  lcdDataWrite(0x80);
  pause(2); //wait for pll stable
  if ((lcdDataRead() & 0x80) == 0x80)
    return true;
  else
    return false;
}

//**************************************************************//
bool Ra8876_Lite::ra8876SdramInitial(void)
{
  ru8 CAS_Latency;
  ru16 Auto_Refresh;
#ifdef W9812G6JH
  CAS_Latency = 3;
  Auto_Refresh = (64 * DRAM_FREQ * 1000) / (4096);
  Auto_Refresh = Auto_Refresh - 2;
  lcdRegDataWrite(0xe0, 0x29);
  lcdRegDataWrite(0xe1, CAS_Latency); //CAS:2=0x02£¬CAS:3=0x03
  lcdRegDataWrite(0xe2, Auto_Refresh);
  lcdRegDataWrite(0xe3, Auto_Refresh >> 8);
  lcdRegDataWrite(0xe4, 0x01);
#endif
  checkSdramReady();
}
//**************************************************************//
//**************************************************************//
void Ra8876_Lite::displayOn(bool on)
{
  if (on)
    lcdRegDataWrite(RA8876_DPCR, XPCLK_INV << 7 | RA8876_DISPLAY_ON << 6 | RA8876_OUTPUT_RGB);
  else
    lcdRegDataWrite(RA8876_DPCR, XPCLK_INV << 7 | RA8876_DISPLAY_OFF << 6 | RA8876_OUTPUT_RGB);

  pause(20);
}
//**************************************************************//
//**************************************************************//
void Ra8876_Lite::lcdHorizontalWidthVerticalHeight(ru16 width, ru16 height)
{
  unsigned char temp;
  temp = (width / 8) - 1;
  lcdRegDataWrite(RA8876_HDWR, temp);
  temp = width % 8;
  lcdRegDataWrite(RA8876_HDWFTR, temp);
  temp = height - 1;
  lcdRegDataWrite(RA8876_VDHR0, temp);
  temp = (height - 1) >> 8;
  lcdRegDataWrite(RA8876_VDHR1, temp);
}
//**************************************************************//
//**************************************************************//
void Ra8876_Lite::lcdHorizontalNonDisplay(ru16 numbers)
{
  ru8 temp;
  if (numbers < 8)
  {
    lcdRegDataWrite(RA8876_HNDR, 0x00);
    lcdRegDataWrite(RA8876_HNDFTR, numbers);
  }
  else
  {
    temp = (numbers / 8) - 1;
    lcdRegDataWrite(RA8876_HNDR, temp);
    temp = numbers % 8;
    lcdRegDataWrite(RA8876_HNDFTR, temp);
  }
}
//**************************************************************//
//**************************************************************//
void Ra8876_Lite::lcdHsyncStartPosition(ru16 numbers)
{
  ru8 temp;
  if (numbers < 8)
  {
    lcdRegDataWrite(RA8876_HSTR, 0x00);
  }
  else
  {
    temp = (numbers / 8) - 1;
    lcdRegDataWrite(RA8876_HSTR, temp);
  }
}
//**************************************************************//
//**************************************************************//
void Ra8876_Lite::lcdHsyncPulseWidth(ru16 numbers)
{
  ru8 temp;
  if (numbers < 8)
  {
    lcdRegDataWrite(RA8876_HPWR, 0x00);
  }
  else
  {
    temp = (numbers / 8) - 1;
    lcdRegDataWrite(RA8876_HPWR, temp);
  }
}
//**************************************************************//
//**************************************************************//
void Ra8876_Lite::lcdVerticalNonDisplay(ru16 numbers)
{
  ru8 temp;
  temp = numbers - 1;
  lcdRegDataWrite(RA8876_VNDR0, temp);
  lcdRegDataWrite(RA8876_VNDR1, temp >> 8);
}
//**************************************************************//
//**************************************************************//
void Ra8876_Lite::lcdVsyncStartPosition(ru16 numbers)
{
  ru8 temp;
  temp = numbers - 1;
  lcdRegDataWrite(RA8876_VSTR, temp);
}
//**************************************************************//
//**************************************************************//
void Ra8876_Lite::lcdVsyncPulseWidth(ru16 numbers)
{
  ru8 temp;
  temp = numbers - 1;
  lcdRegDataWrite(RA8876_VPWR, temp);
}
//**************************************************************//
//**************************************************************//
void Ra8876_Lite::displayImageStartAddress(ru32 addr)
{
  lcdRegDataWrite(RA8876_MISA0, addr);       //20h
  lcdRegDataWrite(RA8876_MISA1, addr >> 8);  //21h
  lcdRegDataWrite(RA8876_MISA2, addr >> 16); //22h
  lcdRegDataWrite(RA8876_MISA3, addr >> 24); //23h
}
//**************************************************************//
//**************************************************************//
void Ra8876_Lite::displayImageWidth(ru16 width)
{
  lcdRegDataWrite(RA8876_MIW0, width);      //24h
  lcdRegDataWrite(RA8876_MIW1, width >> 8); //25h
}
//**************************************************************//
//**************************************************************//
void Ra8876_Lite::displayWindowStartXY(ru16 x0, ru16 y0)
{
  lcdRegDataWrite(RA8876_MWULX0, x0);      //26h
  lcdRegDataWrite(RA8876_MWULX1, x0 >> 8); //27h
  lcdRegDataWrite(RA8876_MWULY0, y0);      //28h
  lcdRegDataWrite(RA8876_MWULY1, y0 >> 8); //29h
}
//**************************************************************//
//**************************************************************//
void Ra8876_Lite::canvasImageStartAddress(ru32 addr)
{
  lcdRegDataWrite(RA8876_CVSSA0, addr);       //50h
  lcdRegDataWrite(RA8876_CVSSA1, addr >> 8);  //51h
  lcdRegDataWrite(RA8876_CVSSA2, addr >> 16); //52h
  lcdRegDataWrite(RA8876_CVSSA3, addr >> 24); //53h
}
//**************************************************************//
//**************************************************************//
void Ra8876_Lite::canvasImageWidth(ru16 width)
{
  lcdRegDataWrite(RA8876_CVS_IMWTH0, width);      //54h
  lcdRegDataWrite(RA8876_CVS_IMWTH1, width >> 8); //55h
}
//**************************************************************//
//**************************************************************//
void Ra8876_Lite::activeWindowXY(ru16 x0, ru16 y0)
{
  lcdRegDataWrite(RA8876_AWUL_X0, x0);      //56h
  lcdRegDataWrite(RA8876_AWUL_X1, x0 >> 8); //57h
  lcdRegDataWrite(RA8876_AWUL_Y0, y0);      //58h
  lcdRegDataWrite(RA8876_AWUL_Y1, y0 >> 8); //59h
}
//**************************************************************//
//**************************************************************//
void Ra8876_Lite::activeWindowWH(ru16 width, ru16 height)
{
  lcdRegDataWrite(RA8876_AW_WTH0, width);      //5ah
  lcdRegDataWrite(RA8876_AW_WTH1, width >> 8); //5bh
  lcdRegDataWrite(RA8876_AW_HT0, height);      //5ch
  lcdRegDataWrite(RA8876_AW_HT1, height >> 8); //5dh
}
//**************************************************************//
//**************************************************************//
void Ra8876_Lite::setPixelCursor(ru16 x, ru16 y)
{
  lcdRegDataWrite(RA8876_CURH0, x);      //5fh
  lcdRegDataWrite(RA8876_CURH1, x >> 8); //60h
  lcdRegDataWrite(RA8876_CURV0, y);      //61h
  lcdRegDataWrite(RA8876_CURV1, y >> 8); //62h
}
//**************************************************************//
//**************************************************************//
void Ra8876_Lite::bte_Source0_MemoryStartAddr(ru32 addr)
{
  lcdRegDataWrite(RA8876_S0_STR0, addr);       //93h
  lcdRegDataWrite(RA8876_S0_STR1, addr >> 8);  //94h
  lcdRegDataWrite(RA8876_S0_STR2, addr >> 16); //95h
  lcdRegDataWrite(RA8876_S0_STR3, addr >> 24); ////96h
}
//**************************************************************//
//**************************************************************//
void Ra8876_Lite::bte_Source0_ImageWidth(ru16 width)
{
  lcdRegDataWrite(RA8876_S0_WTH0, width);      //97h
  lcdRegDataWrite(RA8876_S0_WTH1, width >> 8); //98h
}
//**************************************************************//
//**************************************************************//
void Ra8876_Lite::bte_Source0_WindowStartXY(ru16 x0, ru16 y0)
{
  lcdRegDataWrite(RA8876_S0_X0, x0);      //99h
  lcdRegDataWrite(RA8876_S0_X1, x0 >> 8); //9ah
  lcdRegDataWrite(RA8876_S0_Y0, y0);      //9bh
  lcdRegDataWrite(RA8876_S0_Y1, y0 >> 8); //9ch
}
//**************************************************************//
//**************************************************************//
void Ra8876_Lite::bte_Source1_MemoryStartAddr(ru32 addr)
{
  lcdRegDataWrite(RA8876_S1_STR0, addr);       //9dh
  lcdRegDataWrite(RA8876_S1_STR1, addr >> 8);  //9eh
  lcdRegDataWrite(RA8876_S1_STR2, addr >> 16); //9fh
  lcdRegDataWrite(RA8876_S1_STR3, addr >> 24); //a0h
}
//**************************************************************//
//**************************************************************//
void Ra8876_Lite::bte_Source1_ImageWidth(ru16 width)
{
  lcdRegDataWrite(RA8876_S1_WTH0, width);      //a1h
  lcdRegDataWrite(RA8876_S1_WTH1, width >> 8); //a2h
}
//**************************************************************//
//**************************************************************//
void Ra8876_Lite::bte_Source1_WindowStartXY(ru16 x0, ru16 y0)
{
  lcdRegDataWrite(RA8876_S1_X0, x0);      //a3h
  lcdRegDataWrite(RA8876_S1_X1, x0 >> 8); //a4h
  lcdRegDataWrite(RA8876_S1_Y0, y0);      //a5h
  lcdRegDataWrite(RA8876_S1_Y1, y0 >> 8); //a6h
}
//**************************************************************//
//**************************************************************//
void Ra8876_Lite::bte_DestinationMemoryStartAddr(ru32 addr)
{
  lcdRegDataWrite(RA8876_DT_STR0, addr);       //a7h
  lcdRegDataWrite(RA8876_DT_STR1, addr >> 8);  //a8h
  lcdRegDataWrite(RA8876_DT_STR2, addr >> 16); //a9h
  lcdRegDataWrite(RA8876_DT_STR3, addr >> 24); //aah
}
//**************************************************************//
//**************************************************************//
void Ra8876_Lite::bte_DestinationImageWidth(ru16 width)
{
  lcdRegDataWrite(RA8876_DT_WTH0, width);      //abh
  lcdRegDataWrite(RA8876_DT_WTH1, width >> 8); //ach
}
//**************************************************************//
//**************************************************************//
void Ra8876_Lite::bte_DestinationWindowStartXY(ru16 x0, ru16 y0)
{
  lcdRegDataWrite(RA8876_DT_X0, x0);      //adh
  lcdRegDataWrite(RA8876_DT_X1, x0 >> 8); //aeh
  lcdRegDataWrite(RA8876_DT_Y0, y0);      //afh
  lcdRegDataWrite(RA8876_DT_Y1, y0 >> 8); //b0h
}
//**************************************************************//
//**************************************************************//
void Ra8876_Lite::bte_WindowSize(ru16 width, ru16 height)
{
  lcdRegDataWrite(RA8876_BTE_WTH0, width);       //b1h
  lcdRegDataWrite(RA8876_BTE_WTH1, width >> 8);  //b2h
  lcdRegDataWrite(RA8876_BTE_HIG0, height);      //b3h
  lcdRegDataWrite(RA8876_BTE_HIG1, height >> 8); //b4h
}

//**************************************************************//
/*These 8 bits determine prescaler value for Timer 0 and 1.*/
/*Time base is ¡°Core_Freq / (Prescaler + 1)¡±*/
//**************************************************************//
void Ra8876_Lite::pwm_Prescaler(ru8 prescaler)
{
  lcdRegDataWrite(RA8876_PSCLR, prescaler); //84h
}
//**************************************************************//
//**************************************************************//
void Ra8876_Lite::pwm_ClockMuxReg(ru8 pwm1_clk_div, ru8 pwm0_clk_div, ru8 xpwm1_ctrl, ru8 xpwm0_ctrl)
{
  lcdRegDataWrite(RA8876_PMUXR, pwm1_clk_div << 6 | pwm0_clk_div << 4 | xpwm1_ctrl << 2 | xpwm0_ctrl); //85h
}
//**************************************************************//
//**************************************************************//
void Ra8876_Lite::pwm_Configuration(ru8 pwm1_inverter, ru8 pwm1_auto_reload, ru8 pwm1_start, ru8 pwm0_dead_zone, ru8 pwm0_inverter, ru8 pwm0_auto_reload, ru8 pwm0_start)
{
  lcdRegDataWrite(RA8876_PCFGR, pwm1_inverter << 6 | pwm1_auto_reload << 5 | pwm1_start << 4 | pwm0_dead_zone << 3 |
                                    pwm0_inverter << 2 | pwm0_auto_reload << 1 | pwm0_start); //86h
}
//**************************************************************//
//**************************************************************//
void Ra8876_Lite::pwm0_Duty(ru16 duty)
{
  lcdRegDataWrite(RA8876_TCMPB0L, duty);      //88h
  lcdRegDataWrite(RA8876_TCMPB0H, duty >> 8); //89h
}
//**************************************************************//
//**************************************************************//
void Ra8876_Lite::pwm0_ClocksPerPeriod(ru16 clocks_per_period)
{
  lcdRegDataWrite(RA8876_TCNTB0L, clocks_per_period);      //8ah
  lcdRegDataWrite(RA8876_TCNTB0H, clocks_per_period >> 8); //8bh
}
//**************************************************************//
//**************************************************************//
void Ra8876_Lite::pwm1_Duty(ru16 duty)
{
  lcdRegDataWrite(RA8876_TCMPB1L, duty);      //8ch
  lcdRegDataWrite(RA8876_TCMPB1H, duty >> 8); //8dh
}
//**************************************************************//
//**************************************************************//
void Ra8876_Lite::pwm1_ClocksPerPeriod(ru16 clocks_per_period)
{
  lcdRegDataWrite(RA8876_TCNTB1L, clocks_per_period);      //8eh
  lcdRegDataWrite(RA8876_TCNTB1F, clocks_per_period >> 8); //8fh
}
//**************************************************************//
//**************************************************************//
void Ra8876_Lite::ramAccessPrepare(void)
{
  lcdRegWrite(RA8876_MRWDP); //04h
}
//**************************************************************//
//**************************************************************//
void Ra8876_Lite::foreGroundColor16bpp(ru16 color)
{
  lcdRegDataWrite(RA8876_FGCR, color >> 8); //d2h
  lcdRegDataWrite(RA8876_FGCG, color >> 3); //d3h
  lcdRegDataWrite(RA8876_FGCB, color << 3); //d4h
}
//**************************************************************//
//**************************************************************//
void Ra8876_Lite::backGroundColor16bpp(ru16 color)
{
  lcdRegDataWrite(RA8876_BGCR, color >> 8); //d5h
  lcdRegDataWrite(RA8876_BGCG, color >> 3); //d6h
  lcdRegDataWrite(RA8876_BGCB, color << 3); //d7h
}

/*graphic function*/
//**************************************************************//
/* If use ra8876 graphic mode function turns on */
//**************************************************************//
void Ra8876_Lite::graphicMode(bool on)
{
  if (on)
    lcdRegDataWrite(RA8876_ICR, RA8877_LVDS_FORMAT << 3 | RA8876_GRAPHIC_MODE << 2 | RA8876_MEMORY_SELECT_IMAGE); //03h  //switch to graphic mode
  else
    lcdRegDataWrite(RA8876_ICR, RA8877_LVDS_FORMAT << 3 | RA8876_TEXT_MODE << 2 | RA8876_MEMORY_SELECT_IMAGE); //03h  //switch back to text mode
}
//**************************************************************//
/*support serial IF to write 16bpp pixel*/
//**************************************************************//
void Ra8876_Lite::putPixel_16bpp(ru16 x, ru16 y, ru16 color)
{
  setPixelCursor(x, y);
  ramAccessPrepare();
  //checkWriteFifoNotFull();//if high speed mcu and without Xnwait check
  //lcdDataWrite(color);
  //lcdDataWrite(color>>8);
  lcdDataWrite16bbp(color);
  //checkWriteFifoEmpty();//if high speed mcu and without Xnwait check
}
//**************************************************************//
/*support ra8876 serial IF to write 16bpp(RGB565) picture data for user operation */
//**************************************************************//
void Ra8876_Lite::putPicture_16bpp(ru16 x, ru16 y, ru16 width, ru16 height)
{
  ru16 i, j;
  activeWindowXY(x, y);
  activeWindowWH(width, height);
  setPixelCursor(x, y);
  ramAccessPrepare();
}
//**************************************************************//
/*support serial IF to write 16bpp(RGB565) picture data byte format from data pointer*/
//**************************************************************//
void Ra8876_Lite::putPicture_16bpp(ru16 x, ru16 y, ru16 width, ru16 height, const unsigned char *data)
{
  ru16 i, j;

  activeWindowXY(x, y);
  activeWindowWH(width, height);
  setPixelCursor(x, y);
  ramAccessPrepare();
  for (j = 0; j < height; j++)
  {
    for (i = 0; i < width; i++)
    {
      //checkWriteFifoNotFull();//if high speed mcu and without Xnwait check
      lcdDataWrite(*data);
      data++;
      //checkWriteFifoNotFull();//if high speed mcu and without Xnwait check
      lcdDataWrite(*data);
      data++;
    }
  }
  checkWriteFifoEmpty(); //if high speed mcu and without Xnwait check
  activeWindowXY(0, 0);
  activeWindowWH(SCREEN_WIDTH, SCREEN_HEIGHT);
}
//**************************************************************//
/*support serial IF to write 16bpp(RGB565) picture data word format from data pointer*/
//**************************************************************//
void Ra8876_Lite::putPicture_16bpp(ru16 x, ru16 y, ru16 width, ru16 height, const unsigned short *data)
{
  ru16 i, j;
  activeWindowXY(x, y);
  activeWindowWH(width, height);
  setPixelCursor(x, y);
  ramAccessPrepare();
  for (j = 0; j < height; j++)
  {
    for (i = 0; i < width; i++)
    {
      //checkWriteFifoNotFull();//if high speed mcu and without Xnwait check
      lcdDataWrite16bbp(*data);
      data++;
      //checkWriteFifoEmpty();//if high speed mcu and without Xnwait check
    }
  }
  checkWriteFifoEmpty(); //if high speed mcu and without Xnwait check
  activeWindowXY(0, 0);
  activeWindowWH(SCREEN_WIDTH, SCREEN_HEIGHT);
}

/*text function*/
//**************************************************************//
/* If use ra8876 text mode function turns on */
//**************************************************************//
void Ra8876_Lite::textMode(bool on)
{
  if (on)
    lcdRegDataWrite(RA8876_ICR, RA8877_LVDS_FORMAT << 3 | RA8876_TEXT_MODE << 2 | RA8876_MEMORY_SELECT_IMAGE); //03h  //switch to text mode
  else
    lcdRegDataWrite(RA8876_ICR, RA8877_LVDS_FORMAT << 3 | RA8876_GRAPHIC_MODE << 2 | RA8876_MEMORY_SELECT_IMAGE); //03h  //switch back to graphic mode
}
//**************************************************************//
//**************************************************************//
void Ra8876_Lite::textColor(ru16 foreground_color, ru16 background_color)
{
  foreGroundColor16bpp(foreground_color);
  backGroundColor16bpp(background_color);
}
//**************************************************************//
//**************************************************************//
void Ra8876_Lite::setTextCursor(ru16 x, ru16 y)
{
  lcdRegDataWrite(RA8876_F_CURX0, x);      //63h
  lcdRegDataWrite(RA8876_F_CURX1, x >> 8); //64h
  lcdRegDataWrite(RA8876_F_CURY0, y);      //65h
  lcdRegDataWrite(RA8876_F_CURY1, y >> 8); //66h
}
//**************************************************************//
/* source_select = 0 : internal CGROM,  source_select = 1: external CGROM, source_select = 2: user-define*/
/* size_select = 0 : 8*16/16*16, size_select = 1 : 12*24/24*24, size_select = 2 : 16*32/32*32  */
/* iso_select = 0 : iso8859-1, iso_select = 1 : iso8859-2, iso_select = 2 : iso8859-4, iso_select = 3 : iso8859-5*/
//**************************************************************//
void Ra8876_Lite::setTextParameter1(ru8 source_select, ru8 size_select, ru8 iso_select) //cch
{
  lcdRegDataWrite(RA8876_CCR0, source_select << 6 | size_select << 4 | iso_select); //cch
}
//**************************************************************//
/*align = 0 : full alignment disable, align = 1 : full alignment enable*/
/*chroma_key = 0 : text with chroma key disable, chroma_key = 1 : text with chroma key enable*/
/* width_enlarge and height_enlarge can be set 0~3, (00b: X1) (01b : X2)  (10b : X3)  (11b : X4)*/
//**************************************************************//
void Ra8876_Lite::setTextParameter2(ru8 align, ru8 chroma_key, ru8 width_enlarge, ru8 height_enlarge)
{
  lcdRegDataWrite(RA8876_CCR1, align << 7 | chroma_key << 6 | width_enlarge << 2 | height_enlarge); //cdh
}
//**************************************************************//
//**************************************************************//
void Ra8876_Lite::genitopCharacterRomParameter(ru8 scs_select, ru8 clk_div, ru8 rom_select, ru8 character_select, ru8 gt_width)
{
  if (scs_select == 0)
    lcdRegDataWrite(RA8876_SFL_CTRL, RA8876_SERIAL_FLASH_SELECT0 << 7 | RA8876_SERIAL_FLASH_FONT_MODE << 6 | RA8876_SERIAL_FLASH_ADDR_24BIT << 5 | RA8876_FOLLOW_RA8876_MODE << 4 | RA8876_SPI_FAST_READ_8DUMMY); //b7h
  if (scs_select == 1)
    lcdRegDataWrite(RA8876_SFL_CTRL, RA8876_SERIAL_FLASH_SELECT1 << 7 | RA8876_SERIAL_FLASH_FONT_MODE << 6 | RA8876_SERIAL_FLASH_ADDR_24BIT << 5 | RA8876_FOLLOW_RA8876_MODE << 4 | RA8876_SPI_FAST_READ_8DUMMY); //b7h

  lcdRegDataWrite(RA8876_SPI_DIVSOR, clk_div); //bbh

  lcdRegDataWrite(RA8876_GTFNT_SEL, rom_select << 5);                 //ceh
  lcdRegDataWrite(RA8876_GTFNT_CR, character_select << 3 | gt_width); //cfh
}
//**************************************************************//
//support ra8876 internal font and external string font code write from data pointer
//**************************************************************//
void Ra8876_Lite::putString(ru16 x0, ru16 y0, char *str)
{
  textMode(true);
  setTextCursor(x0, y0);
  ramAccessPrepare();
  while (*str != '\0')
  {
    checkWriteFifoNotFull();
    lcdDataWrite(*str);
    ++str;
  }
  check2dBusy();
  textMode(false);
}
/*put value,base on sprintf*/
//**************************************************************//
//vaule: -2147483648(-2^31) ~ 2147483647(2^31-1)
//len: 1~11 minimum output length
/*
  [flag] 
    n:¿¿ÓÒ¡¡
    -:¿¿×ó
    +:Ý”³öÕýØ“Ì–
    (space):®”²»Ý”³öÕýØ“Ì–•r£¬¾ÍÝ”³ö¿Õ°×
    0:ÔÚé_î^ÌŽ(×ó‚È) Ña 0£¬¶ø·ÇÑa¿Õ°×¡£ 
 */
//**************************************************************//
void Ra8876_Lite::putDec(ru16 x0, ru16 y0, rs32 vaule, ru8 len, const char *flag)
{
  char char_buffer[12];
  switch (len)
  {
  case 1:
    if (flag == "n")
    {
      sprintf(char_buffer, "%1d", vaule);
      putString(x0, y0, char_buffer);
    }
    else if (flag == "-")
    {
      sprintf(char_buffer, "%-1d", vaule);
      putString(x0, y0, char_buffer);
    }
    else if (flag == "+")
    {
      sprintf(char_buffer, "%+1d", vaule);
      putString(x0, y0, char_buffer);
    }
    else if (flag == "0")
    {
      sprintf(char_buffer, "%01d", vaule);
      putString(x0, y0, char_buffer);
    }
    break;
  case 2:
    if (flag == "n")
    {
      sprintf(char_buffer, "%2d", vaule);
      putString(x0, y0, char_buffer);
    }
    else if (flag == "-")
    {
      sprintf(char_buffer, "%-2d", vaule);
      putString(x0, y0, char_buffer);
    }
    else if (flag == "+")
    {
      sprintf(char_buffer, "%+2d", vaule);
      putString(x0, y0, char_buffer);
    }
    else if (flag == "0")
    {
      sprintf(char_buffer, "%02d", vaule);
      putString(x0, y0, char_buffer);
    }
    break;
  case 3:
    if (flag == "n")
    {
      sprintf(char_buffer, "%3d", vaule);
      putString(x0, y0, char_buffer);
    }
    else if (flag == "-")
    {
      sprintf(char_buffer, "%-3d", vaule);
      putString(x0, y0, char_buffer);
    }
    else if (flag == "+")
    {
      sprintf(char_buffer, "%+3d", vaule);
      putString(x0, y0, char_buffer);
    }
    else if (flag == "0")
    {
      sprintf(char_buffer, "%03d", vaule);
      putString(x0, y0, char_buffer);
    }
    break;
  case 4:
    if (flag == "n")
    {
      sprintf(char_buffer, "%4d", vaule);
      putString(x0, y0, char_buffer);
    }
    else if (flag == "-")
    {
      sprintf(char_buffer, "%-4d", vaule);
      putString(x0, y0, char_buffer);
    }
    else if (flag == "+")
    {
      sprintf(char_buffer, "%+4d", vaule);
      putString(x0, y0, char_buffer);
    }
    else if (flag == "0")
    {
      sprintf(char_buffer, "%04d", vaule);
      putString(x0, y0, char_buffer);
    }
    break;
  case 5:
    if (flag == "n")
    {
      sprintf(char_buffer, "%5d", vaule);
      putString(x0, y0, char_buffer);
    }
    else if (flag == "-")
    {
      sprintf(char_buffer, "%-5d", vaule);
      putString(x0, y0, char_buffer);
    }
    else if (flag == "+")
    {
      sprintf(char_buffer, "%+5d", vaule);
      putString(x0, y0, char_buffer);
    }
    else if (flag == "0")
    {
      sprintf(char_buffer, "%05d", vaule);
      putString(x0, y0, char_buffer);
    }
    break;
  case 6:
    if (flag == "n")
    {
      sprintf(char_buffer, "%6d", vaule);
      putString(x0, y0, char_buffer);
    }
    else if (flag == "-")
    {
      sprintf(char_buffer, "%-6d", vaule);
      putString(x0, y0, char_buffer);
    }
    else if (flag == "+")
    {
      sprintf(char_buffer, "%+6d", vaule);
      putString(x0, y0, char_buffer);
    }
    else if (flag == "0")
    {
      sprintf(char_buffer, "%06d", vaule);
      putString(x0, y0, char_buffer);
    }
    break;
  case 7:
    if (flag == "n")
    {
      sprintf(char_buffer, "%7d", vaule);
      putString(x0, y0, char_buffer);
    }
    else if (flag == "-")
    {
      sprintf(char_buffer, "%-7d", vaule);
      putString(x0, y0, char_buffer);
    }
    else if (flag == "+")
    {
      sprintf(char_buffer, "%+7d", vaule);
      putString(x0, y0, char_buffer);
    }
    else if (flag == "0")
    {
      sprintf(char_buffer, "%07d", vaule);
      putString(x0, y0, char_buffer);
    }
    break;
  case 8:
    if (flag == "n")
    {
      sprintf(char_buffer, "%8d", vaule);
      putString(x0, y0, char_buffer);
    }
    else if (flag == "-")
    {
      sprintf(char_buffer, "%-8d", vaule);
      putString(x0, y0, char_buffer);
    }
    else if (flag == "+")
    {
      sprintf(char_buffer, "%+8d", vaule);
      putString(x0, y0, char_buffer);
    }
    else if (flag == "0")
    {
      sprintf(char_buffer, "%08d", vaule);
      putString(x0, y0, char_buffer);
    }
    break;
  case 9:
    if (flag == "n")
    {
      sprintf(char_buffer, "%9d", vaule);
      putString(x0, y0, char_buffer);
    }
    else if (flag == "-")
    {
      sprintf(char_buffer, "%-9d", vaule);
      putString(x0, y0, char_buffer);
    }
    else if (flag == "+")
    {
      sprintf(char_buffer, "%+9d", vaule);
      putString(x0, y0, char_buffer);
    }
    else if (flag == "0")
    {
      sprintf(char_buffer, "%09d", vaule);
      putString(x0, y0, char_buffer);
    }
    break;
  case 10:
    if (flag == "n")
    {
      sprintf(char_buffer, "%10d", vaule);
      putString(x0, y0, char_buffer);
    }
    else if (flag == "-")
    {
      sprintf(char_buffer, "%-10d", vaule);
      putString(x0, y0, char_buffer);
    }
    else if (flag == "+")
    {
      sprintf(char_buffer, "%+10d", vaule);
      putString(x0, y0, char_buffer);
    }
    else if (flag == "0")
    {
      sprintf(char_buffer, "%010d", vaule);
      putString(x0, y0, char_buffer);
    }
    break;
  case 11:
    if (flag == "n")
    {
      sprintf(char_buffer, "%11d", vaule);
      putString(x0, y0, char_buffer);
    }
    else if (flag == "-")
    {
      sprintf(char_buffer, "%-11d", vaule);
      putString(x0, y0, char_buffer);
    }
    else if (flag == "+")
    {
      sprintf(char_buffer, "%+11d", vaule);
      putString(x0, y0, char_buffer);
    }
    else if (flag == "0")
    {
      sprintf(char_buffer, "%011d", vaule);
      putString(x0, y0, char_buffer);
    }
    break;
  default:
    break;
  }
}

//**************************************************************//
//vaule: (3.4E-38) ~ (3.4E38)
//len: 1~11 minimum output length
//precision: right side of point numbers 1~4
/*
  [flag] 
    n:¿¿ÓÒ¡¡
    -:¿¿×ó
    +:Ý”³öÕýØ“Ì–
    (space):®”²»Ý”³öÕýØ“Ì–•r£¬¾ÍÝ”³ö¿Õ°×
    #: ŠÖÆÝ”³öÐ¡”µüc
    0:ÔÚé_î^ÌŽ(×ó‚È) Ña 0£¬¶ø·ÇÑa¿Õ°×¡£
    
 */
//arduino Floats have only 6-7 decimal digits of precision. That means the total number of digits, not the number to the right of the decimal point.
//Unlike other platforms, where you can get more precision by using a double (e.g. up to 15 digits), on the Arduino, double is the same size as float.
//**************************************************************//
void Ra8876_Lite::putFloat(ru16 x0, ru16 y0, double vaule, ru8 len, ru8 precision, const char *flag)
{
  char char_buffer[20];
  switch (len)
  {
  case 1:
    if (flag == "n")
    {
      if (precision == 1)
        sprintf(char_buffer, "%1.1f", vaule);
      putString(x0, y0, char_buffer);
      if (precision == 2)
        sprintf(char_buffer, "%1.2f", vaule);
      putString(x0, y0, char_buffer);
      if (precision == 3)
        sprintf(char_buffer, "%1.3f", vaule);
      putString(x0, y0, char_buffer);
      if (precision == 4)
        sprintf(char_buffer, "%1.4f", vaule);
      putString(x0, y0, char_buffer);
    }
    else if (flag == "-")
    {
      if (precision == 1)
        sprintf(char_buffer, "%-1.1f", vaule);
      putString(x0, y0, char_buffer);
      if (precision == 2)
        sprintf(char_buffer, "%-1.2f", vaule);
      putString(x0, y0, char_buffer);
      if (precision == 3)
        sprintf(char_buffer, "%-1.3f", vaule);
      putString(x0, y0, char_buffer);
      if (precision == 4)
        sprintf(char_buffer, "%-1.4f", vaule);
      putString(x0, y0, char_buffer);
    }
    else if (flag == "+")
    {
      if (precision == 1)
        sprintf(char_buffer, "%+1.1f", vaule);
      putString(x0, y0, char_buffer);
      if (precision == 2)
        sprintf(char_buffer, "%+1.2f", vaule);
      putString(x0, y0, char_buffer);
      if (precision == 3)
        sprintf(char_buffer, "%+1.3f", vaule);
      putString(x0, y0, char_buffer);
      if (precision == 4)
        sprintf(char_buffer, "%+1.4f", vaule);
      putString(x0, y0, char_buffer);
    }
    else if (flag == "0")
    {
      if (precision == 1)
        sprintf(char_buffer, "%01.1f", vaule);
      putString(x0, y0, char_buffer);
      if (precision == 2)
        sprintf(char_buffer, "%01.2f", vaule);
      putString(x0, y0, char_buffer);
      if (precision == 3)
        sprintf(char_buffer, "%01.3f", vaule);
      putString(x0, y0, char_buffer);
      if (precision == 4)
        sprintf(char_buffer, "%01.4f", vaule);
      putString(x0, y0, char_buffer);
    }
    break;
  case 2:
    if (flag == "n")
    {
      if (precision == 1)
        sprintf(char_buffer, "%2.1f", vaule);
      putString(x0, y0, char_buffer);
      if (precision == 2)
        sprintf(char_buffer, "%2.2f", vaule);
      putString(x0, y0, char_buffer);
      if (precision == 3)
        sprintf(char_buffer, "%2.3f", vaule);
      putString(x0, y0, char_buffer);
      if (precision == 4)
        sprintf(char_buffer, "%2.4f", vaule);
      putString(x0, y0, char_buffer);
    }
    else if (flag == "-")
    {
      if (precision == 1)
        sprintf(char_buffer, "%-2.1f", vaule);
      putString(x0, y0, char_buffer);
      if (precision == 2)
        sprintf(char_buffer, "%-2.2f", vaule);
      putString(x0, y0, char_buffer);
      if (precision == 3)
        sprintf(char_buffer, "%-2.3f", vaule);
      putString(x0, y0, char_buffer);
      if (precision == 4)
        sprintf(char_buffer, "%-2.4f", vaule);
      putString(x0, y0, char_buffer);
    }
    else if (flag == "+")
    {
      if (precision == 1)
        sprintf(char_buffer, "%+2.1f", vaule);
      putString(x0, y0, char_buffer);
      if (precision == 2)
        sprintf(char_buffer, "%+2.2f", vaule);
      putString(x0, y0, char_buffer);
      if (precision == 3)
        sprintf(char_buffer, "%+2.3f", vaule);
      putString(x0, y0, char_buffer);
      if (precision == 4)
        sprintf(char_buffer, "%+2.4f", vaule);
      putString(x0, y0, char_buffer);
    }
    else if (flag == "0")
    {
      if (precision == 1)
        sprintf(char_buffer, "%02.1f", vaule);
      putString(x0, y0, char_buffer);
      if (precision == 2)
        sprintf(char_buffer, "%02.2f", vaule);
      putString(x0, y0, char_buffer);
      if (precision == 3)
        sprintf(char_buffer, "%02.3f", vaule);
      putString(x0, y0, char_buffer);
      if (precision == 4)
        sprintf(char_buffer, "%02.4f", vaule);
      putString(x0, y0, char_buffer);
    }
    break;
  case 3:
    if (flag == "n")
    {
      if (precision == 1)
        sprintf(char_buffer, "%3.1f", vaule);
      putString(x0, y0, char_buffer);
      if (precision == 2)
        sprintf(char_buffer, "%3.2f", vaule);
      putString(x0, y0, char_buffer);
      if (precision == 3)
        sprintf(char_buffer, "%3.3f", vaule);
      putString(x0, y0, char_buffer);
      if (precision == 4)
        sprintf(char_buffer, "%3.4f", vaule);
      putString(x0, y0, char_buffer);
    }
    else if (flag == "-")
    {
      if (precision == 1)
        sprintf(char_buffer, "%-3.1f", vaule);
      putString(x0, y0, char_buffer);
      if (precision == 2)
        sprintf(char_buffer, "%-3.2f", vaule);
      putString(x0, y0, char_buffer);
      if (precision == 3)
        sprintf(char_buffer, "%-3.3f", vaule);
      putString(x0, y0, char_buffer);
      if (precision == 4)
        sprintf(char_buffer, "%-3.4f", vaule);
      putString(x0, y0, char_buffer);
    }
    else if (flag == "+")
    {
      if (precision == 1)
        sprintf(char_buffer, "%+3.1f", vaule);
      putString(x0, y0, char_buffer);
      if (precision == 2)
        sprintf(char_buffer, "%+3.2f", vaule);
      putString(x0, y0, char_buffer);
      if (precision == 3)
        sprintf(char_buffer, "%+3.3f", vaule);
      putString(x0, y0, char_buffer);
      if (precision == 4)
        sprintf(char_buffer, "%+3.4f", vaule);
      putString(x0, y0, char_buffer);
    }
    else if (flag == "0")
    {
      if (precision == 1)
        sprintf(char_buffer, "%03.1f", vaule);
      putString(x0, y0, char_buffer);
      if (precision == 2)
        sprintf(char_buffer, "%03.2f", vaule);
      putString(x0, y0, char_buffer);
      if (precision == 3)
        sprintf(char_buffer, "%03.3f", vaule);
      putString(x0, y0, char_buffer);
      if (precision == 4)
        sprintf(char_buffer, "%03.4f", vaule);
      putString(x0, y0, char_buffer);
    }
    break;
  case 4:
    if (flag == "n")
    {
      if (precision == 1)
        sprintf(char_buffer, "%4.1f", vaule);
      putString(x0, y0, char_buffer);
      if (precision == 2)
        sprintf(char_buffer, "%4.2f", vaule);
      putString(x0, y0, char_buffer);
      if (precision == 3)
        sprintf(char_buffer, "%4.3f", vaule);
      putString(x0, y0, char_buffer);
      if (precision == 4)
        sprintf(char_buffer, "%4.4f", vaule);
      putString(x0, y0, char_buffer);
    }
    else if (flag == "-")
    {
      if (precision == 1)
        sprintf(char_buffer, "%-4.1f", vaule);
      putString(x0, y0, char_buffer);
      if (precision == 2)
        sprintf(char_buffer, "%-4.2f", vaule);
      putString(x0, y0, char_buffer);
      if (precision == 3)
        sprintf(char_buffer, "%-4.3f", vaule);
      putString(x0, y0, char_buffer);
      if (precision == 4)
        sprintf(char_buffer, "%-4.4f", vaule);
      putString(x0, y0, char_buffer);
    }
    else if (flag == "+")
    {
      if (precision == 1)
        sprintf(char_buffer, "%+4.1f", vaule);
      putString(x0, y0, char_buffer);
      if (precision == 2)
        sprintf(char_buffer, "%+4.2f", vaule);
      putString(x0, y0, char_buffer);
      if (precision == 3)
        sprintf(char_buffer, "%+4.3f", vaule);
      putString(x0, y0, char_buffer);
      if (precision == 4)
        sprintf(char_buffer, "%+4.4f", vaule);
      putString(x0, y0, char_buffer);
    }
    else if (flag == "0")
    {
      if (precision == 1)
        sprintf(char_buffer, "%04.1f", vaule);
      putString(x0, y0, char_buffer);
      if (precision == 2)
        sprintf(char_buffer, "%04.2f", vaule);
      putString(x0, y0, char_buffer);
      if (precision == 3)
        sprintf(char_buffer, "%04.3f", vaule);
      putString(x0, y0, char_buffer);
      if (precision == 4)
        sprintf(char_buffer, "%04.4f", vaule);
      putString(x0, y0, char_buffer);
    }
    break;
  case 5:
    if (flag == "n")
    {
      if (precision == 1)
        sprintf(char_buffer, "%5.1f", vaule);
      putString(x0, y0, char_buffer);
      if (precision == 2)
        sprintf(char_buffer, "%5.2f", vaule);
      putString(x0, y0, char_buffer);
      if (precision == 3)
        sprintf(char_buffer, "%5.3f", vaule);
      putString(x0, y0, char_buffer);
      if (precision == 4)
        sprintf(char_buffer, "%5.4f", vaule);
      putString(x0, y0, char_buffer);
    }
    else if (flag == "-")
    {
      if (precision == 1)
        sprintf(char_buffer, "%-5.1f", vaule);
      putString(x0, y0, char_buffer);
      if (precision == 2)
        sprintf(char_buffer, "%-5.2f", vaule);
      putString(x0, y0, char_buffer);
      if (precision == 3)
        sprintf(char_buffer, "%-5.3f", vaule);
      putString(x0, y0, char_buffer);
      if (precision == 4)
        sprintf(char_buffer, "%-5.4f", vaule);
      putString(x0, y0, char_buffer);
    }
    else if (flag == "+")
    {
      if (precision == 1)
        sprintf(char_buffer, "%+5.1f", vaule);
      putString(x0, y0, char_buffer);
      if (precision == 2)
        sprintf(char_buffer, "%+5.2f", vaule);
      putString(x0, y0, char_buffer);
      if (precision == 3)
        sprintf(char_buffer, "%+5.3f", vaule);
      putString(x0, y0, char_buffer);
      if (precision == 4)
        sprintf(char_buffer, "%+5.4f", vaule);
      putString(x0, y0, char_buffer);
    }
    else if (flag == "0")
    {
      if (precision == 1)
        sprintf(char_buffer, "%05.1f", vaule);
      putString(x0, y0, char_buffer);
      if (precision == 2)
        sprintf(char_buffer, "%05.2f", vaule);
      putString(x0, y0, char_buffer);
      if (precision == 3)
        sprintf(char_buffer, "%05.3f", vaule);
      putString(x0, y0, char_buffer);
      if (precision == 4)
        sprintf(char_buffer, "%05.4f", vaule);
      putString(x0, y0, char_buffer);
    }
    break;
  case 6:
    if (flag == "n")
    {
      if (precision == 1)
        sprintf(char_buffer, "%6.1f", vaule);
      putString(x0, y0, char_buffer);
      if (precision == 2)
        sprintf(char_buffer, "%6.2f", vaule);
      putString(x0, y0, char_buffer);
      if (precision == 3)
        sprintf(char_buffer, "%6.3f", vaule);
      putString(x0, y0, char_buffer);
      if (precision == 4)
        sprintf(char_buffer, "%6.4f", vaule);
      putString(x0, y0, char_buffer);
    }
    else if (flag == "-")
    {
      if (precision == 1)
        sprintf(char_buffer, "%-6.1f", vaule);
      putString(x0, y0, char_buffer);
      if (precision == 2)
        sprintf(char_buffer, "%-6.2f", vaule);
      putString(x0, y0, char_buffer);
      if (precision == 3)
        sprintf(char_buffer, "%-6.3f", vaule);
      putString(x0, y0, char_buffer);
      if (precision == 4)
        sprintf(char_buffer, "%-6.4f", vaule);
      putString(x0, y0, char_buffer);
    }
    else if (flag == "+")
    {
      if (precision == 1)
        sprintf(char_buffer, "%+6.1f", vaule);
      putString(x0, y0, char_buffer);
      if (precision == 2)
        sprintf(char_buffer, "%+6.2f", vaule);
      putString(x0, y0, char_buffer);
      if (precision == 3)
        sprintf(char_buffer, "%+6.3f", vaule);
      putString(x0, y0, char_buffer);
      if (precision == 4)
        sprintf(char_buffer, "%+6.4f", vaule);
      putString(x0, y0, char_buffer);
    }
    else if (flag == "0")
    {
      if (precision == 1)
        sprintf(char_buffer, "%06.1f", vaule);
      putString(x0, y0, char_buffer);
      if (precision == 2)
        sprintf(char_buffer, "%06.2f", vaule);
      putString(x0, y0, char_buffer);
      if (precision == 3)
        sprintf(char_buffer, "%06.3f", vaule);
      putString(x0, y0, char_buffer);
      if (precision == 4)
        sprintf(char_buffer, "%06.4f", vaule);
      putString(x0, y0, char_buffer);
    }
    break;
  case 7:
    if (flag == "n")
    {
      if (precision == 1)
        sprintf(char_buffer, "%7.1f", vaule);
      putString(x0, y0, char_buffer);
      if (precision == 2)
        sprintf(char_buffer, "%7.2f", vaule);
      putString(x0, y0, char_buffer);
      if (precision == 3)
        sprintf(char_buffer, "%7.3f", vaule);
      putString(x0, y0, char_buffer);
      if (precision == 4)
        sprintf(char_buffer, "%7.4f", vaule);
      putString(x0, y0, char_buffer);
    }
    else if (flag == "-")
    {
      if (precision == 1)
        sprintf(char_buffer, "%-7.1f", vaule);
      putString(x0, y0, char_buffer);
      if (precision == 2)
        sprintf(char_buffer, "%-7.2f", vaule);
      putString(x0, y0, char_buffer);
      if (precision == 3)
        sprintf(char_buffer, "%-7.3f", vaule);
      putString(x0, y0, char_buffer);
      if (precision == 4)
        sprintf(char_buffer, "%-7.4f", vaule);
      putString(x0, y0, char_buffer);
    }
    else if (flag == "+")
    {
      if (precision == 1)
        sprintf(char_buffer, "%+7.1f", vaule);
      putString(x0, y0, char_buffer);
      if (precision == 2)
        sprintf(char_buffer, "%+7.2f", vaule);
      putString(x0, y0, char_buffer);
      if (precision == 3)
        sprintf(char_buffer, "%+7.3f", vaule);
      putString(x0, y0, char_buffer);
      if (precision == 4)
        sprintf(char_buffer, "%+7.4f", vaule);
      putString(x0, y0, char_buffer);
    }
    else if (flag == "0")
    {
      if (precision == 1)
        sprintf(char_buffer, "%07.1f", vaule);
      putString(x0, y0, char_buffer);
      if (precision == 2)
        sprintf(char_buffer, "%07.2f", vaule);
      putString(x0, y0, char_buffer);
      if (precision == 3)
        sprintf(char_buffer, "%07.3f", vaule);
      putString(x0, y0, char_buffer);
      if (precision == 4)
        sprintf(char_buffer, "%07.4f", vaule);
      putString(x0, y0, char_buffer);
    }
    break;
  case 8:
    if (flag == "n")
    {
      if (precision == 1)
        sprintf(char_buffer, "%8.1f", vaule);
      putString(x0, y0, char_buffer);
      if (precision == 2)
        sprintf(char_buffer, "%8.2f", vaule);
      putString(x0, y0, char_buffer);
      if (precision == 3)
        sprintf(char_buffer, "%8.3f", vaule);
      putString(x0, y0, char_buffer);
      if (precision == 4)
        sprintf(char_buffer, "%8.4f", vaule);
      putString(x0, y0, char_buffer);
    }
    else if (flag == "-")
    {
      if (precision == 1)
        sprintf(char_buffer, "%-8.1f", vaule);
      putString(x0, y0, char_buffer);
      if (precision == 2)
        sprintf(char_buffer, "%-8.2f", vaule);
      putString(x0, y0, char_buffer);
      if (precision == 3)
        sprintf(char_buffer, "%-8.3f", vaule);
      putString(x0, y0, char_buffer);
      if (precision == 4)
        sprintf(char_buffer, "%-8.4f", vaule);
      putString(x0, y0, char_buffer);
    }
    else if (flag == "+")
    {
      if (precision == 1)
        sprintf(char_buffer, "%+8.1f", vaule);
      putString(x0, y0, char_buffer);
      if (precision == 2)
        sprintf(char_buffer, "%+8.2f", vaule);
      putString(x0, y0, char_buffer);
      if (precision == 3)
        sprintf(char_buffer, "%+8.3f", vaule);
      putString(x0, y0, char_buffer);
      if (precision == 4)
        sprintf(char_buffer, "%+8.4f", vaule);
      putString(x0, y0, char_buffer);
    }
    else if (flag == "0")
    {
      if (precision == 1)
        sprintf(char_buffer, "%08.1f", vaule);
      putString(x0, y0, char_buffer);
      if (precision == 2)
        sprintf(char_buffer, "%08.2f", vaule);
      putString(x0, y0, char_buffer);
      if (precision == 3)
        sprintf(char_buffer, "%08.3f", vaule);
      putString(x0, y0, char_buffer);
      if (precision == 4)
        sprintf(char_buffer, "%08.4f", vaule);
      putString(x0, y0, char_buffer);
    }
    break;
  case 9:
    if (flag == "n")
    {
      if (precision == 1)
        sprintf(char_buffer, "%9.1f", vaule);
      putString(x0, y0, char_buffer);
      if (precision == 2)
        sprintf(char_buffer, "%9.2f", vaule);
      putString(x0, y0, char_buffer);
      if (precision == 3)
        sprintf(char_buffer, "%9.3f", vaule);
      putString(x0, y0, char_buffer);
      if (precision == 4)
        sprintf(char_buffer, "%9.4f", vaule);
      putString(x0, y0, char_buffer);
    }
    else if (flag == "-")
    {
      if (precision == 1)
        sprintf(char_buffer, "%-9.1f", vaule);
      putString(x0, y0, char_buffer);
      if (precision == 2)
        sprintf(char_buffer, "%-9.2f", vaule);
      putString(x0, y0, char_buffer);
      if (precision == 3)
        sprintf(char_buffer, "%-9.3f", vaule);
      putString(x0, y0, char_buffer);
      if (precision == 4)
        sprintf(char_buffer, "%-9.4f", vaule);
      putString(x0, y0, char_buffer);
    }
    else if (flag == "+")
    {
      if (precision == 1)
        sprintf(char_buffer, "%+9.1f", vaule);
      putString(x0, y0, char_buffer);
      if (precision == 2)
        sprintf(char_buffer, "%+9.2f", vaule);
      putString(x0, y0, char_buffer);
      if (precision == 3)
        sprintf(char_buffer, "%+9.3f", vaule);
      putString(x0, y0, char_buffer);
      if (precision == 4)
        sprintf(char_buffer, "%+9.4f", vaule);
      putString(x0, y0, char_buffer);
    }
    else if (flag == "0")
    {
      if (precision == 1)
        sprintf(char_buffer, "%09.1f", vaule);
      putString(x0, y0, char_buffer);
      if (precision == 2)
        sprintf(char_buffer, "%09.2f", vaule);
      putString(x0, y0, char_buffer);
      if (precision == 3)
        sprintf(char_buffer, "%09.3f", vaule);
      putString(x0, y0, char_buffer);
      if (precision == 4)
        sprintf(char_buffer, "%09.4f", vaule);
      putString(x0, y0, char_buffer);
    }
    break;
  case 10:
    if (flag == "n")
    {
      if (precision == 1)
        sprintf(char_buffer, "%10.1f", vaule);
      putString(x0, y0, char_buffer);
      if (precision == 2)
        sprintf(char_buffer, "%10.2f", vaule);
      putString(x0, y0, char_buffer);
      if (precision == 3)
        sprintf(char_buffer, "%10.3f", vaule);
      putString(x0, y0, char_buffer);
      if (precision == 4)
        sprintf(char_buffer, "%10.4f", vaule);
      putString(x0, y0, char_buffer);
    }
    else if (flag == "-")
    {
      if (precision == 1)
        sprintf(char_buffer, "%-10.1f", vaule);
      putString(x0, y0, char_buffer);
      if (precision == 2)
        sprintf(char_buffer, "%-10.2f", vaule);
      putString(x0, y0, char_buffer);
      if (precision == 3)
        sprintf(char_buffer, "%-10.3f", vaule);
      putString(x0, y0, char_buffer);
      if (precision == 4)
        sprintf(char_buffer, "%-10.4f", vaule);
      putString(x0, y0, char_buffer);
    }
    else if (flag == "+")
    {
      if (precision == 1)
        sprintf(char_buffer, "%+10.1f", vaule);
      putString(x0, y0, char_buffer);
      if (precision == 2)
        sprintf(char_buffer, "%+10.2f", vaule);
      putString(x0, y0, char_buffer);
      if (precision == 3)
        sprintf(char_buffer, "%+10.3f", vaule);
      putString(x0, y0, char_buffer);
      if (precision == 4)
        sprintf(char_buffer, "%+10.4f", vaule);
      putString(x0, y0, char_buffer);
    }
    else if (flag == "0")
    {
      if (precision == 1)
        sprintf(char_buffer, "%010.1f", vaule);
      putString(x0, y0, char_buffer);
      if (precision == 2)
        sprintf(char_buffer, "%010.2f", vaule);
      putString(x0, y0, char_buffer);
      if (precision == 3)
        sprintf(char_buffer, "%010.3f", vaule);
      putString(x0, y0, char_buffer);
      if (precision == 4)
        sprintf(char_buffer, "%010.4f", vaule);
      putString(x0, y0, char_buffer);
    }
    break;
  case 11:
    if (flag == "n")
    {
      if (precision == 1)
        sprintf(char_buffer, "%11.1f", vaule);
      putString(x0, y0, char_buffer);
      if (precision == 2)
        sprintf(char_buffer, "%11.2f", vaule);
      putString(x0, y0, char_buffer);
      if (precision == 3)
        sprintf(char_buffer, "%11.3f", vaule);
      putString(x0, y0, char_buffer);
      if (precision == 4)
        sprintf(char_buffer, "%11.4f", vaule);
      putString(x0, y0, char_buffer);
    }
    else if (flag == "-")
    {
      if (precision == 1)
        sprintf(char_buffer, "%-11.1f", vaule);
      putString(x0, y0, char_buffer);
      if (precision == 2)
        sprintf(char_buffer, "%-11.2f", vaule);
      putString(x0, y0, char_buffer);
      if (precision == 3)
        sprintf(char_buffer, "%-11.3f", vaule);
      putString(x0, y0, char_buffer);
      if (precision == 4)
        sprintf(char_buffer, "%-11.4f", vaule);
      putString(x0, y0, char_buffer);
    }
    else if (flag == "+")
    {
      if (precision == 1)
        sprintf(char_buffer, "%+11.1f", vaule);
      putString(x0, y0, char_buffer);
      if (precision == 2)
        sprintf(char_buffer, "%+11.2f", vaule);
      putString(x0, y0, char_buffer);
      if (precision == 3)
        sprintf(char_buffer, "%+11.3f", vaule);
      putString(x0, y0, char_buffer);
      if (precision == 4)
        sprintf(char_buffer, "%+11.4f", vaule);
      putString(x0, y0, char_buffer);
    }
    else if (flag == "0")
    {
      if (precision == 1)
        sprintf(char_buffer, "%011.1f", vaule);
      putString(x0, y0, char_buffer);
      if (precision == 2)
        sprintf(char_buffer, "%011.2f", vaule);
      putString(x0, y0, char_buffer);
      if (precision == 3)
        sprintf(char_buffer, "%011.3f", vaule);
      putString(x0, y0, char_buffer);
      if (precision == 4)
        sprintf(char_buffer, "%011.4f", vaule);
      putString(x0, y0, char_buffer);
    }
    break;
  default:
    break;
  }
}
//**************************************************************//
//vaule: 0x00000000 ~ 0xffffffff
//len: 1~11 minimum output length
/*
  [flag] 
    n:¿¿ÓÒ,Ña¿Õ°×¡¡
    #:ŠÖÆÝ”³ö 0x ×÷žéé_î^.
    0:ÔÚé_î^ÌŽ(×ó‚È) Ña 0£¬¶ø·ÇÑa¿Õ°×¡£
    x:ŠÖÆÝ”³ö 0x ×÷žéé_î^£¬Ña 0¡£ 
 */
//**************************************************************//
void Ra8876_Lite::putHex(ru16 x0, ru16 y0, ru32 vaule, ru8 len, const char *flag)
{
  char char_buffer[12];
  switch (len)
  {
  case 1:
    if (flag == "n")
    {
      sprintf(char_buffer, "%1x", vaule);
      putString(x0, y0, char_buffer);
    }
    else if (flag == "0")
    {
      sprintf(char_buffer, "%01x", vaule);
      putString(x0, y0, char_buffer);
    }
    else if (flag == "#")
    {
      sprintf(char_buffer, "%#1x", vaule);
      putString(x0, y0, char_buffer);
    }
    else if (flag == "x")
    {
      sprintf(char_buffer, "%#01x", vaule);
      putString(x0, y0, char_buffer);
    }
    break;
  case 2:
    if (flag == "n")
    {
      sprintf(char_buffer, "%2x", vaule);
      putString(x0, y0, char_buffer);
    }
    else if (flag == "0")
    {
      sprintf(char_buffer, "%02x", vaule);
      putString(x0, y0, char_buffer);
    }
    else if (flag == "#")
    {
      sprintf(char_buffer, "%#2x", vaule);
      putString(x0, y0, char_buffer);
    }
    else if (flag == "x")
    {
      sprintf(char_buffer, "%#02x", vaule);
      putString(x0, y0, char_buffer);
    }
    break;
  case 3:
    if (flag == "n")
    {
      sprintf(char_buffer, "%3x", vaule);
      putString(x0, y0, char_buffer);
    }
    else if (flag == "0")
    {
      sprintf(char_buffer, "%03x", vaule);
      putString(x0, y0, char_buffer);
    }
    else if (flag == "#")
    {
      sprintf(char_buffer, "%#3x", vaule);
      putString(x0, y0, char_buffer);
    }
    else if (flag == "x")
    {
      sprintf(char_buffer, "%#03x", vaule);
      putString(x0, y0, char_buffer);
    }
    break;
  case 4:
    if (flag == "n")
    {
      sprintf(char_buffer, "%4x", vaule);
      putString(x0, y0, char_buffer);
    }
    else if (flag == "0")
    {
      sprintf(char_buffer, "%04x", vaule);
      putString(x0, y0, char_buffer);
    }
    else if (flag == "#")
    {
      sprintf(char_buffer, "%#4x", vaule);
      putString(x0, y0, char_buffer);
    }
    else if (flag == "x")
    {
      sprintf(char_buffer, "%#04x", vaule);
      putString(x0, y0, char_buffer);
    }
    break;
  case 5:
    if (flag == "n")
    {
      sprintf(char_buffer, "%5x", vaule);
      putString(x0, y0, char_buffer);
    }
    else if (flag == "0")
    {
      sprintf(char_buffer, "%05x", vaule);
      putString(x0, y0, char_buffer);
    }
    else if (flag == "#")
    {
      sprintf(char_buffer, "%#5x", vaule);
      putString(x0, y0, char_buffer);
    }
    else if (flag == "x")
    {
      sprintf(char_buffer, "%#05x", vaule);
      putString(x0, y0, char_buffer);
    }
    break;
  case 6:
    if (flag == "n")
    {
      sprintf(char_buffer, "%6x", vaule);
      putString(x0, y0, char_buffer);
    }
    else if (flag == "0")
    {
      sprintf(char_buffer, "%06x", vaule);
      putString(x0, y0, char_buffer);
    }
    else if (flag == "#")
    {
      sprintf(char_buffer, "%#6x", vaule);
      putString(x0, y0, char_buffer);
    }
    else if (flag == "x")
    {
      sprintf(char_buffer, "%#06x", vaule);
      putString(x0, y0, char_buffer);
    }
    break;
  case 7:
    if (flag == "n")
    {
      sprintf(char_buffer, "%7x", vaule);
      putString(x0, y0, char_buffer);
    }
    else if (flag == "0")
    {
      sprintf(char_buffer, "%07x", vaule);
      putString(x0, y0, char_buffer);
    }
    else if (flag == "#")
    {
      sprintf(char_buffer, "%#7x", vaule);
      putString(x0, y0, char_buffer);
    }
    else if (flag == "x")
    {
      sprintf(char_buffer, "%#07x", vaule);
      putString(x0, y0, char_buffer);
    }
    break;
  case 8:
    if (flag == "n")
    {
      sprintf(char_buffer, "%8x", vaule);
      putString(x0, y0, char_buffer);
    }
    else if (flag == "0")
    {
      sprintf(char_buffer, "%08x", vaule);
      putString(x0, y0, char_buffer);
    }
    else if (flag == "#")
    {
      sprintf(char_buffer, "%#8x", vaule);
      putString(x0, y0, char_buffer);
    }
    else if (flag == "x")
    {
      sprintf(char_buffer, "%#08x", vaule);
      putString(x0, y0, char_buffer);
    }
    break;
  case 9:
    if (flag == "n")
    {
      sprintf(char_buffer, "%9x", vaule);
      putString(x0, y0, char_buffer);
    }
    else if (flag == "0")
    {
      sprintf(char_buffer, "%09x", vaule);
      putString(x0, y0, char_buffer);
    }
    else if (flag == "#")
    {
      sprintf(char_buffer, "%#9x", vaule);
      putString(x0, y0, char_buffer);
    }
    else if (flag == "x")
    {
      sprintf(char_buffer, "%#09x", vaule);
      putString(x0, y0, char_buffer);
    }
    break;
  case 10:
    if (flag == "n")
    {
      sprintf(char_buffer, "%10x", vaule);
      putString(x0, y0, char_buffer);
    }
    else if (flag == "0")
    {
      sprintf(char_buffer, "%010x", vaule);
      putString(x0, y0, char_buffer);
    }
    else if (flag == "#")
    {
      sprintf(char_buffer, "%#10x", vaule);
      putString(x0, y0, char_buffer);
    }
    else if (flag == "x")
    {
      sprintf(char_buffer, "%#010x", vaule);
      putString(x0, y0, char_buffer);
    }
    break;

  default:
    break;
  }
}
/*draw function*/
//**************************************************************//
//**************************************************************//
void Ra8876_Lite::drawLine(ru16 x0, ru16 y0, ru16 x1, ru16 y1, ru16 color)
{
  foreGroundColor16bpp(color);
  lcdRegDataWrite(RA8876_DLHSR0, x0);             //68h
  lcdRegDataWrite(RA8876_DLHSR1, x0 >> 8);        //69h
  lcdRegDataWrite(RA8876_DLVSR0, y0);             //6ah
  lcdRegDataWrite(RA8876_DLVSR1, y0 >> 8);        //6bh
  lcdRegDataWrite(RA8876_DLHER0, x1);             //6ch
  lcdRegDataWrite(RA8876_DLHER1, x1 >> 8);        //6dh
  lcdRegDataWrite(RA8876_DLVER0, y1);             //6eh
  lcdRegDataWrite(RA8876_DLVER1, y1 >> 8);        //6fh
  lcdRegDataWrite(RA8876_DCR0, RA8876_DRAW_LINE); //67h,0x80
  check2dBusy();
}
//**************************************************************//
//**************************************************************//
void Ra8876_Lite::drawSquare(ru16 x0, ru16 y0, ru16 x1, ru16 y1, ru16 color)
{
  foreGroundColor16bpp(color);
  lcdRegDataWrite(RA8876_DLHSR0, x0);               //68h
  lcdRegDataWrite(RA8876_DLHSR1, x0 >> 8);          //69h
  lcdRegDataWrite(RA8876_DLVSR0, y0);               //6ah
  lcdRegDataWrite(RA8876_DLVSR1, y0 >> 8);          //6bh
  lcdRegDataWrite(RA8876_DLHER0, x1);               //6ch
  lcdRegDataWrite(RA8876_DLHER1, x1 >> 8);          //6dh
  lcdRegDataWrite(RA8876_DLVER0, y1);               //6eh
  lcdRegDataWrite(RA8876_DLVER1, y1 >> 8);          //6fh
  lcdRegDataWrite(RA8876_DCR1, RA8876_DRAW_SQUARE); //76h,0xa0
  check2dBusy();
}
//**************************************************************//
//**************************************************************//
void Ra8876_Lite::drawSquareFill(ru16 x0, ru16 y0, ru16 x1, ru16 y1, ru16 color)
{
  foreGroundColor16bpp(color);
  lcdRegDataWrite(RA8876_DLHSR0, x0);                    //68h
  lcdRegDataWrite(RA8876_DLHSR1, x0 >> 8);               //69h
  lcdRegDataWrite(RA8876_DLVSR0, y0);                    //6ah
  lcdRegDataWrite(RA8876_DLVSR1, y0 >> 8);               //6bh
  lcdRegDataWrite(RA8876_DLHER0, x1);                    //6ch
  lcdRegDataWrite(RA8876_DLHER1, x1 >> 8);               //6dh
  lcdRegDataWrite(RA8876_DLVER0, y1);                    //6eh
  lcdRegDataWrite(RA8876_DLVER1, y1 >> 8);               //6fh
  lcdRegDataWrite(RA8876_DCR1, RA8876_DRAW_SQUARE_FILL); //76h,0xa0
  check2dBusy();
}
//**************************************************************//
//**************************************************************//
void Ra8876_Lite::drawCircleSquare(ru16 x0, ru16 y0, ru16 x1, ru16 y1, ru16 xr, ru16 yr, ru16 color)
{
  foreGroundColor16bpp(color);
  lcdRegDataWrite(RA8876_DLHSR0, x0);                      //68h
  lcdRegDataWrite(RA8876_DLHSR1, x0 >> 8);                 //69h
  lcdRegDataWrite(RA8876_DLVSR0, y0);                      //6ah
  lcdRegDataWrite(RA8876_DLVSR1, y0 >> 8);                 //6bh
  lcdRegDataWrite(RA8876_DLHER0, x1);                      //6ch
  lcdRegDataWrite(RA8876_DLHER1, x1 >> 8);                 //6dh
  lcdRegDataWrite(RA8876_DLVER0, y1);                      //6eh
  lcdRegDataWrite(RA8876_DLVER1, y1 >> 8);                 //6fh
  lcdRegDataWrite(RA8876_ELL_A0, xr);                      //77h
  lcdRegDataWrite(RA8876_ELL_A1, xr >> 8);                 //79h
  lcdRegDataWrite(RA8876_ELL_B0, yr);                      //7ah
  lcdRegDataWrite(RA8876_ELL_B1, yr >> 8);                 //7bh
  lcdRegDataWrite(RA8876_DCR1, RA8876_DRAW_CIRCLE_SQUARE); //76h,0xb0
  check2dBusy();
}
//**************************************************************//
//**************************************************************//
void Ra8876_Lite::drawCircleSquareFill(ru16 x0, ru16 y0, ru16 x1, ru16 y1, ru16 xr, ru16 yr, ru16 color)
{
  foreGroundColor16bpp(color);
  lcdRegDataWrite(RA8876_DLHSR0, x0);                           //68h
  lcdRegDataWrite(RA8876_DLHSR1, x0 >> 8);                      //69h
  lcdRegDataWrite(RA8876_DLVSR0, y0);                           //6ah
  lcdRegDataWrite(RA8876_DLVSR1, y0 >> 8);                      //6bh
  lcdRegDataWrite(RA8876_DLHER0, x1);                           //6ch
  lcdRegDataWrite(RA8876_DLHER1, x1 >> 8);                      //6dh
  lcdRegDataWrite(RA8876_DLVER0, y1);                           //6eh
  lcdRegDataWrite(RA8876_DLVER1, y1 >> 8);                      //6fh
  lcdRegDataWrite(RA8876_ELL_A0, xr);                           //77h
  lcdRegDataWrite(RA8876_ELL_A1, xr >> 8);                      //78h
  lcdRegDataWrite(RA8876_ELL_B0, yr);                           //79h
  lcdRegDataWrite(RA8876_ELL_B1, yr >> 8);                      //7ah
  lcdRegDataWrite(RA8876_DCR1, RA8876_DRAW_CIRCLE_SQUARE_FILL); //76h,0xf0
  check2dBusy();
}
//**************************************************************//
//**************************************************************//
void Ra8876_Lite::drawTriangle(ru16 x0, ru16 y0, ru16 x1, ru16 y1, ru16 x2, ru16 y2, ru16 color)
{
  foreGroundColor16bpp(color);
  lcdRegDataWrite(RA8876_DLHSR0, x0);                 //68h
  lcdRegDataWrite(RA8876_DLHSR1, x0 >> 8);            //69h
  lcdRegDataWrite(RA8876_DLVSR0, y0);                 //6ah
  lcdRegDataWrite(RA8876_DLVSR1, y0 >> 8);            //6bh
  lcdRegDataWrite(RA8876_DLHER0, x1);                 //6ch
  lcdRegDataWrite(RA8876_DLHER1, x1 >> 8);            //6dh
  lcdRegDataWrite(RA8876_DLVER0, y1);                 //6eh
  lcdRegDataWrite(RA8876_DLVER1, y1 >> 8);            //6fh
  lcdRegDataWrite(RA8876_DTPH0, x2);                  //70h
  lcdRegDataWrite(RA8876_DTPH1, x2 >> 8);             //71h
  lcdRegDataWrite(RA8876_DTPV0, y2);                  //72h
  lcdRegDataWrite(RA8876_DTPV1, y2 >> 8);             //73h
  lcdRegDataWrite(RA8876_DCR0, RA8876_DRAW_TRIANGLE); //67h,0x82
  check2dBusy();
}
//**************************************************************//
//**************************************************************//
void Ra8876_Lite::drawTriangleFill(ru16 x0, ru16 y0, ru16 x1, ru16 y1, ru16 x2, ru16 y2, ru16 color)
{
  foreGroundColor16bpp(color);
  lcdRegDataWrite(RA8876_DLHSR0, x0);                      //68h
  lcdRegDataWrite(RA8876_DLHSR1, x0 >> 8);                 //69h
  lcdRegDataWrite(RA8876_DLVSR0, y0);                      //6ah
  lcdRegDataWrite(RA8876_DLVSR1, y0 >> 8);                 //6bh
  lcdRegDataWrite(RA8876_DLHER0, x1);                      //6ch
  lcdRegDataWrite(RA8876_DLHER1, x1 >> 8);                 //6dh
  lcdRegDataWrite(RA8876_DLVER0, y1);                      //6eh
  lcdRegDataWrite(RA8876_DLVER1, y1 >> 8);                 //6fh
  lcdRegDataWrite(RA8876_DTPH0, x2);                       //70h
  lcdRegDataWrite(RA8876_DTPH1, x2 >> 8);                  //71h
  lcdRegDataWrite(RA8876_DTPV0, y2);                       //72h
  lcdRegDataWrite(RA8876_DTPV1, y2 >> 8);                  //73h
  lcdRegDataWrite(RA8876_DCR0, RA8876_DRAW_TRIANGLE_FILL); //67h,0xa2
  check2dBusy();
}
//**************************************************************//
//**************************************************************//
void Ra8876_Lite::drawCircle(ru16 x0, ru16 y0, ru16 r, ru16 color)
{
  foreGroundColor16bpp(color);
  lcdRegDataWrite(RA8876_DEHR0, x0);                //7bh
  lcdRegDataWrite(RA8876_DEHR1, x0 >> 8);           //7ch
  lcdRegDataWrite(RA8876_DEVR0, y0);                //7dh
  lcdRegDataWrite(RA8876_DEVR1, y0 >> 8);           //7eh
  lcdRegDataWrite(RA8876_ELL_A0, r);                //77h
  lcdRegDataWrite(RA8876_ELL_A1, r >> 8);           //78h
  lcdRegDataWrite(RA8876_ELL_B0, r);                //79h
  lcdRegDataWrite(RA8876_ELL_B1, r >> 8);           //7ah
  lcdRegDataWrite(RA8876_DCR1, RA8876_DRAW_CIRCLE); //76h,0x80
  check2dBusy();
}
//**************************************************************//
//**************************************************************//
void Ra8876_Lite::drawCircleFill(ru16 x0, ru16 y0, ru16 r, ru16 color)
{
  foreGroundColor16bpp(color);
  lcdRegDataWrite(RA8876_DEHR0, x0);                     //7bh
  lcdRegDataWrite(RA8876_DEHR1, x0 >> 8);                //7ch
  lcdRegDataWrite(RA8876_DEVR0, y0);                     //7dh
  lcdRegDataWrite(RA8876_DEVR1, y0 >> 8);                //7eh
  lcdRegDataWrite(RA8876_ELL_A0, r);                     //77h
  lcdRegDataWrite(RA8876_ELL_A1, r >> 8);                //78h
  lcdRegDataWrite(RA8876_ELL_B0, r);                     //79h
  lcdRegDataWrite(RA8876_ELL_B1, r >> 8);                //7ah
  lcdRegDataWrite(RA8876_DCR1, RA8876_DRAW_CIRCLE_FILL); //76h,0xc0
  check2dBusy();
}
//**************************************************************//
//**************************************************************//
void Ra8876_Lite::drawEllipse(ru16 x0, ru16 y0, ru16 xr, ru16 yr, ru16 color)
{
  foreGroundColor16bpp(color);
  lcdRegDataWrite(RA8876_DEHR0, x0);                 //7bh
  lcdRegDataWrite(RA8876_DEHR1, x0 >> 8);            //7ch
  lcdRegDataWrite(RA8876_DEVR0, y0);                 //7dh
  lcdRegDataWrite(RA8876_DEVR1, y0 >> 8);            //7eh
  lcdRegDataWrite(RA8876_ELL_A0, xr);                //77h
  lcdRegDataWrite(RA8876_ELL_A1, xr >> 8);           //78h
  lcdRegDataWrite(RA8876_ELL_B0, yr);                //79h
  lcdRegDataWrite(RA8876_ELL_B1, yr >> 8);           //7ah
  lcdRegDataWrite(RA8876_DCR1, RA8876_DRAW_ELLIPSE); //76h,0x80
  check2dBusy();
}

void Ra8876_Lite::writeGT9271TouchRegister(uint16_t regAddr, uint8_t *val, uint16_t cnt)
{
  uint16_t i = 0;
  i2c_start(bus);
  i2c_writeByte(bus, i2c_addr_write);
  i2c_writeByte(bus, regAddr >> 8);
  i2c_writeByte(bus, regAddr);
  print("entering loop\n");
  for (i = 0; i < cnt; i++, val++)
  {
    print("writing:%d\n", *val);
    i2c_writeByte(bus, *val);
  }
  i2c_stop(bus);
}

uint8_t Ra8876_Lite::gt9271_Send_Cfg(uint8_t *buf, uint16_t cfg_len)
{
  writeGT9271TouchRegister(0x8047, buf, cfg_len);
  pause(10);
}

uint8_t Ra8876_Lite::readGT9271TouchAddr(uint16_t regAddr, uint8_t *pBuf, uint8_t len)
{
  uint8_t i;
  i2c_start(bus);

  i2c_writeByte(bus, i2c_addr_write);
  i2c_writeByte(bus, regAddr >> 8);
  i2c_writeByte(bus, regAddr);
  i2c_start(bus);
  print("length: %d\n", len);
  i2c_writeByte(bus, i2c_addr_read);
  for (i = 0; i < len; i++)
  {
    if (i == (len - 1))
    {
      pBuf[i] = i2c_readByte(bus, 1);
      print("endbuf %d: '%u'\n", i, pBuf[i]);
      break;
    }
    pBuf[i] = i2c_readByte(bus, 0);
    print("buf %d: '%u'\n", i, pBuf[i]);
  }
  i2c_stop(bus);
  return i;
}

uint8_t Ra8876_Lite::readGT9271TouchLocation(TouchLocation *pLoc, uint8_t num)
{
  uint8_t retVal = 0;
  uint8_t i = 0;
  uint8_t k = 0;
  uint8_t ss[1];
  do
  {

    if (!pLoc)
      break; // must have a buffer
    if (!num)
      break; // must be able to take at least one
    ss[0] = 0;
    readGT9271TouchAddr(0x814e, ss, 1);
    uint8_t status = ss[0];
    print("status: %u\n", status);
    if ((status & 0x0f) == 0)
      break; // no points detected
    uint8_t hitPoints = status & 0x0f;

    print("number of hit points = %d\n", hitPoints);

    uint8_t tbuf[32];
    uint8_t tbuf1[32];
    uint8_t tbuf2[16];
    readGT9271TouchAddr(0x8150, tbuf, 32);
    readGT9271TouchAddr(0x8150 + 32, tbuf1, 32);
    readGT9271TouchAddr(0x8150 + 64, tbuf2, 16);

    if (hitPoints <= 4)
    {
      for (k = 0, i = 0;
           (i < 4 * 8) && (k < num); k++, i += 8)
      {
        pLoc[k].x = tbuf[i + 1] << 8 | tbuf[i + 0];
        pLoc[k].y = tbuf[i + 3] << 8 | tbuf[i + 2];
      }
    }
    if (hitPoints > 4)
    {
      for (k = 0, i = 0;
           (i < 4 * 8) && (k < num); k++, i += 8)
      {
        pLoc[k].x = tbuf[i + 1] << 8 | tbuf[i + 0];
        pLoc[k].y = tbuf[i + 3] << 8 | tbuf[i + 2];
      }

      for (k = 4, i = 0;
           (i < 4 * 8) && (k < num); k++, i += 8)
      {
        pLoc[k].x = tbuf1[i + 1] << 8 | tbuf1[i + 0];
        pLoc[k].y = tbuf1[i + 3] << 8 | tbuf1[i + 2];
      }
    }

    if (hitPoints > 8)
    {
      for (k = 0, i = 0;
           (i < 4 * 8) && (k < num); k++, i += 8)
      {
        pLoc[k].x = tbuf[i + 1] << 8 | tbuf[i + 0];
        pLoc[k].y = tbuf[i + 3] << 8 | tbuf[i + 2];
      }

      for (k = 4, i = 0;
           (i < 4 * 8) && (k < num); k++, i += 8)
      {
        pLoc[k].x = tbuf1[i + 1] << 8 | tbuf1[i + 0];
        pLoc[k].y = tbuf1[i + 3] << 8 | tbuf1[i + 2];
      }

      for (k = 8, i = 0;
           (i < 2 * 8) && (k < num); k++, i += 8)
      {
        pLoc[k].x = tbuf2[i + 1] << 8 | tbuf2[i + 0];
        pLoc[k].y = tbuf2[i + 3] << 8 | tbuf2[i + 2];
      }
    }

    retVal = hitPoints;

  } while (0);

  ss[0] = 0;
  writeGT9271TouchRegister(0x814e, ss, 1);

  return retVal;
}
