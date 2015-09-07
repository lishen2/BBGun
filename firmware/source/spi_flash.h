#ifndef _SPI_FLASH_H_
#define _SPI_FLASH_H_

void flash_spi_init(void);
int flash_spi_erase(uint32_t addr, uint32_t len);
int flash_spi_read(uint32_t addr, uint32_t len, uint8_t *data);
int flash_spi_write(uint32_t addr, uint32_t len, uint8_t *data);

#endif
