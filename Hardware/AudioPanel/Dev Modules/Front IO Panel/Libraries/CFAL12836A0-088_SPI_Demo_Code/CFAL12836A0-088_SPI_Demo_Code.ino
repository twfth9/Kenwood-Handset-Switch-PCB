//===========================================================================
//
//  Code written for Seeeduino v4.2 set to 3.3v (important!)
//
//  CRYSTALFONTZ CFAL12836A0-088 128x36 COLOR 0.88" OLED
//
//  This code uses the 4-wire SPI mode of the display. Actually only 3 wires
//  since MISO is unused.
//
//  ref: https://www.crystalfontz.com/product/cfal12836A0-088
//
//  video: https://www.youtube.com/watch?v=AT-oL1UojrA
//
//  2017 - 01 - 19 Brent A. Crosby
//===========================================================================
//This is free and unencumbered software released into the public domain.
//
//Anyone is free to copy, modify, publish, use, compile, sell, or
//distribute this software, either in source code form or as a compiled
//binary, for any purpose, commercial or non-commercial, and by any
//means.
//
//In jurisdictions that recognize copyright laws, the author or authors
//of this software dedicate any and all copyright interest in the
//software to the public domain. We make this dedication for the benefit
//of the public at large and to the detriment of our heirs and
//successors. We intend this dedication to be an overt act of
//relinquishment in perpetuity of all present and future rights to this
//software under copyright law.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
//EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
//MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
//IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
//OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
//ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
//OTHER DEALINGS IN THE SOFTWARE.
//
//For more information, please refer to <http://unlicense.org/>
//============================================================================
//
// Display is Crystalfontz CFAL12836A0-088
//   https://www.crystalfontz.com/product/cfal12836A0088
//
// The controller is a LDT LD7138
//   https://www.crystalfontz.com/controllers/LDT/LD7138/
//
//============================================================================
#include <avr/io.h>
#include <SPI.h>
// C:\Program Files (x86)\Arduino\hardware\arduino\avr\libraries\SPI\src\SPI.cpp
// C:\Program Files (x86)\Arduino\hardware\arduino\avr\libraries\SPI\src\SPI.h
#include <util/delay.h>
#include <avr/pgmspace.h>

//Set BUILD_SD to enable a slide show from the uSD card to the OLED.
//Files need to be Windows 128x36 24-bit BMP, file size will be
//exactly 13,880 bytes.
#define BUILD_SD (1)

#if BUILD_SD
#include <SD.h>
// C:\Program Files (x86)\Arduino\libraries\SD\src\SD.cpp
// C:\Program Files (x86)\Arduino\libraries\SD\src\SD.h

/* You might want to make these changes to the SD.cpp library
   function SDClass::begin() to allow multiple calls
  
//ref: https://github.com/arduino-libraries/SD/issues/5
// ----> Line Added to beginning of SDClass::begin
  if (root.isOpen()) root.close();      // allows repeated calls
// <---- Line Added

And this change to make speed reasonable:

  //  return card.init(SPI_HALF_SPEED, csPin) &&
  //         volume.init(card) &&
  //         root.openRoot(volume);
  //
  // to:
  //
  //  return card.init(SPI_FULL_SPEED, csPin) &&
  //         volume.init(card) &&
  //         root.openRoot(volume);

*/
#endif
//============================================================================
//CFA-10082 0v2
// OLED SPI & control lines
//   ARD      | Port | OLED
// -----------+------+-------------------------
//  #6/D6     |  PD6 | OLED_RESET_NOT
//  #7/D7     |  PD7 | VCC_EN
//  #8/D8     |  PB0 | OLED_RS
//  #9/D9     |  PB1 | OLED_CS_NOT
// #10/D10    |  PB2 | SD_CS_NOT
// #11/D11    |  PB3 | MOSI (hardware SPI)
// #12/D12    |  PB4 | MISO (hardware SPI)
// #13/D13    |  PB5 | SCK  (hardware SPI)
#define SD_CS 10

#define CLR_RESET    (PORTD &= ~(0x40))
#define SET_RESET    (PORTD |=  (0x40))
#define DISABLE_VCC  (PORTD &= ~(0x80))
#define ENABLE_VCC   (PORTD |=  (0x80))

#define CLR_RS       (PORTB &= ~(0x01))
#define SET_RS       (PORTB |=  (0x01))
#define CLR_CS       (PORTB &= ~(0x02))
#define SET_CS       (PORTB |=  (0x02))
#define CLR_SD_CS    (PORTB &= ~(0x04))
#define SET_SD_CS    (PORTB |=  (0x04))
#define CLR_MOSI     (PORTB &= ~(0x08))
#define SET_MOSI     (PORTB |=  (0x08))
//#define CLR_MISO     (PORTB &= ~(0x10))  //Unused
//#define SET_MISO     (PORTB |=  (0x10))  //Unused
#define CLR_SCK      (PORTB &= ~(0x20))
#define SET_SCK      (PORTB |=  (0x20))
//============================================================================
void SPI_sendCommand(uint8_t command)
  {
  // Select the OLED's command register
  CLR_RS;
  // Select the OLED controller
  CLR_CS;
  //Send the command via SPI:
  SPI.transfer(command);
  // Deselect the OLED controller
  SET_CS;
  }
//----------------------------------------------------------------------------
void SPI_sendData(uint8_t data)
  {
  // Select the OLED's data register
  SET_RS;
  // Select the OLED controller
  CLR_CS;
  //Send the data via SPI:
  SPI.transfer(data);
  // Deselect the OLED controller
  SET_CS;
  }
//============================================================================
// Defines for the LD7138 registers.
// ref: https://www.crystalfontz.com/controllers/LDT/LD7138/
#define LD7138_0x01_SOFTRES                (0x01)
#define LD7138_0x02_DISPLAY_ON_OFF         (0x02)
#define LD7138_0x03_DISPLAY_STANDBY_ON_OFF (0x03)
#define LD7138_0x04_OSCILLATOR_SPEED       (0x04)
#define LD7138_0x05_WRITE_DIRECTION        (0x05)
#define LD7138_0x06_SCAN_DIRECTION         (0x06)
#define LD7138_0x07_SET_DISPLAY_WINDOW     (0x07)
#define LD7138_0x08_IF_BUS_SEL             (0x08)
#define LD7138_0x09_DATA_MASKING           (0x09)
#define LD7138_0x0A_SET_DATA_WINDOW        (0x0A)
#define LD7138_0x0B_SET_ADDRESS            (0x0B)
#define LD7138_0x0C_DATA_WRITE_READ        (0x0C)
#define LD7138_0x0D_REGISTER_READ          (0x0D)
#define LD7138_0x0E_RGB_CURRENT_LEVEL      (0x0E)
#define LD7138_0x0F_PEAK_CURRENT_LEVEL     (0x0F)
#define LD7138_0x10_SCLK                   (0x10)
#define LD7138_0x1C_PRE_CHARGE_WIDTH       (0x1C)
#define LD7138_0x1C_SET_PEAK_WIDTH         (0x1D)
#define LD7138_0x1E_SET_PEAK_DELAY         (0x1E)
#define LD7138_0x1F_SET_ROW_SCAN           (0x1F)
#define LD7138_0x30_VCC_R_SELECT           (0x30)
#define LD7138_0x34_RGB_MODE               (0x34)
#define LD7138_0x3A_GAMMA_TUNE             (0x3A)
#define LD7138_0x3B_GAMMA_INIT             (0x3B)
#define LD7138_0x3C_SET_VDD_SELECTION      (0x3C)
#define LD7138_0x3E_TEST                   (0x3E)
//============================================================================
void Set_OLED_for_write_at_X_Y(uint8_t x, uint8_t y)
  {
  // Select the OLED controller
  CLR_CS;
  // Select the OLED's command register
  CLR_RS;
  //Send the command via SPI:
  SPI.transfer(LD7138_0x0A_SET_DATA_WINDOW);
    // Select the OLED's data register
    SET_RS;
    //Xstart, bits 6,5,4 right justified
    SPI.transfer((x>>4)&0x07);
    //Xstart, bits 3,2,1,0 right justified
    SPI.transfer(x&0x0F);
    #define XEND (127)
    //Xend, bits 6,5,4 right justified
    SPI.transfer((XEND>>4)&0x07);
    //Xend, bits 3,2,1,0 right justified
    SPI.transfer(XEND&0x0F);
    y+=60;
    //Ystart, bits 6,5,4 right justified
    SPI.transfer((y>>4)&0x07);
    //Ystart, bits 3,2,1,0 right justified
    SPI.transfer(y&0x0F);
    #define YEND (95)
    //Yend, bits 6,5,4 right justified
    SPI.transfer((YEND>>4)&0x07);
    //Yend, bits 3,2,1,0 right justified
    SPI.transfer(YEND&0x0F);
    #undef XEND
    #undef YEND
    
  // Select the OLED's command register
  CLR_RS;
  //Send the command via SPI:
  SPI.transfer(LD7138_0x0C_DATA_WRITE_READ);
  // Data will be sent by calling function
  // Deselect the OLED controller
  SET_CS;  
  }
//============================================================================
// ~19.15mS
void Fill_OLED(uint8_t r, uint8_t g, uint8_t b)
  {
  uint16_t
    i;
  Set_OLED_for_write_at_X_Y(0, 0);

  //Pre-calculate the two bytes for this color of pixel
  uint8_t
    first_half;
  uint8_t
    second_half;
  //The display takes two bytes (565) RRRRR GGGGGG BBBBB 
  //to show one pixel.
  first_half=(r&0xF8) | (g >> 5);
  second_half=((g << 3)&0xE0) | (b >> 3);

  // Select the OLED controller
  CLR_CS;
  // Select the OLED's data register
  SET_RS;

  //Fill display with a given RGB value
  for (i = 0; i < (128*36); i++)
    {
    SPI.transfer(first_half);
    SPI.transfer(second_half);
    }
  // Deselect the OLED controller
  SET_CS;    
  }
//============================================================================
// CURRENTS
//Original spec:
//   Red   0x5A = 90
//   Green 0x53 = 83
//   Blue  0x45 = 69
//Better color balance:
#define IRED  (135)
#define IGREEN (54)
#define IBLUE  (50)
//----------------------------------------------------------------------------
const uint8_t red_gamma[32] PROGMEM = {
    0,  2,  2,  2,  2,  4,  6,  8,
   10, 14, 18, 22, 28, 34, 40, 48,
   56, 64, 74, 84, 94,104,116,128,
  142,154,168,184,198,215,232,248};
const uint8_t green_gamma[64] PROGMEM = {
    0, 96, 96, 96, 96, 96, 96, 96,
   96, 96, 96, 98, 98, 98, 98,100,
  100,100,102,102,104,104,106,106,
  108,110,110,112,114,116,118,120,
  122,124,126,128,132,134,138,140,
  144,148,150,154,158,162,166,170,
  176,180,184,188,194,198,204,208,
  214,220,224,230,236,242,248,254};
const uint8_t blue_gamma[32] PROGMEM = {
    0,  8,  8,  8,  8,  8,  8, 10,
   10, 12, 12, 14, 14, 16, 18, 20,
   22, 24, 28, 30, 34, 40, 46, 54,
   62, 74, 90,110,134,164,200,240};
//----------------------------------------------------------------------------
void Initialize_OLED(void)
  {
  //Reset the OLED controller by hardware
  CLR_RESET;
  delay(1);
  SET_RESET;
  delay(100);
  
  // Software Reset(0x01h)
  // All registers are initialized with default value without altering
  // the graphic RAM.
  // All Dot display are turned OFF.
  // The OSC is stopped.
  SPI_sendCommand(LD7138_0x01_SOFTRES);
  // Set Dot Matrix Display ON/OFF (0x02h)
  //   P0 = 0: indicates the dot matrix display turns OFF(Default).
  //   P0 = 1: indicates the dot matrix Display turns ON.
  // NOTE: Display OFF means:
  //   * All Column Output go to pre-charge level.
  //   * All Row Output go to the ground level
  //   * Stop Data transfer from memory to Dot Matrix Driver.  
  SPI_sendCommand(LD7138_0x02_DISPLAY_ON_OFF);
    SPI_sendData(0x00);   //0 = Off, 1 = On
  // Set Dot Matrix Display Stand-by ON/OFF(0x03h)
  //   P0=0: Indicates the dot oscillator is starting. And it does not make
  //         the dot matrix display turn ON.
  //   P0=1: Indicates the dot oscillator is stopping. And it make the dot
  //         matrix display OFF.
  // NOTE: After software or hardware reset command is executed, it makes
  //   dot matrix display stand-by ON.
  SPI_sendCommand(LD7138_0x03_DISPLAY_STANDBY_ON_OFF);
    SPI_sendData(0x00);  //0 = oscillator running
                         //1 = oscillator stopped/standby
    // Set OSC Control (0x04h)
  SPI_sendCommand(LD7138_0x04_OSCILLATOR_SPEED);
    SPI_sendData(0x02); // 90Hz
    // ---- MFFF
    //      ||||-- FFF:
    //      |       000 60Hz      
    //      |       001 75Hz      
    //      |       010 90Hz *   
    //      |       011 105Hz      
    //      |       100 120Hz      
    //      |----- Mode:
    //              0 = Internal RC
    //              1 = External Clock
  // Set Graphic RAM Writing Direction (0x05h)
  SPI_sendCommand(LD7138_0x05_WRITE_DIRECTION);
    SPI_sendData(0x00);
    // ---- RDDD
    //      ||||-- DDD: See page 22 in datasheet
    //      |       Sets the up/down right left address
    //      |       increment order
    //      |----- Color Order:
    //              0 = RGB
    //              1 = BGR
  // Set Row Scan Direction (0x06h)
  SPI_sendCommand(LD7138_0x06_SCAN_DIRECTION);
    SPI_sendData(0x00);  //0 = min to max / increment
                         //1 = max to min / decrement

  // Set Display Size (0x07h)
  SPI_sendCommand(LD7138_0x07_SET_DISPLAY_WINDOW);
    #define XSTART (0)
    //Xstart, bits 6,5,4 right justified
    SPI_sendData((XSTART>>4)&0x07);
    //Xstart, bits 3,2,1,0 right justified
    SPI_sendData(XSTART&0x0F);
    #define XEND (127)
    //Xend, bits 6,5,4 right justified
    SPI_sendData((XEND>>4)&0x07);
    //Xend, bits 3,2,1,0 right justified
    SPI_sendData(XEND&0x0F);
    #define YSTART (60)
    //Ystart, bits 6,5,4 right justified
    SPI_sendData((YSTART>>4)&0x07);
    //Ystart, bits 3,2,1,0 right justified
    SPI_sendData(YSTART&0x0F);
    #define YEND (95)
    //Yend, bits 6,5,4 right justified
    SPI_sendData((YEND>>4)&0x07);
    //Yend, bits 3,2,1,0 right justified
    SPI_sendData(YEND&0x0F);
    #undef XSTART
    #undef XEND
    #undef YSTART
    #undef YEND

  // Set Interface Bus Type (0x08h)
  SPI_sendCommand(LD7138_0x08_IF_BUS_SEL);
    SPI_sendData(0x01);
    // ---- --II
    //        ||-- II:
    //               0 0 = 6 bit
    //               0 1 = 8 bit *
    //               1 1 = 16 bit (invalid for SPI/I2C)

  // Set Masking Data (0x09h)
  SPI_sendCommand(LD7138_0x09_DATA_MASKING);
    SPI_sendData(0x07);
    // ---I -RGB
    //    |  |||-- RGB: Show this data
    //    |------- Invert shown data

  // Set Read/WriteBoxData (0x0Ah)
  SPI_sendCommand(LD7138_0x0A_SET_DATA_WINDOW);
    #define XSTART (0)
    //Xstart, bits 6,5,4 right justified
    SPI_sendData((XSTART>>4)&0x07);
    //Xstart, bits 3,2,1,0 right justified
    SPI_sendData(XSTART&0x0F);
    #define XEND (127)
    //Xend, bits 6,5,4 right justified
    SPI_sendData((XEND>>4)&0x07);
    //Xend, bits 3,2,1,0 right justified
    SPI_sendData(XEND&0x0F);
    #define YSTART (60)
    //Ystart, bits 6,5,4 right justified
    SPI_sendData((YSTART>>4)&0x07);
    //Ystart, bits 3,2,1,0 right justified
    SPI_sendData(YSTART&0x0F);
    #define YEND (95)
    //Yend, bits 6,5,4 right justified
    SPI_sendData((YEND>>4)&0x07);
    //Yend, bits 3,2,1,0 right justified
    SPI_sendData(YEND&0x0F);
    #undef XSTART
    #undef XEND
    #undef YSTART
    #undef YEND
  // Set Display Start Address(0x0Bh)
  SPI_sendCommand(LD7138_0x0B_SET_ADDRESS);
    #define XSTART (0)
    //Xstart, bits 6,5,4 right justified
    SPI_sendData((XSTART>>4)&0x07);
    //Xstart, bits 3,2,1,0 right justified
    SPI_sendData(XSTART&0x0F);
    #define YSTART (0)
    //Ystart, bits 6,5,4 right justified
    SPI_sendData((YSTART>>4)&0x07);
    //Ystart, bits 3,2,1,0 right justified
    SPI_sendData(YSTART&0x0F);
    #undef XSTART
    #undef YSTART
  // Set Dot Matrix Current Level (0x0Eh)
  // 1uA per step
  SPI_sendCommand(LD7138_0x0E_RGB_CURRENT_LEVEL);
    //IRED, bits 7,6,5,4 right justified
    SPI_sendData((IRED>>4)&0x0F);
    //IRED, bits 3,2,1,0 right justified
    SPI_sendData(IRED&0x0F);
    //IGREEN, bits 7,6,5,4 right justified
    SPI_sendData((IGREEN>>4)&0x0F);
    //IGREEN, bits 3,2,1,0 right justified
    SPI_sendData(IGREEN&0x0F);
    //IBLUE, bits 7,6,5,4 right justified
    SPI_sendData((IBLUE>>4)&0x0F);
    //IBLUE, bits 3,2,1,0 right justified
    SPI_sendData(IBLUE&0x0F);
  // Set Dot Matrix Peak Current Level (0x0Fh)
  SPI_sendCommand(LD7138_0x0F_PEAK_CURRENT_LEVEL);
    SPI_sendData(0x05); //Red
    SPI_sendData(0x10); //Green
    SPI_sendData(0x23); //Blue
  // Set Pre-Charge Width (0x1Ch)
  SPI_sendCommand(LD7138_0x1C_PRE_CHARGE_WIDTH);
    SPI_sendData(0x01);
  // Set Peak Pulse Width (0x1Dh)  
  SPI_sendCommand(LD7138_0x1C_SET_PEAK_WIDTH);
    SPI_sendData(0x3F); //Red
    SPI_sendData(0x10); //Green
    SPI_sendData(0x3C); //Blue
  // Set Peak Pulse Delay (0x1Eh)
  SPI_sendCommand(LD7138_0x1E_SET_PEAK_DELAY);
    SPI_sendData(0x0F);
  //Set Row Scan Operation (0x1Fh)
  SPI_sendCommand(LD7138_0x1F_SET_ROW_SCAN);
    SPI_sendData(0x30);
    // ---TT E-II
    //    || | ||-- Interlace modes
    //    || |----- 0 = normal, 1 = Ground rows
    //    ||------- Precharge/peak/max flags
  // Set Internal Regulator for Row Scan (0x30h)
  SPI_sendCommand(LD7138_0x30_VCC_R_SELECT);
    SPI_sendData(0x11);
    // ----E -MMM
    //     |  |||-- Multiplier:
    //     |        000 = VCC_C * 0.85
    //     |        001 = VCC_C * 0.80 *
    //     |        010 = VCC_C * 0.75
    //     |        011 = VCC_C * 0.70
    //     |        100 = VCC_C * 0.65
    //     |------- 1 = Enable, 0 = Disable
  //Set VDD Selection(0x3Ch)
  //Controls voltage used for Iref feedback
  SPI_sendCommand(LD7138_0x3C_SET_VDD_SELECTION);
    SPI_sendData(0x00); // 0 = 2.8v, 1 = 1.8v

  //Original spec uses linear gamma table:
  //  SPI_sendCommand(LD7138_0x3B_GAMMA_INIT);
  //Send gamma table
  SPI_sendCommand(LD7138_0x3A_GAMMA_TUNE);
  //Send red table
  uint8_t
      i;
  uint8_t
      g;
  //Send 32 bytes of red from the table
  for (i = 0; i<32; i++)
    {
    g=pgm_read_byte(&red_gamma[i]);
    g>>=1;
    SPI_sendData(g >> 4);   //high nibble
    SPI_sendData(0x0F & g); //low nibble
    }
  //Send 64 bytes of green from the table
  for (i = 0; i<64; i++)
    {
    g=pgm_read_byte(&green_gamma[i]);
    g>>=1;    
    SPI_sendData(g >> 4);   //high nibble
    SPI_sendData(0x0F & g); //low nibble
    }
  //Send 32 bytes of blue from the table
  for (i = 0; i<32; i++)
    {
    g=pgm_read_byte(&blue_gamma[i]);
    g>>=1;    
    SPI_sendData(g >> 4);   //high nibble
    SPI_sendData(0x0F & g); //low nibble
    }
  Fill_OLED(0x00,0x00,0x00);
    
  // Set Dot Matrix Display ON/OFF (0x02h)
  //   P0 = 0: indicates the dot matrix display turns OFF(Default).
  //   P0 = 1: indicates the dot matrix Display turns ON.
  // NOTE: Display OFF means:
  //   * All Column Output go to pre-charge level.
  //   * All Row Output go to the ground level
  //   * Stop Data transfer from memory to Dot Matrix Driver.  
  SPI_sendCommand(LD7138_0x02_DISPLAY_ON_OFF);
    SPI_sendData(0x01);   //0 = Off, 1 = On
  }
//============================================================================
#if(0) //simple, 30.38uS 8000
void Put_Pixel(uint16_t x, uint16_t y, uint8_t R, uint8_t G, uint8_t B)
  {
  Set_OLED_for_write_at_X_Y(x, y);
  //Write the single pixel's worth of data
  SPI_sendData((R&0xF8) | (G >> 5));
  SPI_sendData(((G << 3)&0xE0) | (B >> 3));
  }
#else //faster: 27.75uS, bigger (70 bytes)
void Put_Pixel(uint16_t x, uint16_t y, uint8_t R, uint8_t G, uint8_t B)
  {
  // Select the OLED controller
  CLR_CS;
  // Select the OLED's command register
  CLR_RS;
  //Send the command via SPI:
  SPI.transfer(LD7138_0x0A_SET_DATA_WINDOW);
    // Select the OLED's data register
    SET_RS;
    //Xstart, bits 6,5,4 right justified
    SPI.transfer((x>>4)&0x07);
    //Xstart, bits 3,2,1,0 right justified
    SPI.transfer(x&0x0F);
    #define XEND (127)
    //Xend, bits 6,5,4 right justified
    SPI.transfer((XEND>>4)&0x07);
    //Xend, bits 3,2,1,0 right justified
    SPI.transfer(XEND&0x0F);
    y+=60;
    //Ystart, bits 6,5,4 right justified
    SPI.transfer((y>>4)&0x07);
    //Ystart, bits 3,2,1,0 right justified
    SPI.transfer(y&0x0F);
    #define YEND (95)
    //Yend, bits 6,5,4 right justified
    SPI.transfer((YEND>>4)&0x07);
    //Yend, bits 3,2,1,0 right justified
    SPI.transfer(YEND&0x0F);
    #undef XEND
    #undef YEND

  // Select the OLED's command register
  CLR_RS;
  //Send the command via SPI:
  SPI.transfer(LD7138_0x0C_DATA_WRITE_READ);
    // Select the OLED's data register
    SET_RS;
    SPI.transfer((R&0xF8) | (G >> 5));
    SPI.transfer(((G << 3)&0xE0) | (B >> 3));

  // Deselect the OLED controller
  SET_CS;
  }
#endif
//============================================================================
// From: http://en.wikipedia.org/wiki/Midpoint_circle_algorithm
void OLED_Circle(uint16_t x0, uint16_t y0, uint16_t radius, uint16_t R, uint16_t G, uint16_t B)
  {
  uint16_t x = radius;
  uint16_t y = 0;
  int16_t radiusError = 1 - (int16_t) x;

  while (x >= y)
    {
    //11 O'Clock
    Put_Pixel(x0 - y, y0 + x, R, G, B);
    //1 O'Clock
    Put_Pixel(x0 + y, y0 + x, R, G, B);
    //10 O'Clock
    Put_Pixel(x0 - x, y0 + y, R, G, B);
    //2 O'Clock
    Put_Pixel(x0 + x, y0 + y, R, G, B);
    //8 O'Clock
    Put_Pixel(x0 - x, y0 - y, R, G, B);
    //4 O'Clock
    Put_Pixel(x0 + x, y0 - y, R, G, B);
    //7 O'Clock
    Put_Pixel(x0 - y, y0 - x, R, G, B);
    //5 O'Clock
    Put_Pixel(x0 + y, y0 - x, R, G, B);

    y++;
    if (radiusError < 0)
      radiusError += (int16_t)(2 * y + 1);
    else
      {
      x--;
      radiusError += 2 * (((int16_t) y - (int16_t) x) + 1);
      }
    }
  }
//============================================================================
#define mSwap(a,b,t)\
  {\
  t=a;\
  a=b;\
  b=t;\
  }\
//----------------------------------------------------------------------------
void Fast_Horizontal_Line(uint16_t x0, uint16_t y, uint16_t x1,
                          uint8_t R, uint8_t G, uint8_t B)
  {
  uint16_t
    temp;
  if(x1 < x0)
    mSwap(x0, x1, temp);
  Set_OLED_for_write_at_X_Y(x0, y);

  //Pre-calculate the two bytes for this color of pixel/
  uint8_t
    first_half;
  uint8_t
    second_half;
  //The display takes two bytes (565) RRRRR GGGGGG BBBBB 
  //to show one pixel.
  first_half=(R&0xF8) | (G >> 5);
  second_half=((G << 3)&0xE0) | (B >> 3);

  // Select the OLED controller
  CLR_CS;
  // Select the OLED's data register
  SET_RS;

  while(x0 <= x1)
    {
    //Write the single pixel's worth of data
    SPI.transfer(first_half);
    SPI.transfer(second_half);
    x0++;
    }
  // Deselect the OLED controller
  SET_CS;    
  }
//============================================================================
// From: http://rosettacode.org/wiki/Bitmap/Bresenham's_line_algorithm#C
void OLED_Line(uint16_t x0, uint16_t y0,
              uint16_t x1, uint16_t y1,
              uint8_t r, uint8_t g, uint8_t b)
  {
  int16_t
    dx;
  int16_t
    sx;
  int16_t
    dy;
  int16_t
    sy;
  int16_t
    err;
  int16_t
   e2;

  //General case
  if (y0 != y1)
    {
    dx = abs((int16_t )x1 - (int16_t )x0);
    sx = x0 < x1 ? 1 : -1;
    dy = abs((int16_t )y1 - (int16_t )y0);
    sy = y0 < y1 ? 1 : -1;
    err = (dx > dy ? dx : -dy) / 2;

    for (;;)
      {
      Put_Pixel(x0, y0, r,g,b);
      if ((x0 == x1) && (y0 == y1))
        break;
      e2 = err;
      if (e2 > -dx)
        {
        err -= dy;
        x0 = (uint16_t)((int16_t) x0 + sx);
        }
      if (e2 < dy)
        {
        err += dx;
        y0 = (uint16_t)((int16_t) y0 + sy);
        }
      }
    }
  else
    {
    //Optimized for OLED
    Fast_Horizontal_Line(x0, y0, x1,r,g,b);
    }
  }  
//============================================================================
//Gradient Bar fill
void Fill_OLED_Gamma_Gradient()
  {
  uint8_t
    x;
  for(x=0;x<=127;x++)
    {
    //Red
    OLED_Line(x,0,
              x,11,
              x<<1,0,0);
    //Green
    OLED_Line(x,12,
              x,23,
              0,x<<1,0);
    //Blue
    OLED_Line(x,24,
              x,35,
              0,0,x<<1);
    }
  }
//============================================================================
#if(1)
// This function transfers data, in one stream. Slightly
// optimized to do index operations during SPI transfers.
// 312uS ~ 319uS
void SPI_send_pixels(uint8_t pixel_count, uint8_t *data_ptr)
  {
  uint8_t
    r;
  uint8_t
    g;
  uint8_t
    b;
  uint8_t
    first_half;
  uint8_t
    second_half;

  // Select the OLED's data register
  SET_RS;
  // Select the OLED controller
  CLR_CS;

  //Load the first pixel. BMPs BGR format
  b=*data_ptr;
  data_ptr++;
  g=*data_ptr;
  data_ptr++;
  r=*data_ptr;
  data_ptr++;

  //The display takes two bytes (565) RRRRR GGGGGG BBBBB 
  //to show one pixel.
  first_half=(r&0xF8) | (g >> 5);
  second_half=((g << 3)&0xE0) | (b >> 3);

  while (pixel_count)
  {
    //Send the first half of this pixel out
    SPDR = first_half;
    //Load the next pixel while that is transmitting
    b = *data_ptr;
    data_ptr++;
    g = *data_ptr;
    data_ptr++;
    r = *data_ptr;
    data_ptr++;
    //Calculate the next first half while that is transmitting
    // ~1.9368us -0.1256 us = 1.8112uS
    first_half = (r & 0xF8) | (g >> 5);
    //Send the second half of the this pixel out
    SPDR = second_half;
    //Calculate the next second half
    second_half = ((g << 3) & 0xE0) | (b >> 3);
    //Done with this pixel
    pixel_count--;
    //small delay to allow the SPI register to finish
    delayMicroseconds(2);
  }
  //Wait for the final transfer to complete before we bang on CS.
  while (!(SPSR & _BV(SPIF))) ;
  // Deselect the OLED controller
  SET_CS;    
  }
#else
// Simple. Slower
// This function transfers data, in one stream.
// 660uS ~ 667uS
void SPI_send_pixels(uint8_t pixel_count, uint8_t *data_ptr)
  {
  uint8_t
    r;
  uint8_t
    g;
  uint8_t
    b;

  //  SPI_sendCommand(LD7138_0x0C_DATA_WRITE_READ);
  while(pixel_count)
    {
    //Load the first pixel. BMPs BGR format
    b=*data_ptr;
    data_ptr++;
    g=*data_ptr;
    data_ptr++;
    r=*data_ptr;
    data_ptr++;

    //The display takes two bytes (565) RRRRR GGGGGG BBBBB 
    //two show one pixel.
    //Calculate the first byte
    //RRRR RGGG
    SPI_sendData((r&0xF8) | (g >> 5));
    //Calculate the second byte
    //GGGB BBBB
    SPI_sendData(((g << 3)&0xE0) | (b >> 3));
    //Done with this pixel
    pixel_count--;
    }
  }
#endif  
//----------------------------------------------------------------------------
#if BUILD_SD
//Takes ~ 89.5mS per file
void show_BMPs_in_root(void)
  {
  File
    root_dir;
  root_dir = SD.open("/");
  if(0 == root_dir)
    {
    Serial.println("show_BMPs_in_root: Can't open \"root\"");
    return;
    }
  File
    bmp_file;

  while(1)
    {
    bmp_file = root_dir.openNextFile();
    if (0 == bmp_file)
      {
      // no more files, break out of while()
      // root_dir will be closed below.
      break;
      }
    //Skip directories (what about volume name?)
    if(0 == bmp_file.isDirectory())
      {
      //The file name must include ".BMP"
      if(0 != strstr(bmp_file.name(),".BMP"))
        {
        //The BMP must be exactly 13880 long
        //(this is correct for 128x36, 24-bit)
        if(13880 == bmp_file.size())
          {
          //Jump over BMP header. BMP must be 128x36 24-bit
          bmp_file.seek(54);
    
          //Since we are limited in memory, break the line up from
          // 128*3 = 384 bytes into two chunks of 63 pixels
          // each 64*3 = 192 bytes.
          //Making this static speeds it up slightly (10ms)
          //Reduces flash size by 114 bytes, and uses 192 bytes.
          static uint8_t
            half_of_a_line[64*3];
          for(uint8_t line=0;line<36;line++)
            {
            //Set the OLED to the left of this line
            Set_OLED_for_write_at_X_Y(0,35-line);
            for(uint8_t line_section=0;line_section<2;line_section++)
              {
              //Get half of the line
              bmp_file.read(half_of_a_line,64*3);
              //Now write this half to the TFT, doing the BGR -> 565
              //color fixup interlaced with the SPI transfers.
              SPI_send_pixels(64,half_of_a_line);
              }
            // Deselect the OLED controller
            SET_CS;    
            }
           }
         }
       }
    //Release the BMP file handle
    bmp_file.close();
    delay(1000);
    }
  //Release the root directory file handle
  root_dir.close();
  }
#endif
//============================================================================
void setup()
  {
  // OLED SPI & control lines
  //   ARD      | Port | OLED
  // -----------+------+-------------------------
  //  #6/D6     |  PD6 | OLED_RESET_NOT
  //  #7/D7     |  PD7 | VCC_EN
  //  #8/D8     |  PB0 | OLED_RS
  //  #9/D9     |  PB1 | OLED_CS_NOT
  // #10/D10    |  PB2 | SD_CS_NOT
  // #11/D11    |  PB3 | MOSI (hardware SPI)
  // #12/D12    |  PB4 | MISO (hardware SPI)
  // #13/D13    |  PB5 | SCK  (hardware SPI)

  //Set port directions
  DDRB |= 0x2F;
  DDRD |= 0xC0;

  //Drive the ports to a reasonable starting state.
  DISABLE_VCC;
  CLR_RESET;
  CLR_RS;
  SET_CS;
  CLR_MOSI;
  CLR_SCK;

  //debug console
  Serial.begin(9600);
  Serial.println("setup()");

  // Initialize SPI. By default the clock is 4MHz.
  SPI.begin();
  //Bump the clock to 8MHz. Appears to be the maximum.
  SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));

  //Initialize the OLED controller
  Initialize_OLED();
  delay(50);
  //Turn on the (~14v) OLED power supply
  ENABLE_VCC;
  delay(50);

#if BUILD_SD
  // For the Seeduino I am using, the default speed of SPI_HALF_SPEED
  // set in C:\Program Files (x86)\Arduino\libraries\SD\src\SD.cpp
  // results in a 4MHz clock.
  //
  // If you change this function call in SDClass::begin() of SD.cpp
  // from:
  //
  //  return card.init(SPI_HALF_SPEED, csPin) &&
  //         volume.init(card) &&
  //         root.openRoot(volume);
  //
  // to:
  //
  //  return card.init(SPI_FULL_SPEED, csPin) &&
  //         volume.init(card) &&
  //         root.openRoot(volume);
  //
  // That appears to make the SD library talk at 8MHz.
  //
  if (!SD.begin(SD_CS)) 
    {
    Serial.println("Card failed to initialize, or not present");
    //Reset the SPI clock to fast. SD card library does not clean up well.
    //Bump the clock to 8MHz. Appears to be the maximum.
    SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));    
    }
  else
    {
    Serial.println("Card initialized.");
    }
#endif
  }
//============================================================================
//Demo Controls
#define fillDemo	0
#define cheesyLines	0
#define circleDemo	0
#define checkerDemo	1
#define bitmapDemo	1
//----------------------------------------------------------------------------
void loop()
  {
  uint8_t
    i;
  uint16_t
    x;
  uint16_t
    sub_x;
  uint16_t
    y;
  uint16_t
    sub_y;
  uint8_t
    r;
  uint8_t
    g;
  uint8_t
    b;

#if(fillDemo)
  //Fill Demo
  for(i=0;i<2;i++)
    {
    Fill_OLED(0x00,0x00,0x00);
    delay(250);
    Fill_OLED(0x00,0x00,0xFF);
    delay(250);
    Fill_OLED(0x00,0xFF,0x00);
    delay(250);
    Fill_OLED(0x00,0xFF,0xFF);
    delay(250);
    Fill_OLED(0xFF,0x00,0x00);
    delay(250);
    Fill_OLED(0xFF,0x00,0xFF);
    delay(250);
    Fill_OLED(0xFF,0xFF,0x00);
    delay(250);
    Fill_OLED(0xFF,0xFF,0xFF);
    delay(250);
	Fill_OLED_Gamma_Gradient();
    delay(500);
    }
#endif

#if(cheesyLines)
  //Cheesy lines demo
  r=0xff;
  g=0x00;
  b=0x80;
  for(i=0;i<10;i++)
    {
    for(x=0;x<128;x++)
      {
      OLED_Line(63,17,
               x,0,
               r++,g--,b+=2);
      }
    for(y=0;y<36;y++)
      {
      OLED_Line(63,17,
               127,y,
               r++,g+=4,b+=2);
      }
    for(x=127;0!=x;x--)
      {
      OLED_Line(63,17,
               x,35,
               r-=3,g-=2,b-=1);
      }
    for(y=35;0!=y;y--)
      {
      OLED_Line(63,17,
               0,y,
               r+-3,g--,b++);
      }
	//delay(5000);
    }
#endif

#if(circleDemo)
  //Circle demo
  Fill_OLED(0x00,0x00,0x00);
  r=0xff;
  g=0x00;
  b=0x80;
  for(i=0;i<20;i++)
    {
    for(x=2;x<114;x+=2)
      {
      OLED_Circle(x+2, 17, 2+x%15,r+-6,g-=4,b+=11);
      }
    }
#endif

#if(checkerDemo)
  //8x8 checkerboard demo
  Fill_OLED(0x00,0x00,0x00);
  for(i=0;i<20;i++)
    {
    //Write a 8x8 checkerboard
    for(x=0;x<=15;x++)
      {
      for(y=0;y<=5;y++)
        {
        for(sub_x=0;sub_x<=7;sub_x++)
          {
          for(sub_y=0;sub_y<=7;sub_y++)
            {
            if(((x&0x01)&&!(y&0x01)) || (!(x&0x01)&&(y&0x01)))
              {
              Put_Pixel((x<<3)+sub_x,(y<<3)+sub_y, 0x00, 0x00, 0x00);
              }
            else
              {
              Put_Pixel((x<<3)+sub_x,(y<<3)+sub_y,0xFF-((x+i)<<4), 0xFF-((x<<5)+(i<<3)), 0xFF-(y<<6));
              }
            }
          }
        }
      }
    }
#endif

#if(bitmapDemo)
#if BUILD_SD
  //Bitmaps demo
  show_BMPs_in_root();
#endif
#endif
  }
//============================================================================

