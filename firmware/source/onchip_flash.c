#include <string.h>
#include "stm32f10x.h"
#include "onchip_flash.h"
#include "utils.h"
#include "assert.h"

/* 将配置写入片内flash */
int FLASH_Write(u32 addr, void* buf, int size)
{
	FLASH_Status status;
	u32 word_count;
	u32 dest_addr;
	int ret;
 	int i;

	/* 写入数据
	   原数据不为字对齐时会写超，暂不处理，原因如下：
	   1、将一点无用数据写入falsh不会有不良影响，读取的时候是按照字节读取的，多写入的数据不会被读出；
	   2、STM32没有内存访问越界检查，所以越界访问内存应该不会造成问题
	 */
	word_count = size / 4;
	if (0 != (size % 4)){
		word_count += 1;
	}
	
	ret = ERROR_SUCCESS;
	dest_addr = (u32)buf;
	for (i = 0; i < word_count; ++i){
		status = FLASH_ProgramWord(addr + i * 4, *(u32*)(dest_addr	+ i * 4));
		if (FLASH_COMPLETE != status){
			ret = ERROR_FAILED;
			break;
		}
	}

	return ret;
}

int FLASH_Erase(u32 addr, u32 size)
{
	FLASH_Status status;
	u32 count;
	int ret;
	int i;
	
	count = size / FLASH_PAGE_SIZE;
	if (0 != size % FLASH_PAGE_SIZE){
		count++;
	}

	ret = ERROR_SUCCESS;
	for (i = 0; i < count; ++i){
		status = FLASH_ErasePage(addr + i * FLASH_PAGE_SIZE);
		if (FLASH_COMPLETE != status){
			ret = ERROR_FAILED;
			break;
		}
	}

	return ret;
}
