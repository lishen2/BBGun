#ifndef _SPI_FLASH_H_
#define _SPI_FLASH_H_

//block erase aligned to 64kb bondary
#define FLASH_SPI_ERASE_BLOCK_SHIFT      16
#define FLASH_SPI_ERASE_BLOCK_SIZE       (0x01 << FLASH_SPI_ERASE_BLOCK_SHIFT)
#define FLASH_SPI_ERASE_BLOCK_ADDR_MASK  ((~0U) << FLASH_SPI_ERASE_BLOCK_SHIFT)

//sector erase aligned to 4kb bondary
#define FLASH_SPI_ERASE_SECTOR_SHIFT      12
#define FLASH_SPI_ERASE_SECTOR_SIZE       (0x01 << FLASH_SPI_ERASE_SECTOR_SHIFT)
#define FLASH_SPI_ERASE_SECTOR_ADDR_MASK  ((~0U) << FLASH_SPI_ERASE_SECTOR_SHIFT)

//写入地址256字节对齐
#define FLASH_SPI_WRITE_SHIFT      8
#define FLASH_SPI_WRITE_SIZE       (0x01 << FLASH_SPI_WRITE_SHIFT)
#define FLASH_SPI_WRITE_ADDR_MASK  ((~0U) << FLASH_SPI_WRITE_SHIFT)

void flash_spi_init(void);
int flash_spi_erase_block(uint32_t addr, uint32_t len);
int flash_spi_erase_sector(uint32_t addr, uint32_t len);
int flash_spi_read(uint32_t addr, uint32_t len, uint8_t *data);
int flash_spi_write(uint32_t addr, uint32_t len, uint8_t *data);

#endif

