// MUST define ACCESS_MEM_TO_RAM_ENABLED in board.h
#include <asf.h>
#include <string.h>
#include <math.h>

#include "LCD.h"

#define MMA_ADDR	0x1C
#define ESPTX		PIOB, PIO_PB10
#define ESPRX		PIOB, PIO_PB11
#define ESPRST		PIOC, PIO_PC22
#define BLED		PIOA, PIO_PA0
#define GLED		PIOA, PIO_PA23
#define OLED		PIOC, PIO_PC23
#define EN_COLOR	PIOA, PIO_PA27

//-------------- UART0 STUFF-------------------------//
#define	UART0PINS		(PIO_PA10A_UTXD0|PIO_PA9A_URXD0)
#define PINS_UART0_FLAGS    (PIO_PERIPH_A | PIO_DEFAULT)
#define PINS_UART0_MASK     UART0PINS
#define PINS_UART0_PIO      PIOA

#define PINS_UART0_TYPE     PIO_PERIPH_A
#define PINS_UART0_ATTR     PIO_DEFAULT

#define UART_SERIAL_BAUDRATE	9600
#define UART_SERIAL_MODE		UART_MR_PAR_NO

void uart_custom_init(void);
void uartSend(Uart *p_uart, char * data);
twi_options_t twinit2(twi_options_t opt);
twi_packet_t configTwiPacket(twi_packet_t packet, uint8_t address, uint8_t registro, uint8_t * buffer, uint8_t bufferSize);


uint8_t PALETTE[16];
uint16_t FLAG = 0;
uint8_t twi2Tx[] = {0x00};
uint8_t twi2Rx[10];



	
int main (void)
{
	uint32_t i = 10;
	uint8_t sdPresent=0;
	uint32_t x=1;
	char work[]="something\n\r";
	char filename[] = {"0:hello.txt"};
	sd_mmc_err_t  sdResponse = 55;
	BYTE * number;
	FATFS fs;
	FRESULT res;
	FIL fileObject;
	TCHAR * name[20];
	
	twi_options_t opt;
	twi_packet_t packet_tx, packet_rx;
	// Insert system clock initialization code here (sysclk_init()).
	sysclk_init();
	board_init();
	wdt_disable(WDT);
	
	pmc_enable_periph_clk(ID_PIOA);
	pmc_enable_periph_clk(ID_PIOB);
	pmc_enable_periph_clk(ID_PIOC);
	pmc_enable_periph_clk(ID_UART3);	// Enabling Uart3 clock to communicate with ESP8266										
	pmc_enable_periph_clk(ID_TWI2);	// removed pmc_enable_periph_clk(SPI) which should be wrong anyways.
	
	uart_custom_init();
	sd_mmc_init();

	pio_set_output(ESPTX, HIGH, DISABLE, DISABLE);
	pio_set_output(ESPRST, HIGH, DISABLE, DISABLE);
	pio_set_input(ESPRX, PIO_DEFAULT);
	pio_set_output(BLED, LOW, DISABLE, DISABLE);
	pio_set_output(OLED, LOW, DISABLE, DISABLE);
	pio_set_output(GLED, LOW, DISABLE, DISABLE);
	pio_set_output(EN_COLOR, HIGH, DISABLE, DISABLE);
	

	/* Configure the options of TWI driver */
	opt.master_clk = sysclk_get_cpu_hz();
	opt.speed      = 50000;

	/* Configure the data packet to be transmitted */
	packet_tx.chip        = MMA_ADDR;
	packet_tx.addr[0]     = 0x00;

	packet_tx.addr_length = 1;
	packet_tx.buffer      = (uint8_t *) twi2Tx;
	packet_tx.length      = sizeof(twi2Tx);

	/* Configure the data packet to be received */
	packet_rx.chip        = packet_tx.chip;
	packet_rx.addr[0]     = 0x11;
	packet_rx.addr_length = packet_tx.addr_length;
	packet_rx.buffer      = twi2Rx;
	packet_rx.length      = packet_tx.length;
	
	if (twi_master_init(TWI2, &opt) != TWI_SUCCESS) 
	{
		pio_set(GLED);
	}
	delay_ms(6);
/*	

	while(twi_master_read(TWI2, &packet_rx) != TWI_SUCCESS) 
	{

		pio_set(OLED);
		i= twi_master_read(TWI2, &packet_rx);
		pio_clear(OLED);
	}
	
	memset(&fs, 0, sizeof(FATFS));
	uartSend(UART0, work);
	*/
	

	lcd_init();
	turnLCDon(); 
	setup_lcd();
while(1)
{
	x += i++;
	
	drawPixel((i%x)%4+1, (0xffff)*sin(x)*sin(x));
	
}
	x=1;
	delay_s(2);
	//turnLCDoff();
	i=10;
	while(sd_mmc_check(0)!= SD_MMC_OK)
	{
		sdResponse = sd_mmc_check(0);
		delay_ms(10);
		i--;
		if (i==0)
		{
			break;
		}
	}
	
	if(sdResponse == SD_MMC_OK)
	{
	//	uart_write(UART0, 79);
		sdPresent = 1;
	}
	
	while(1)
	{	
		if(sdPresent)
		{
			sdPresent = 0;

				do{
					res = disk_initialize(LUN_ID_SD_MMC_0_MEM);
	
					delay_ms(10);
				}while (res != FR_OK );
				
				res = f_mount(LUN_ID_SD_MMC_0_MEM, &fs);	// Using FatFs API, gotta mount the SD Card/ drive
				filename[0] = LUN_ID_SD_MMC_0_MEM + '0';
			//	res = f_opendir(&dir, "0:NICO");
				res = f_open(&fileObject,(char const *)filename,FA_CREATE_ALWAYS | FA_WRITE);	
				while(res != FR_OK)
				{
				
					res = f_open(&fileObject,(char *)filename,FA_CREATE_ALWAYS | FA_WRITE);
					delay_ms(1);
					
				}
				
				if (res == FR_OK)
				{
					
					f_write(&fileObject, "Hello Nico!\n", 13, number);
					
					f_close(&fileObject);
					
				}
				
				
			
		}
		
		delay_ms(x*10);
		pio_set(OLED);
		pio_clear(GLED);
		delay_ms(x*10);
		pio_set(BLED);
		pio_clear(OLED);
		delay_ms(x*10);
		pio_set(GLED);
		pio_clear(BLED);
		x++;
		
		if(x>60){
			x=1;
		
		}
	//		uart_write(UART0, 60);
		
	}

	// Insert application code here, after the board has been initialized.
}

void uart_custom_init(void)
{
	
	// set the pins to use the uart peripheral
	pio_configure(PINS_UART0_PIO, PINS_UART0_TYPE, PINS_UART0_MASK, PINS_UART0_ATTR);
	pmc_enable_periph_clk(ID_UART0);


	const sam_uart_opt_t uart0_settings ={ sysclk_get_cpu_hz(), UART_SERIAL_BAUDRATE, UART_SERIAL_MODE };

	uart_init(UART0,&uart0_settings);      //Init UART1 and enable Rx and Tx
/*
	uart_enable_interrupt(UART0,UART_IER_RXRDY);   //Interrupt reading ready
	NVIC_EnableIRQ(UART1_IRQn);
*/
}

void uartSend(Uart *p_uart, char * data)
{
	uint8_t i=0;
	while(data[i] != '\0')
	{
		while (!(UART0->UART_SR & UART_SR_TXRDY));
			uart_write(UART0, data[i++]);
	}
}

twi_options_t twinit2(twi_options_t opt)
{
		/* Configure the options of TWI driver */
	opt.master_clk = sysclk_get_cpu_hz();
	opt.speed      = 100000;

	if (twi_master_init(TWI2, &opt) != TWI_SUCCESS)
	{
		return opt;
	}
	
	return opt;
}

twi_packet_t configTwiPacket(twi_packet_t packet, uint8_t address, uint8_t registro, uint8_t * buffer, uint8_t bufferSize)
{
		/* Configure the data packet to be transmitted */
		packet.chip        = address;
		packet.addr[0]     = registro;
		packet.addr_length = 1;
		packet.buffer      = (uint8_t *) buffer;
		packet.length      = bufferSize;
		
		return packet;
}