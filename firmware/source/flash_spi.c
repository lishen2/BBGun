#include "spi_flash.h"

#define SPI_FLASH_SPI    		SPI1
#define SPI_FLASH_PIN_CS		GPIO_Pin_0
#define SPI_FLASH_PIN_SCK		GPIO_Pin_5
#define SPI_FLASH_PIN_MISO      GPIO_Pin_6
#define SPI_FLASH_PIN_MOSI		GPIO_Pin_7

#define SPI_FLASH_PORT_CS		GPIOA
#define SPI_FLASH_PORT_SCK		GPIOA
#define SPI_FLASH_PORT_MISO     GPIOA
#define SPI_FLASH_PORT_MOSI		GPIOA

#define SPI_FLASH_CLK_CS		RCC_APB2Periph_GPIOA
#define SPI_FLASH_CLK_SCK		RCC_APB2Periph_GPIOA
#define SPI_FLASH_CLK_MISO      RCC_APB2Periph_GPIOA
#define SPI_FLASH_CLK_MOSI		RCC_APB2Periph_GPIOA

/* Port Controls */
#define CS_HIGH()					SPI_FLASH_PORT_CS->BSRR   = SPI_FLASH_PIN_CS		/* MMC CS = H */
#define CS_LOW()					SPI_FLASH_PORT_CS->BRR    = SPI_FLASH_PIN_CS		/* MMC CS = L */

/* STM32 SPI Peripheral Settings */
#define SPI_PERIF_CLK(x)        	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1,x);
#define SPI_GPIO_CLK(x)				RCC_APB2PeriphClockCmd(SPI_FLASH_CLK_CS,x); 		\
									RCC_APB2PeriphClockCmd(SPI_FLASH_CLK_SCK,x); 		\
									RCC_APB2PeriphClockCmd(SPI_FLASH_CLK_MISO,x); 		\
									RCC_APB2PeriphClockCmd(SPI_FLASH_CLK_MOSI,x);		\
									RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,x)

//FLASH command code (w23q32)
#define FLASH_COM_WRITE_ENABLE		0x06
#define FLASH_COM_WRITE_DISABLE		0x04
#define FLASH_COM_READ_STATUS		0x05
#define FLASH_COM_WRITE_STATUS		0x01
#define FLASH_COM_READ_DATA			0x03
#define FLASH_COM_PAGE_PROGRAM		0x02
#define FLASH_COM_SECTOR_ERASE		0x20
#define FLASH_COM_BLOCK_ERASE		0xD8

#define FLASH_TIMEOUT_OTHER   200
#define FLASH_TIMEOUT_ERASE   150

static inline uint8_t _spi_transmit(uint8_t out)
{
	/* Send byte through the SPIMMC peripheral */
	SPI_I2S_SendData(SPI_FLASH_SPI, out);

	/* Wait to receive a byte */
	while (SPI_I2S_GetFlagStatus(SPI_FLASH_SPI, SPI_I2S_FLAG_RXNE) == RESET) { ; }

	/* Return the byte read from the SPI bus */
	return SPI_I2S_ReceiveData(SPI_FLASH_SPI);
}

static void _spi_init(void)
{
	SPI_InitTypeDef  SPI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	/* Enable SPI & GPIO clocks */
	SPI_GPIO_CLK(ENABLE);
	SPI_PERIF_CLK(ENABLE);

	/* Configure I/O for Flash Chip select */
	GPIO_InitStructure.GPIO_Pin		= SPI_FLASH_PIN_CS;
	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_50MHz;
	GPIO_Init(SPI_FLASH_PORT_CS, &GPIO_InitStructure);

	/* Deselect the Card: Chip Select high */
	CS_HIGH();

	/* SPI SCK pin configuration */
	GPIO_InitStructure.GPIO_Pin = SPI_FLASH_PIN_SCK;
    GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_50MHz;
	GPIO_Init(SPI_FLASH_PORT_SCK, &GPIO_InitStructure);

	/* SPI MOSI pin configuration */
	GPIO_InitStructure.GPIO_Pin =  SPI_FLASH_PIN_MOSI;
	GPIO_Init(SPI_FLASH_PORT_MOSI, &GPIO_InitStructure);

	/* SPI MISO pin configuration */
	GPIO_InitStructure.GPIO_Pin =  SPI_FLASH_PIN_MISO;
	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_IPU;
	GPIO_Init(SPI_FLASH_PORT_MISO, &GPIO_InitStructure);

	/* SPIMMC configuration */
	SPI_InitStructure.SPI_Direction	 = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode		 = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize	 = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL		 = SPI_CPOL_High;
	SPI_InitStructure.SPI_CPHA		 = SPI_CPHA_2Edge;
	SPI_InitStructure.SPI_NSS		 = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
	SPI_InitStructure.SPI_FirstBit	 = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init(SPI_FLASH_SPI, &SPI_InitStructure);

	SPI_CalculateCRC(SPI_FLASH_SPI, DISABLE);

	/* Enable SPIx  */
	SPI_Cmd(SPI_FLASH_SPI, ENABLE);

	/* drain SPI */
	while (SPI_I2S_GetFlagStatus(SPI_FLASH_SPI, SPI_I2S_FLAG_TXE) == RESET) { ; }
	SPI_I2S_ReceiveData(SPI_FLASH_SPI);
	
	return;
}

static uint32_t _w25_wait_ready( uint32_t timeout )
{
	uint32_t rcode;
    uint8_t response;

    rcode = ERR_OK;
    timeout += g_jiffies;
	while( 1 )
	{
	    CS_LOW();
        _spi_transmit(FLASH_COM_READ_STATUS);
        response = _spi_transmit(0xFF);
        CS_HIGH();
        
		if( ( response & 0x01 ) != 0x01 ){ 
            rcode = ERR_OK;
		    break; 
        }
        
        if (time_after(timeout, g_jiffies)){
            rcode = ERR_FAILED;
            break;
        }
    }

	return rcode;
}

static uint32_t _w25_disable_wp( void )
{
    CS_LOW();
    _spi_transmit(FLASH_COM_WRITE_ENABLE);
    CS_HIGH();

    return _w25_wait_ready(FLASH_TIMEOUT_OTHER);
}

static uint32_t _w25_page_programm(uint32_t addr, uint32_t size, uint8_t *buffer)
{
    uint32_t rcode;
    int i;

    //disable write protect
    rcode = _w25_disable_wp();
    if ( ERR_OK != rcode ) {
        return rcode;
    }

	//send command and address
	CS_LOW();
    _spi_transmit(FLASH_COM_PAGE_PROGRAM);
    _spi_transmit(addr >> 16);
    _spi_transmit(addr >> 8);
    _spi_transmit(addr >> 0);

    //send data
    for (i = 0; i < size; ++i){
        _spi_transmit(buffer[i]);
    }
    CS_HIGH();

    return  _w25_wait_ready(FLASH_TIMEOUT_ERASE);
}

static uint32_t _w25_sector_erase(uint32_t addr)
{
    uint32_t rcode;

    rcode = _w25_disable_wp();
    if ( ERR_OK != rcode ) {
        return rcode;
    }

	CS_LOW();
    _spi_transmit(FLASH_COM_SECTOR_ERASE);
    _spi_transmit(addr >> 16);
    _spi_transmit(addr >> 8);
    _spi_transmit(addr >> 0);
    CS_HIGH();

    return  _w25_wait_ready(FLASH_TIMEOUT_ERASE);
}

static uint32_t _w25_block_erase(uint32_t addr)
{
    uint32_t rcode;

    rcode = _w25_disable_wp();
    if ( ERR_OK != rcode ) {
        return rcode;
    }

	CS_LOW();
    _spi_transmit(FLASH_COM_BLOCK_ERASE);
    _spi_transmit(addr >> 16);
    _spi_transmit(addr >> 8);
    _spi_transmit(addr >> 0);
    CS_HIGH();

    return  _w25_wait_ready(FLASH_TIMEOUT_ERASE);
}

void flash_spi_init(void)
{
    _spi_init();

    return;
}

int flash_spi_erase_block(uint32_t addr, uint32_t len)
{
	uint32_t rcode;
    uint32_t end;

    //检查地址合法性
    if ((FLASH_SPI_ERASE_BLOCK_ADDR_MASK & addr) != addr){
        return ERR_FAILED;
    }

    //检测FLASH是否准备OK
    rcode = _w25_wait_ready(FLASH_TIMEOUT_OTHER);
    if ( ERR_OK != rcode ) {
        return rcode;
    }

    //计算结束块的地址
    end = (addr + len) & FLASH_SPI_ERASE_BLOCK_ADDR_MASK;
    if (len == (len & FLASH_SPI_ERASE_BLOCK_ADDR_MASK)){
        end -= FLASH_SPI_ERASE_BLOCK_SIZE;
    }
    //执行擦除
    for (addr &= FLASH_SPI_ERASE_BLOCK_ADDR_MASK;
         addr <= end; 
         addr += FLASH_SPI_ERASE_BLOCK_SIZE){
            
        rcode = _w25_block_erase(addr);
        if (ERR_OK != rcode){
            break;
        }
    }

    return rcode;
}

int flash_spi_erase_sector(uint32_t addr, uint32_t len)
{
	uint32_t rcode;
    uint32_t end;

    //检查地址合法性
    if ((FLASH_SPI_ERASE_SECTOR_ADDR_MASK & addr) != addr){
        return ERR_FAILED;
    }

    //检测FLASH是否准备OK
    rcode = _w25_wait_ready(FLASH_TIMEOUT_OTHER);
    if ( ERR_OK != rcode ) {
        return rcode;
    }

    //计算结束块的地址
    end = (addr + len) & FLASH_SPI_ERASE_SECTOR_ADDR_MASK;
    if (len == (len & FLASH_SPI_ERASE_SECTOR_ADDR_MASK)){
        end -= FLASH_SPI_ERASE_SECTOR_SIZE;
    }
    //执行擦除
    for (addr &= FLASH_SPI_ERASE_SECTOR_ADDR_MASK;
         addr <= end; 
         addr += FLASH_SPI_ERASE_SECTOR_SIZE){
            
        rcode = _w25_sector_erase(addr);
        if (ERR_OK != rcode){
            break;
        }
    }

    return rcode;
}

int flash_spi_read(uint32_t addr, uint32_t len, uint8_t *data)
{
	uint32_t rcode;
    int i;

    rcode = _w25_wait_ready(FLASH_TIMEOUT_OTHER);
    if ( ERR_OK != rcode ) {
        return rcode;
    }

    //发送命令
	CS_LOW();
    _spi_transmit(FLASH_COM_READ_DATA);
    _spi_transmit(addr >> 16);
    _spi_transmit(addr >> 8);
    _spi_transmit(addr >> 0);
      
    //读取数据
    for (i = 0; i < len; ++i){
        data[i] = _spi_transmit(0xFF);
    }
    CS_HIGH();  

    return ERR_OK;
}

int flash_spi_write(uint32_t addr, uint32_t len, uint8_t *data)
{
	uint32_t rcode;
    uint32_t end;

    //检查地址合法性
    if ((FLASH_SPI_WRITE_ADDR_MASK & addr) != addr){
        return ERR_FAILED;
    }

    //检测FLASH是否准备OK
    rcode = _w25_wait_ready(FLASH_TIMEOUT_OTHER);
    if ( ERR_OK != rcode ) {
        return rcode;
    }

    //计算结束块的地址
    end = (addr + len) & FLASH_SPI_WRITE_ADDR_MASK;
    if (len == (len & FLASH_SPI_WRITE_ADDR_MASK)){
        end -= FLASH_SPI_WRITE_SIZE;
    }
    
     //执行写入
    for (addr &= FLASH_SPI_WRITE_ADDR_MASK;
         addr <= end; 
         addr += FLASH_SPI_WRITE_SIZE){

        //最后一块，且长度不为blocksize整数倍时
        if ((addr == end) && (len != (len & FLASH_SPI_WRITE_ADDR_MASK))){
            rcode = _w25_page_programm(addr, len % FLASH_SPI_WRITE_SIZE, data);
        } else {
            rcode = _w25_page_programm(addr, FLASH_SPI_WRITE_SIZE, data);
        }
        
        if (ERR_OK != rcode){
            break;
        }

        data += FLASH_SPI_WRITE_SIZE;
    } 

    return rcode;
}

