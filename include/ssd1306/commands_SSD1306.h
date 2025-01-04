/**
 * @file commands_SSD1306.h
 * @author xiansnn (xiansnn@hotmail.com)
 * @brief This file contains all the defineition of SSD1306 commands
 * @version 0.1
 * @date 2024-08-05
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#if !defined(CONFIG_SSD1306_H)
#define CONFIG_SSD1306_H

// a definir si besoin
// #define SSD1306_WRITE_MODE _u(0xFE)
// #define SSD1306_READ_MODE _u(0xFF)


// ---------- physical screen size ----------
#define SSD1306_HEIGHT 64
#define SSD1306_WIDTH 128
#define SSD1306_PAGE_HEIGHT _u(8)
#define SSD1306_NUM_PAGES (SSD1306_HEIGHT / SSD1306_PAGE_HEIGHT)
#define SSD1306_BUF_LEN (SSD1306_NUM_PAGES * SSD1306_WIDTH)


// ---------- SSD1306 commands (see datasheet)----------
//  Continuation bit (Co), D/~C data/command bit
#define I2C_CMD_FLAG 0x80  // Co = 1, D/~C = 0 => the driver expects a command
#define I2C_DATA_FLAG 0x40 // Co = 0, D/~C = 1 => followinf bytes are data only


// ----- Fundamental Commands -----
#define SSD1306_SET_CONTRAST _u(0x81) //  Set Contrast Control, command with 1 parameter byte
// next byte A[7:0] to select 1 out of 256 contrast steps. Contrast increases as the value increases. (RESET = 7Fh )
#define SSD1306_SET_RAM_DISPLAY _u(0xA4)       // Resume to RAM content display (RESET value). Output follows RAM content
#define SSD1306_SET_DISPLAY_ON _u(0xA5)        // Entire display ON. Output ignores RAM content
#define SSD1306_SET_NORM_DISP _u(0xA6)         // Normal display (RESET). 0 in RAM: OFF in display panel, 1 in RAM: ON in display panel
#define SSD1306_SET_INV_DISP _u(0xA7)          // Inverse display (RESET). 1 in RAM: OFF in display panel, 0 in RAM: ON in display panel
#define SSD1306_SET_DISPLAY_SLEEP _u(0xAE)     // Display OFF (sleep mode) (RESET)
#define SSD1306_SET_DISPLAY_NORMAL_ON _u(0xAF) // Display ON in normal mode

// ----- Scrolling commands -----
#define SSD1306_SET_R_HORIZ_SCROLL _u(0x26) // Continuous Right Horizontal Scroll Setup, command with 6 parameter bytes
#define SSD1306_SET_L_HORIZ_SCROLL _u(0x27) // Continuous Left Horizontal Scroll Setup, command with 6 parameter bytes
// next byte A[7:0] = 0x00, Dummy byte (Set as 00h)
// next byte B[2:0] Define start page address
//     000b : PAGE0 | 001b : PAGE1 | 010b : PAGE2 | 011b : PAGE3 | 100b : PAGE4 | 101b : PAGE5 | 110b : PAGE6 | 111b : PAGE7
// next byte C[2:0] Set time interval between each scroll step in terms of frame frequency
//     000b : 5 frames | 001b : 64 frames | 010b : 128 frames | 011b : 256 frames | 100b : 3 frames | 101b : 4 frames | 110b : 25 frames | 111b : 2 frames
// next byte D[2:0] Define end page address.The value of D[2:0] must be larger or equal to B[2:0]
//     000b : PAGE0 | 001b : PAGE1 | 010b : PAGE2 | 011b : PAGE3 | 100b : PAGE4 | 101b : PAGE5 | 110b : PAGE6 | 111b : PAGE7
// next byte E[7:0] = 0x00, Dummy byte (Set as 00h)
// next byte F[7:0] = 0xFF, Dummy byte (Set as FFh)

#define SSD1306_SET_VERTICAL_R_HORIZ_SCROLL _u(0x29) // Continuous Vertical and Right Horizontal Scroll Setup, command with 5 parameter bytes
#define SSD1306_SET_VERTICAL_L_HORIZ_SCROLL _u(0x2A) // Continuous Vertical and Left Horizontal Scroll Setup, command with 5 parameter bytes
// next byte A[7:0] = 0x00, Dummy byte (Set as 00h)
// next byte B[2:0] Define start page address
//     000b : PAGE0 | 001b : PAGE1 | 010b : PAGE2 | 011b : PAGE3 | 100b : PAGE4 | 101b : PAGE5 | 110b : PAGE6 | 111b : PAGE7
// next byte C[2:0] Set time interval between each scroll step in terms of frame frequency
//     000b : 5 frames | 001b : 64 frames | 010b : 128 frames | 011b : 256 frames | 100b : 3 frames | 101b : 4 frames | 110b : 25 frames | 111b : 2 frames
// next byte D[2:0] Define end page address.The value of D[2:0] must be larger or equal to B[2:0]
//     000b : PAGE0 | 001b : PAGE1 | 010b : PAGE2 | 011b : PAGE3 | 100b : PAGE4 | 101b : PAGE5 | 110b : PAGE6 | 111b : PAGE7
// next byte E[5:0] = 0x01, Vertical scrolling offset = 1 row
//           E[5:0] = 0x3F, Vertical scrolling offset = 63 row
//           Note : No continuous vertical scrolling is available

#define SSD1306_SET_SCROLL_ON _u(0x2F) // Activate scroll. Start scrolling that is configured by the scrolling setup commands :26h/27h/29h/2Ah
//                                                                   with the following valid sequences:
//                                                                           Valid command sequence 1: 26h ;2Fh.
//                                                                           Valid command sequence 2: 27h ;2Fh.
//                                                                           Valid command sequence 3: 29h ;2Fh.
//                                                                           Valid command sequence 4: 2Ah ;2Fh.
// For example, if “26h; 2Ah; 2Fh.” commands are issued, the setting in the last scrolling setup command, i.e. 2Ah in this case, will be executed.
// In other words, setting in the last scrolling setup command overwrites the setting in the previous scrolling setup commands.

#define SSD1306_SET_SCROLL_OFF _u(0x2E) // Deactivate scroll. Stop scrolling that is configured by command 26h/27h/29h/2Ah.
// Note : After sending 2Eh command to deactivate the scrolling action, the ram data needs to be rewritten.

#define SSD1306_SET_VERTICAL_SCROLL_AREA _u(0xA3) //Set Vertical Scroll Area, command with 2 parameter bytes
// next byte A[5:0] Set No. of rows in top fixed area. The No. of rows in top fixed area is referenced to the top of the GDDRAM (i.e. row 0).(RESET = 0)
// next byte B[6:0] Set No. of rows in scroll area. This is the number of rows to be used for vertical scrolling. The scroll area starts in the first row below the top fixed area. (RESET = 64)
// Note 
// (1) A[5:0]+B[6:0] <= MUX ratio 
// (2) B[6:0] <= MUX ratio 
// (3a) Vertical scrolling offset (E[5:0] in 29h/2Ah) < B[6:0] 
// (3b) Set Display Start Line (X5X4X3X2X1X0 of 40h~7Fh) < B[6:0] 
// (4) The last row of the scroll area shifts to the first row of the scroll area. 
// (5) For 64d MUX display 
//         A[5:0] = 0, B[6:0]=64 : whole area scrolls 
//         A[5:0]= 0, B[6:0] < 64 : top area scrolls 
//         A[5:0] + B[6:0] < 64 : central area scrolls 
//         A[5:0] + B[6:0] = 64 : bottom area scroll

// ----- Addressing setting commands -----
#define SSD1306_SET_MEM_MODE _u(0x20) //Set Memory Addressing Mode, command with 1 parameter bytes
// next byte A[1:0] 
//                   A[1:0] = 00b, Horizontal Addressing Mode 
//                   A[1:0] = 01b, Vertical Addressing Mode 
//                   A[1:0] = 10b, Page Addressing Mode (RESET)
//                   A[1:0] = 11b, Invalid

// command : 0x00 - 0x0F  Set Lower  Column Start Address for Page Addressing Mode. This command is only for page addressing mode
// command : 0x10 - 0x1F  Set Higher Column Start Address for Page Addressing Mode. This command is only for page addressing mode
// command : 0xB0 - 0xB7  Set Page Start Address for Page Addressing Mode. This command is only for page addressing mode

// ----- Hardware Configuration (Panel resolution & layout related) Command -----
#define SSD1306_SET_DISP_START_LINE _u(0x40) // Set display RAM display start line register to 0.
// command : 0x40 - 0x7F  Set Display Start Line. Set display RAM display start line register from 0-63 using bit[5-0]. 
//                         Display start line register is reset to 000000b during RESET.

#define SSD1306_SET_COL_ADDR _u(0x21) // Set Column Address, command with 2 parameter bytes
// next byte A[6:0] : Column start address, range : 0-127d, (RESET = 0d)
// next byte B[6:0] : Column end address, range : 0-127d, (RESET = 127d)

#define SSD1306_SET_PAGE_ADDR _u(0x22) // Set Page Address, command with 2 parameter bytes. This command is only for horizontal or vertical addressing mode
// next byte A[2:0] : Page start Address, range : 0-7d, (RESET = 0d)
// next byte B[2:0] : Page end Address, range : 0-7d, (RESET = 7d)

#define SSD1306_SET_SEG_NO_REMAP _u(0xA0) // Set Segment Re-map. column address 0 is mapped to SEG0 (RESET)
#define SSD1306_SET_SEG_REMAP _u(0xA1) // Set Segment Re-map. column address 127 is mapped to SEG0

#define SSD1306_SET_MUX_RATIO _u(0xA8) // Set Multiplex Ratio, command with 1 parameter byte. Set MUX ratio to N+1 MUX 
// next byte A[5:0] : N=A[5:0] : from 16MUX to 64MUX, RESET= 111111b (i.e. 63d, 64MUX). A[5:0] from 0 to 14 are invalid entry. 

#define SSD1306_SET_COM_OUT_DIR _u(0xC0) // Set COM Output Scan Direction. normal mode (RESET) Scan from COM0 to COM[N –1] 
#define SSD1306_SET_COM_OUT_DIR_REMAP _u(0xC8) // Set COM Output Scan Direction. remapped mode. Scan from COM[N-1] to COM0 Where N is the Multiplex ratio. 

#define SSD1306_SET_DISP_OFFSET _u(0xD3) // Set Display Offset, command with 1 parameter byte.
// next byte A[5:0] : Set vertical shift by COM from 0d~63d. The value is reset to 00h after RESET. 

#define SSD1306_SET_COM_PIN_CFG _u(0xDA) // Set COM Pins Hardware Configuration, command with 1 parameter byte.
// next byte A[5:4] : A[4]=0b        0x02      Sequential COM pin configuration 
//                    A[4]=1b(RESET) 0x12      Alternative COM pin configuration 
//                    A[5]=0b(RESET) 0x02      Disable COM Left/Right remap 
//                    A[5]=1b        0x22      Enable COM Left/Right remap
// if ((SSD1306_WIDTH == 128) && (SSD1306_HEIGHT == 32))
//         0x02,
// elif ((SSD1306_WIDTH == 128) && (SSD1306_HEIGHT == 64))
//         0x12,
// else
//         0x02,
// endif



// ----- Timing & Driving Scheme Setting Command -----
#define SSD1306_SET_DISP_CLK_DIV _u(0xD5) // Set Display Clock Divide Ratio/Oscillator Frequency, command with 1 parameter byte.
// next byte A[7:0] : A[3:0] : Define the divide ratio (D) of the display clocks (DCLK): 
//                                 Divide ratio= A[3:0] + 1, RESET is 0000b (divide ratio = 1) 
//                    A[7:4] : Set the Oscillator Frequency, FOSC. 
//                                 Oscillator Frequency increases with the value of A[7:4] and vice versa. RESET is 1000b 
//                                 Range:0000b~1111b.  Frequency increases as setting value increases.  

#define SSD1306_SET_PRECHARGE _u(0xD9) //Set Pre-charge Period, command with 1 parameter byte.
// next byte A[7:0] : A[3:0] : Phase 1 period of up to 15 DCLK clocks 0 is invalid entry. (RESET=2h) 
//                    A[7:4] : Phase 2 period of up to 15 DCLK clocks 0 is invalid entry. (RESET=2h) 

#define SSD1306_SET_VCOM_DESEL _u(0xDB) // Set V(COMH) Deselect Level, command with 1 parameter byte.
// next byte A[6:4] : 0x00 - ~ 0.65 x Vcc | 0x20 - ~ 0.77 x Vcc (RESET) | 0x30 - ~ 0.83 x Vcc

#define SSD1306_NOP _u(0xE3) // Command for no operation

// ----- Charge Pump Command -----
#define SSD1306_SET_CHARGE_PUMP _u(0x8D) // Charge Pump Setting, command with 1 parameter byte. 
// next byte A[7:0] : A[2] = 0b, A[7:0]=0x00, Disable charge pump(RESET)           
//                    A[2] = 1b, A[7:0]=0x14, Enable charge pump during display on 
// Note :
//       The Charge Pump must be enabled by the following command: 
//       8Dh ; Charge Pump Setting 
//       14h ; Enable Charge Pump 
//       AFh ; SSD1306_SET_DISPLAY_NORMAL_ON
// ----------------------------------------------------------------------------------------------------

#endif // CONFIG_SSD1306_H

