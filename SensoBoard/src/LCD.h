/*
 * LCD.h
 *
 * Created: 4/28/2015 2:13:22 PM
 *  Author: Nico-PC
 */ 


#ifndef LCD_H_
#define LCD_H_
#include <pio.h>
#include <delay.h>
#include <ioport.h>

#define LCDPower	PIOA, PIO_PA20
#define LCDReset	PIOA, PIO_PA15
#define DnC			PIOC, PIO_PC12
#define Wclk		PIOA, PIO_PA16
#define Rclk		PIOC, PIO_PC27

void turnLCDon(void);
void turnLCDoff(void);
void drawPixel(uint32_t repeat, uint16_t pixNumber);
void drawComPixel (uint32_t repeat, uint8_t pixNumber, uint8_t * PALETTE);
void cmd_nDat8(uint8_t cnd, uint16_t a);
void lcd_init(void);
void setup_lcd(void);

#endif /* LCD_H_ */