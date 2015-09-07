#ifndef _PLATFORM_H_
#define _PLATFORM_H_

/////////////////////////////////////////////////////////////
//internal flash
/////////////////////////////////////////////////////////////
#define PLATFORM_INTERNAL_FLASH_START 0x20000000
#define PLATFORM_INTERNAL_FLASH_SIZE  0x10000

//program size 60kb
#define PLATFORM_INTERNAL_PROGRAM_START PLATFORM_INTERNAL_FLASH_START
#define PLATFORM_INTERNAL_PROGRAM_SIZE  0xE000

/////////////////////////////////////////////////////////////
//SPI flash
/////////////////////////////////////////////////////////////
#define PLATFORM_SPI_FLASH_START  0x00000000
#define PLATFORM_SPI_FLASH_SIZE   0x00400000   //4Mbytes

//sound area 256kb
#define PLATFORM_SPI_SOUND_START  PLATFORM_SPI_FLASH_START
#define PLATFORM_SPI_SOUND_SIZE   0x00040000

//config area 16KB
#define PLATFORM_SPI_CONFIG_START (PLATFORM_SPI_SOUND_START + \
                                   PLATFORM_SPI_SOUND_SIZE)
#define PLATFORM_SPI_CONFIG_SIZE  0x00004000

#endif

