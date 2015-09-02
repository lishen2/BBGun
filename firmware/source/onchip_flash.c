#include <string.h>
#include "stm32f10x.h"
#include "onchip_flash.h"
#include "utils.h"
#include "assert.h"

/* ������д��Ƭ��flash */
int FLASH_Write(u32 addr, void* buf, int size)
{
	FLASH_Status status;
	u32 word_count;
	u32 dest_addr;
	int ret;
 	int i;

	/* д������
	   ԭ���ݲ�Ϊ�ֶ���ʱ��д�����ݲ�����ԭ�����£�
	   1����һ����������д��falsh�����в���Ӱ�죬��ȡ��ʱ���ǰ����ֽڶ�ȡ�ģ���д������ݲ��ᱻ������
	   2��STM32û���ڴ����Խ���飬����Խ������ڴ�Ӧ�ò����������
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
