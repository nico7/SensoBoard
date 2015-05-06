/*
 * LCD.c
 *
 * Created: 4/28/2015 2:13:01 PM
 *  Author: Nico-PC
 */ 
#include "LCD.h"



void turnLCDon(void)
{

	pio_clear(LCDPower);

}

void turnLCDoff(void)
{
	pio_set(LCDPower);
}

void drawPixel(uint32_t repeat, uint16_t pixNumber)
{
	 uint32_t i;
	for(i=0; i<repeat; i++)
	{
		pio_set(DnC);
		pio_clear(PIOC, 0x00FF);
		pio_set(PIOC, 0x00FF & (pixNumber>>8));
		pio_clear(Wclk);
		nop();
		pio_set(Wclk);
		nop();
			
		pio_clear(PIOC, 0x00FF);
		pio_set(PIOC, 0x00FF & pixNumber);
		pio_clear(Wclk);
		nop();
		
		pio_set(Wclk);
		nop();
		
	}
}

 void drawComPixel (uint32_t repeat, uint8_t pixNumber, uint8_t * PALETTE)	// Sends a pixel worth of data to the LCD. This function lets you put repeated pixels for extra convenience.
 {
	 uint32_t i;
	 for(i=0; i<repeat; i++) 
	 {
		pio_set(DnC);	//	 LATEbits.LATE12 = 1;            //LATE |= 0x1000;        // SET FOR DATA
		pio_clear(PIOC, 0x00FF);		// LATC &= 0xFF00;
		pixNumber &=0x0F;				 // because we only want 1-15... pixNumber is a copy of the real data, and drawPixel takes 1 nibble of data at a time here.
		//----------------MSB---------------------
		pio_set(PIOC, PALETTE[pixNumber-1]>>8);	//LATC |= PALETTE[pixNumber-1] >> 8;
		pio_clear(Wclk);							//LATEbits.LATE13 =0;             //LATE &= ~0x2000;
		nop();
		pio_set(Wclk);								//LATEbits.LATE13 = 1;            //LATE |= 0x2000;
		//----------------LSB---------------------
		pio_clear(PIOC, 0x00FF);		 //LATC &= 0xFF00;
		pio_set(PIOC, PALETTE[pixNumber-1]&0x0FF);			 //LATC |= PALETTE[pixNumber-1] & 0x00FF;
		pio_clear(Wclk);					//LATEbits.LATE13 =0;             //LATE &= ~0x2000;
		nop();
		pio_set(Wclk);				//LATEbits.LATE13 =1;             //LATE |= 0x2000;*/
		nop();
	 }
 }
 void cmd_nDat8(uint8_t cnd, uint16_t a)     // Sends commands or data to LCD
 {                                 					 // 'int a' should be in Hex 0x00## where ## is the desired number
	 if(cnd == 1)                   			 // if cnd is 1, writing command, otherwise, write data.
	 {
		pio_clear(DnC);	//.LATE12 =0;     	// This is necessary to signal the LCD whether the data is a command or not.
	 }
	 else
	 {
		pio_set(DnC);	//LATEbits.LATE12 =1;         //LATE |= 0x1000;            	// Set bit 12
	 }
	 
	 pio_clear(PIOC, 0x00FF);	//LATC &= 0xFF00;					// Clears the PortC pins that are connected to the LCD to then be able to write to them
	 pio_set(PIOC, 0x00FF & a);								// Writes to the PortC pins that are connected to the LCD. (this is how data is sent to the LCD)
	  
	 // Turning off and then on this pin signals the LCD that it can take the data on PortC as data/command
	 pio_clear(Wclk);		//LATEbits.LATE13 =0;              //LATE &= ~0x2000; //0xDFFF;
	 	//nop();
	 	delay_us(1);					//Nop();										//asm("NOP");      // Do nothing for an instruction cycle.
	 pio_set(Wclk);			//LATEbits.LATE13 =1;              //LATE |= 0x2000;
	 delay_us(1);
 }
 
 
 /**
 *\brief Initialize hardware for LCD
 */

 void lcd_init(void)
 {
	 
	pmc_enable_periph_clk(ID_PIOC);
	
	pio_set_output(LCDPower, HIGH, DISABLE, DISABLE);
	pio_set_output(LCDReset, LOW, DISABLE, DISABLE);
	pio_set_output(DnC, HIGH, DISABLE, DISABLE);
	pio_set_output(Wclk, HIGH, DISABLE, DISABLE);
	pio_set_output(Rclk, HIGH, DISABLE, DISABLE);
	pio_set_output(PIOC, 0x00ff, LOW, DISABLE, DISABLE);

	delay_us(2);
 }
 
 
 /**
 *\brief Set up registers in LCD to get it ready to display images.
 */
void setup_lcd(void)
{
	pio_set(LCDReset);
	delay_ms(6); 
	 
	cmd_nDat8(1, 0x0011);         // Command
	delay_ms(6);              // Delay 6ms
	cmd_nDat8(1, 0x0028);          // Command

	// -----------   Power CTL1   -----------------
	cmd_nDat8(1, 0x00C0);          // Command
	cmd_nDat8(0, 0x0026);          // Data
	cmd_nDat8(0, 0x0004);         // Data

	// -----------   Power CTL2   -----------------
	cmd_nDat8(1, 0x00C1);
	cmd_nDat8(0, 0x0004);

	// -----------   VCOM CTL1   -----------------
	cmd_nDat8(1, 0x00C5);
	cmd_nDat8(0, 0x0034);
	cmd_nDat8(0, 0x0040);

	// -----------   FRAME RATE   -----------------
	cmd_nDat8(1, 0x00B1);
	cmd_nDat8(0, 0x0000);
	cmd_nDat8(0, 0x0018);

	// -----------   DISPLAY FUNCTION   -----------------
	cmd_nDat8(1, 0x00B6);
	cmd_nDat8(0, 0x000A);
	cmd_nDat8(0, 0x00A2);
	cmd_nDat8(0, 0x0027);
	cmd_nDat8(0, 0x0000);

	// -----------   MEM ACCESS = BGR  (as in RGB) -----------------
	cmd_nDat8(1, 0x0036);
	cmd_nDat8(0, 0x0008);
	
	// ----------- VCOM CTL2  -----------------
	cmd_nDat8(1, 0x00C7);
	cmd_nDat8(0, 0x00C0);

	// ----------- PIXEL = 16 BIT  -----------------
	cmd_nDat8(1, 0x003A);
	cmd_nDat8(0, 0x0055);

	// ----------- GAMMA POS  -----------------
	cmd_nDat8(1, 0x00E0);
	cmd_nDat8(0, 0x001F);    // Data1
	cmd_nDat8(0, 0x001B);    // Data2
	cmd_nDat8(0, 0x0018);    // Data3
	cmd_nDat8(0, 0x000B);    // Data4
	cmd_nDat8(0, 0x000F);    // Data5

	cmd_nDat8(0, 0x0009);    // Data6
	cmd_nDat8(0, 0x0046);    // Data7
	cmd_nDat8(0, 0x00B5);    // Data8
	cmd_nDat8(0, 0x0037);    // Data9
	cmd_nDat8(0, 0x000A);    // Data10
	
	cmd_nDat8(0, 0x000C);    // Data11
	cmd_nDat8(0, 0x0007);    // Data12
	cmd_nDat8(0, 0x0007);    // Data13
	cmd_nDat8(0, 0x0005);    // Data14
	cmd_nDat8(0, 0x0000);    // Data15

	// ----------- GAMMA NEG  -----------------
	cmd_nDat8(1, 0x00E1);
	cmd_nDat8(0, 0x0000);    // Data1
	cmd_nDat8(0, 0x0024);    // Data2
	cmd_nDat8(0, 0x0027);    // Data3
	cmd_nDat8(0, 0x0004);    // Data4
	cmd_nDat8(0, 0x0010);    // Data5
	cmd_nDat8(0, 0x0006);    // Data6
	cmd_nDat8(0, 0x0039);    // Data7
	cmd_nDat8(0, 0x0074);    // Data8
	cmd_nDat8(0, 0x0048);    // Data9
	cmd_nDat8(0, 0x0005);    // Data10
	cmd_nDat8(0, 0x0013);    // Data11
	cmd_nDat8(0, 0x0038);    // Data12
	cmd_nDat8(0, 0x0038);    // Data13
	cmd_nDat8(0, 0x003A);    // Data14
	cmd_nDat8(0, 0x001F);    // Data15

	// ----------- COLUMN ADDRESS  -----------------
	cmd_nDat8(1, 0x002A);
	cmd_nDat8(0, 0x0000);    // Data1
	cmd_nDat8(0, 0x0000);    // Data2
	cmd_nDat8(0, 0x0000);    // Data3
	cmd_nDat8(0, 0x00EF);    // Data4

	// ----------- ROW ADDRESS  -----------------
	cmd_nDat8(1, 0x002B);
	cmd_nDat8(0, 0x0000);    // Data1
	cmd_nDat8(0, 0x0000);    // Data2
	cmd_nDat8(0, 0x0001);    // Data3
	cmd_nDat8(0, 0x003F);    // Data4       /// simultaneous sampling //DS configuration for 1.1MSPS  // a2d pdf....CPU info

	// ----------- DISPLAY ON -----------------
	cmd_nDat8(1, 0x0029);
   // --------------------------------------------------------------------------
   //################### DELAY 24mS ############################
   //  MINUMUM DELAY FOR DISPLAY CORRECTLY IS 20mS ########
   delay_ms(24);
      // ----------- START MAIN PROGRAM -----------------
      /* ----------- DISPLAY -----------------
    ###########################################################
             BITS            15                            0
     COLOR IS 16 BITS -----> R R R R R G G G G G G B B B B B

    ############## WRITE G-RAM MEMORY ##########################*/

		cmd_nDat8(1, 0x002C);
		delay_ms(1);
	
 }
 
