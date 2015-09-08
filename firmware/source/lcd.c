#include "stm32f10x.h"
#include "public.h"
#include "lcd.h"

#define LED_WIDTH_PIXEL  128
#define LED_PAGES        8

#define LCD_MOSI_RCC	  RCC_APB2Periph_GPIOA
#define LCD_MOSI_PORT	  GPIOA
#define LCD_MOSI_PIN	  GPIO_Pin_7

#define LCD_CLK_RCC		  RCC_APB2Periph_GPIOA
#define LCD_CLK_PORT	  GPIOA
#define LCD_CLK_PIN		  GPIO_Pin_5

#define LCD_RST_RCC		  RCC_APB2Periph_GPIOA
#define LCD_RST_PORT	  GPIOA
#define LCD_RST_PIN		  GPIO_Pin_3

#define LCD_DC_RCC		  RCC_APB2Periph_GPIOA
#define LCD_DC_PORT		  GPIOA
#define LCD_DC_PIN 		  GPIO_Pin_4

#define LCD_SPI           SPI1
#define LCD_SPI_RCC       RCC_APB2Periph_SPI1
#define LCD_DMA_CHANNEL   DMA1_Channel3
#define LCD_DMA_TC_FLAG   DMA1_FLAG_TC3

static uint8_t g_framebuffer[LED_PAGES][LED_WIDTH_PIXEL];

static void _gpioInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(LCD_MOSI_RCC|LCD_CLK_RCC|LCD_RST_RCC|LCD_DC_RCC, ENABLE);

	GPIO_InitStructure.GPIO_Pin = LCD_MOSI_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(LCD_MOSI_PORT, &GPIO_InitStructure);	

	GPIO_InitStructure.GPIO_Pin = LCD_CLK_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(LCD_CLK_PORT, &GPIO_InitStructure);	

	GPIO_InitStructure.GPIO_Pin = LCD_RST_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(LCD_RST_PORT, &GPIO_InitStructure);
	GPIO_SetBits(LCD_RST_PORT, LCD_RST_PIN);	

	GPIO_InitStructure.GPIO_Pin = LCD_DC_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(LCD_DC_PORT, &GPIO_InitStructure);	

	return;
}

static void _spiInit(void)
{
	SPI_InitTypeDef  SPI_InitStructure;

	RCC_APB2PeriphClockCmd(LCD_SPI_RCC, ENABLE);

	/* SPI1 configuration */ 
	SPI_InitStructure.SPI_Direction = SPI_Direction_1Line_Tx; 
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;	                   
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;                  
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;	 		               
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;		               
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;			               
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2; 
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;				   
	SPI_InitStructure.SPI_CRCPolynomial = 7;						   
	SPI_Init(LCD_SPI, &SPI_InitStructure);
    SPI_I2S_DMACmd(LCD_SPI, SPI_I2S_DMAReq_Tx, ENABLE);
	SPI_Cmd(LCD_SPI, ENABLE);

	return;
}

static void _spi_writeByte(u8 txData)
{						 		  
	LCD_SPI->DR = txData;
	while (SPI_I2S_GetFlagStatus(SPI_FLASH_SPI, SPI_I2S_FLAG_RXNE) == RESET) { ; }

    return;
}

static void _lcdWriteCmd(unsigned char cmd)
{
	GPIO_ResetBits(LCD_DC_PORT, LCD_DC_PIN);
	_spi_writeByte(cmd);
	return;
}

static void _lcdSetup(void)
{	 
	/* reset led */
	GPIO_ResetBits(LCD_RST_PORT, LCD_RST_PIN);
	delay_ms(50);
	GPIO_SetBits(LCD_RST_PORT, LCD_RST_PIN);

	/* 关闭显示 */
	_lcdWriteCmd(0xae);// Turn off oled panel
	_lcdWriteCmd(0x40);// Set Display Start Line (40h~7Fh)
	
	/* 设置亮度 */
	_lcdWriteCmd(0x81);//--set contrast control register
	_lcdWriteCmd(0xcf);// Set SEG Output Current Brightness

	_lcdWriteCmd(0xa1);//--Set SEG/Column Mapping     0xa0左右反置 0xa1正常
	_lcdWriteCmd(0xc8);//Set COM/Row Scan Direction   0xc0上下反置 0xc8正常
	_lcdWriteCmd(0xa6);//--set normal display

	_lcdWriteCmd(0xa8);//--set multiplex ratio(1 to 64)
	_lcdWriteCmd(0x3f);

	_lcdWriteCmd(0xd3);//-set display offset	Shift Mapping RAM Counter (0x00~0x3F)
	_lcdWriteCmd(0x00);//-not offset

	_lcdWriteCmd(0xd5);//--set display clock divide ratio/oscillator frequency
	_lcdWriteCmd(0x80);//--set divide ratio, Set Clock as 100 Frames/Sec

	_lcdWriteCmd(0xd9);//--set pre-charge period
	_lcdWriteCmd(0xf1);//Set Pre-Charge as 15 Clocks & Discharge as 1 Clock

	_lcdWriteCmd(0xda);//--set com pins hardware configuration
	_lcdWriteCmd(0x12);

	_lcdWriteCmd(0xdb);//--set vcomh
	_lcdWriteCmd(0x40);//Set VCOM Deselect Level

	_lcdWriteCmd(0x20);//-Set Page Addressing Mode (0x00/0x01/0x02)
	_lcdWriteCmd(0x00);//使用水平模式

	//设置行扫描范围和page扫描范围
	_lcdWriteCmd(0x21);//-Set Column Address Range
	_lcdWriteCmd(0x00);
	_lcdWriteCmd(0x7F);

	_lcdWriteCmd(0x22);
	_lcdWriteCmd(0x00);//-Set Page Address Rage
	_lcdWriteCmd(0x07);

	_lcdWriteCmd(0x8d);//--set Charge Pump enable/disable
	_lcdWriteCmd(0x14);//--set(0x10) disable
	_lcdWriteCmd(0xa4);// Disable Entire Display On (0xa4/0xa5)
	_lcdWriteCmd(0xa6);// Disable Inverse Display On (0xa6/a7) 
	_lcdWriteCmd(0xaf);//--turn on oled panel

	return;
}

static void _flushhBuffer(void)
{
    DMA_InitTypeDef DMA_InitStructure;  

    //wait last transfer finish
    while(!DMA_GetFlagStatus(LCD_DMA_TC_FLAG));
    DMA_Cmd(LCD_DMA_CHANNEL, DISABLE);  //TODO, verify if this is necessary
    
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(LCD_SPI->DR);
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)g_framebuffer;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
    DMA_InitStructure.DMA_BufferSize = sizeof(g_framebuffer);
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(LCD_DMA_CHANNEL, &DMA_InitStructure);

    DMA_Cmd(LCD_DMA_CHANNEL, ENABLE);
    return;
}


