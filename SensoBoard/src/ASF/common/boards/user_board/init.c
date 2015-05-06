/**
 * \file
 *
 * \brief User board initialization template
 *
 */
 /**
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include <asf.h>
#include <board.h>
#include <conf_board.h>

/*
 * \brief Set peripheral mode for IOPORT pins.
 * It will configure port mode and disable pin mode (but enable peripheral).
 * \param port IOPORT port to configure
 * \param masks IOPORT pin masks to configure
 * \param mode Mode masks to configure for the specified pin (\ref ioport_modes)
 */
#define ioport_set_port_peripheral_mode(port, masks, mode) \
	do {\
		ioport_set_port_mode(port, masks, mode);\
		ioport_disable_port(port, masks);\
	} while (0)

/**
 * \brief Set peripheral mode for one single IOPORT pin.
 * It will configure port mode and disable pin mode (but enable peripheral).
 * \param pin IOPORT pin to configure
 * \param mode Mode masks to configure for the specified pin (\ref ioport_modes)
 */
#define ioport_set_pin_peripheral_mode(pin, mode) 	do {ioport_set_pin_mode(pin, mode);	ioport_disable_pin(pin);} while (0)
void board_init(void)
{
	/* This function is meant to contain board-specific initialization code
	 * for, e.g., the I/O pins. The initialization can rely on application-
	 * specific board configuration, found in conf_board.h.
	*/

	
	pmc_enable_periph_clk(ID_SPI);
	
	ioport_init();
	
	
	ioport_set_pin_dir(SD_MMC_0_CD_GPIO, IOPORT_DIR_INPUT);
	ioport_set_pin_mode(SD_MMC_0_CD_GPIO, IOPORT_MODE_PULLUP);
	ioport_set_pin_peripheral_mode(SPI_MISO_GPIO, SPI_MISO_FLAGS);
	ioport_set_pin_peripheral_mode(SPI_MOSI_GPIO, SPI_MOSI_FLAGS);
	ioport_set_pin_peripheral_mode(SPI_SPCK_GPIO, SPI_SPCK_FLAGS);
	ioport_set_pin_peripheral_mode(SPI_NPCS0_GPIO, SPI_NPCS0_FLAGS);
	
	ioport_set_pin_peripheral_mode(PIO_PB0_IDX, IOPORT_MODE_MUX_B);
	ioport_set_pin_peripheral_mode(PIO_PB1_IDX, IOPORT_MODE_MUX_B);


}
