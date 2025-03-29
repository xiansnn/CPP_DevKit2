/**
 * @file commands_ST7735.h
 * @author xiansnn (xiansnn@hotmail.com)
 * @brief These commands are those for either the ST7735 TFT 1.44" 128x128 pixels or the ST7735 TFT 1.77" 128x160 pixels
 * interface : 4-wire serial SPI
 * Color coding : Write Data for 16-bit/Pixel (RGB 5-6-5-bit Input), 65K-Colors, COLMOD(IFPF=5) [Cx/W]0x3A[C/W]0x05
 * _Blue PCB_ : 
 *    TFT 1.77" 128x160 pixels
 *    pinout:
 *       1 GND
 *       2 VCC  (  5V max)
 *       3 SCK  (3.3V max) SPI clock
 *       4 SDA  (3.3V max) SPI data
 *       5 RESX (3.3V max) RESet  to be connected to GPIO pin, Low enable.
 *       6 D/CX  (3.3V max) Register Select or D/C_ data/command selection pin to be connected to GPIO pin
 *       7 CSX  (3.3V max) Chip Select to be connected to GPIO pin, Low enable.
 *       8 LEDA (3.3V )    can be connected to GPIO pin
 * _Red PCB_ :
 *    TFT 1.44" 128x128 pixels mounted on Pico Primer kit  v1.0 from www.makerfabs.com
 *    pinout:
 *       1  GND   connected to GND
 *       2  VSS   connected to GND
 *       3  LEDK  connected to LEDK
 *       4  LEDA  connected to 3.3V
 *       5  VSS_2 connected to GND
 *       6  RES   connected to GPIO15
 *       7  A0    D/C_ data/command connected to GPIO14
 *       8  SDA   connected to SPI1-TX  / GPIO11
 *       9  SCL   connected to SPI1-SCK / GPIO10
 *       10 VDD   connected to 3.3V
 *       11 VDDIO connected to 3.3V
 *       12 CS_   connected to SPI1-CS  / GPIO13
 *       13 VSS_3 connected to GND 
 *       14 GND_2 connected to GND
 * signal D/Cx R/W
 * [Cx/W] Command/Write
 * [D/W]  Data/Write
 * [D/R]  Data/Read
 * Some commands are desactivated because for the used device, the corresponding input/output is not wired
 *     
 * 
 * @version 0.1
 * @date 2025-03-28
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#pragma once

// ---------- physical screen size ----------
/// @brief the height in pixel of ST7735
#define ST7735_HEIGHT 128
/// @brief the width in pixel of ST7735
#define ST7735_WIDTH 128 // can be 160

// ---------- system function commands ----------


#define ST7735_NOP        0x00u // [Cx/W] No Operation 
#define ST7735_SWRESET    0x01u // [Cx/W] Software Reset

// #define ST7735_RDDID      0x04u // [Cx/W] Read Display ID (followed by 4[D/R] )
// #define ST7735_RDDST      0x09u // [Cx/W] Read Display Status (followed by 5[D/R] )
// #define ST7735_RDDPM      0x0Au // [Cx/W] Read Display Power Mode (followed by 2[D/R] )
// #define ST7735_RDDMADCTL  0x0Bu // [Cx/W] Read Display MADCTL (followed by 2[D/R] )
// #define ST7735_RDDCOLMOD  0x0Cu // [Cx/W] Read Display Pixel Format (followed by 2[D/R] )
// #define ST7735_RDDIM      0x0Du // [Cx/W] Read Display Image Mode (followed by 2[D/R] )
// #define ST7735_RDDSM      0x0Eu // [Cx/W] Read Display Signal Mode (followed by 2[D/R] )
// #define ST7735_RDDSDR     0x0Fu // [Cx/W] Read Display Self-Diagnostic Result (followed by 2[D/R] )

#define ST7735_SLPIN      0x10u // [Cx/W] Sleep In & Booster Off 
#define ST7735_SLPOUT     0x11u // [Cx/W] Sleep Out & Booster On . Wait for 120ms before sending next command.

#define ST7735_PTLON      0x12u // [Cx/W] Partial Mode On 
#define ST7735_NORON      0x13u // [Cx/W] Partial Off (Normal)
#define ST7735_INVOFF     0x20u // [Cx/W] Display Inversion Off (Normal)
#define ST7735_INVON      0x21u // [Cx/W] Display Inversion On

#define ST7735_GAMSET     0x26u // [Cx/W] Gamma Curve Select (followed by 1[D/W] )

#define ST7735_DISPOFF    0x28u // [Cx/W] Display Off
#define ST7735_DISPON     0x29u // [Cx/W] Display On

// write date in RAM
#define ST7735_CASET      0x2Au // [Cx/W] Column Address Set (followed by 4[D/W] )
#define ST7735_RASET      0x2Bu // [Cx/W] Row Address Set (followed by 4[D/W] )
#define ST7735_RAMWR      0x2Cu // [Cx/W] Memory Write (followed by n[D/W] )
// #define ST7735_RAMRD      0x2Eu // [Cx/W] Memory Read (followed by 2[D/R] )

#define ST7735_RGBSET     0x2Du // [Cx/W] LUT for 4k,65k,262k Color display (followed by 128[D/W] )


#define ST7735_PTLAR      0x30u // [Cx/W] Partial Start/End Address Set  (followed by 4[D/W] )
// #define ST7735_TEOFF      0x34u // [Cx/W] Tearing effect line off 
// #define ST7735_TEON       0x35u // [Cx/W] Tearing Effect Mode Set & on  (followed by 1[D/W] )

#define ST7735_MADCTL     0x36u // [Cx/W] Memory Data Access Control  (followed by 1[D/W] )

// scrolling definition
#define ST7735_SCRLAR     0x33u // [Cx/W] Scroll area set   (followed by 6[D/W] )
#define ST7735_VSCSAD     0x37u // [Cx/W] Scroll Start Address of RAM  (followed by 2[D/W] )
// IDLE
#define ST7735_IDMOFF     0x38u // [Cx/W] Idle Mode Off
#define ST7735_IDMON      0x39u // [Cx/W] Idle Mode On

#define ST7735_COLMOD     0x3Au // [Cx/W] Interface Pixel Format (followed by 1[D/W]). default value is 18b/pixel->To be changed by 16b/pixel

// #define ST7735_RDID1      0xDAu // [Cx/W] ) Read ID1 (followed by 1[D/R] )
// #define ST7735_RDID2      0xDBu // [Cx/W] ) Read ID2 (followed by 1[D/R] )
// #define ST7735_RDID3      0xDCu // [Cx/W] ) Read ID3 (followed by 1[D/R] )

// ---------- panel function commands ----------
// ---------- Frame Rate Control
#define ST7735_FRMCTR1    0xB1u // [Cx/W] ) In normal mode (Full Colors) (followed by 3[D/W] )
#define ST7735_FRMCTR2    0xB2u // [Cx/W] ) In normal mode (8-colors) (followed by 3[D/W] )
#define ST7735_FRMCTR3    0xB3u // [Cx/W] ) In Partial mode + Full Colors (followed by 6[D/W] )

// // ---------- Display Inversion Control
#define ST7735_INVCTR     0xB4u // [Cx/W] ) Display Inversion Control (followed by 1[D/W] )
#define ST7735_DISSET5    0xB6u // [Cx/W] ) Display Function set 5 (followed by 2[D/W] ) NOTICE: for ST7735 only, not available on ST7735S

// // ---------- Power Control
#define ST7735_PWCTR1     0xC0u // [Cx/W] ) Power Control Setting  (followed by 3[D/W] )
#define ST7735_PWCTR2     0xC1u // [Cx/W] ) Power Control Setting  (followed by 1[D/W] )
#define ST7735_PWCTR3     0xC2u // [Cx/W] ) Power Control Setting  (followed by 2[D/W] )
#define ST7735_PWCTR4     0xC3u // [Cx/W] ) Power Control Setting in Idle Mode (8-colors)  (followed by 2[D/W] )
#define ST7735_PWCTR5     0xC4u // [Cx/W] ) Power Control Setting in Partial Mode + Full COlors (followed by 2[D/W] )

// // ---------- VCOM
#define ST7735_VMCTR1     0xC5u // [Cx/W] ) VCOM Control 1 (followed by 1[D/W] )
#define ST7735_VMOFTR     0xC7u // [Cx/W] ) Set VCOM Offset Control (followed by 1[D/W] )

// // ---------- ID
#define ST7735_WRID2      0xD1u // [Cx/W] ) Set LCM Version Code (followed by 1[D/W] )
#define ST7735_WRID3      0xD2u // [Cx/W] ) Customer Project COde (followed by 1[D/W] )

// // ---------- NVM
#define ST7735_NVCTR1     0xD9u // [Cx/W] ) NVM Control Status (followed by 1[D/W] )
// #define ST7735_NVCTR2     0xDEu // [Cx/W] ) NVM Read Command (followed by [D/W]0xF5,[D/W]0xA5 )
#define ST7735_NVCTR3     0xDFu // [Cx/W] ) NVM Write Command (followed by [D/W]<command code>,[D/W]0xA5 )

// // ---------- Gamma
#define ST7735_GAMCTRP1   0xE0u // [Cx/W] ) Set Gamma adjustment (+polarity) (followed by 16[D/W] )
#define ST7735_GAMCTRN1   0xE1u // [Cx/W] ) Set Gamma adjustment (-polarity) (followed by 16[D/W] )

// // ---------- Gate pump
#define ST7735_GCV        0xFCu // [Cx/W] ) Gate Pump Clock Frequency Variable (followed by 1[D/W] )